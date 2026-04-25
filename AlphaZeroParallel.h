#pragma once

#include "MCTS_Parallel.h"
#include <random>
#include <numeric>
#include <algorithm>

#define selfPlay_iterations  500
#define num_epochs           4
#define num_iterations       8
#define batch_size           128
#define temperature          1.25f   // > 1 = more exploration, < 1 = more exploitation
#define num_parallel_games   100

// ---------------------------------------------------------------------------
// getRandomAction
// FIX #7: accepts gen by reference — no per-call RNG construction
// ---------------------------------------------------------------------------
inline int getRandomAction(int actionSize,
    vector<float>& action_probs,
    mt19937& gen)
{
    discrete_distribution<> dist(action_probs.begin(), action_probs.end());
    return dist(gen);
}

// ---------------------------------------------------------------------------
// AlphaZeroParallel
// ---------------------------------------------------------------------------
template<typename State, typename MoveContainer, typename EncodedState>
class AlphaZeroParallel {
    IGame<State, MoveContainer, EncodedState>& game;
    ResNet& model;   // FIX #8: reference
    torch::optim::Adam                         optimizer;
    MCTSParallel<State, MoveContainer, EncodedState> mcts;

    // Single RNG instance for the whole object lifetime (FIX #7)
    mt19937 gen;

public:
    AlphaZeroParallel(IGame<State, MoveContainer, EncodedState>& game, ResNet& model);

    vector<Experience> self_play();
    void train(vector<Experience>& flat_memory);
    void learn();
};

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
template<typename State, typename MoveContainer, typename EncodedState>
AlphaZeroParallel<State, MoveContainer, EncodedState>::AlphaZeroParallel(
    IGame<State, MoveContainer, EncodedState>& game, ResNet& model): game(game), model(model), mcts(game, model), optimizer(model.parameters(),  torch::optim::AdamOptions(0.001).weight_decay(0.001)), gen(random_device{}()) {}

template<typename State, typename MoveContainer, typename EncodedState>
vector<Experience>
AlphaZeroParallel<State, MoveContainer, EncodedState>::self_play()
{
    vector<Experience> return_memory;
    int player = 1;
    vector<SPG<State, MoveContainer, EncodedState>> spgames;

    spgames.reserve(num_parallel_games);
    for (int i = 0; i < num_parallel_games; i++)
        spgames.emplace_back(game);

    while (!spgames.empty()) {

        vector<State> neutral_states;
        neutral_states.reserve(spgames.size());
        for (auto& spg : spgames) {
            neutral_states.push_back(game.change_perspective(spg.state, player));
        }

        mcts.search(neutral_states, spgames);

        for (int i = (int)spgames.size() - 1; i >= 0; i--) {
            SPG<State, MoveContainer, EncodedState>& spg = spgames[i];

            // Build raw visit-count distribution
            vector<float> action_probs(game.action_size, 0.0f);
            for (auto& child : spg.root->children) {
                action_probs[child->action] = (float)child->visit_count;
            }

            float sum = accumulate(action_probs.begin(), action_probs.end(), 0.0f);
            if (sum > 0.0f)
                for (auto& p : action_probs) p /= sum;

            // FIX #13: MemoryEntry is now templated on State
            spg.memory.emplace_back(spg.root->state, action_probs, player);

            // Temperature-scaled sampling
            vector<float> temperatured_probs = action_probs;
            float tsum = 0.0f;
            for (auto& prob : temperatured_probs) {
                prob = pow(prob, 1.0f / temperature);
                tsum += prob;
            }
            if (tsum > 0.0f)
                for (auto& prob : temperatured_probs) prob /= tsum;

            int action = getRandomAction(game.action_size, temperatured_probs, gen);

            spg.state = game.get_next_state(spg.state, action, player);

            auto [value, is_terminal] = game.get_value_and_terminated(spg.state, action);

            if (is_terminal) {
                for (auto& entry : spg.memory) {
                    int outcome = (entry.player == player) ? value : game.get_opponent_value(value);
                    return_memory.emplace_back(Experience(game.get_encoded_state(entry.state), entry.action_probs, (float)outcome));
                }
                spgames.erase(spgames.begin() + i);
                continue;
            }

            player = game.get_opponent(player);
        }
    }
    return return_memory;
}

// ---------------------------------------------------------------------------
// train  (unchanged logic, minor cleanups)
// ---------------------------------------------------------------------------
template<typename State, typename MoveContainer, typename EncodedState>
void AlphaZeroParallel<State, MoveContainer, EncodedState>::train(
    vector<Experience>& flat_memory)
{
    for (int idx = 0; idx < (int)flat_memory.size(); idx += batch_size) {
        int end = min((int)flat_memory.size(), idx + batch_size);
        int current_batch_size = end - idx;

        vector<vector<vector<vector<float>>>> states;
        vector<vector<float>> action_probs;
        vector<float> rewards;
        states.reserve(current_batch_size);
        action_probs.reserve(current_batch_size);
        rewards.reserve(current_batch_size);

        for (int i = idx; i < end; i++) {
            states.push_back(flat_memory[i].encoded_state);
            action_probs.push_back(flat_memory[i].action_probs);
            rewards.push_back(flat_memory[i].reward);
        }

        // Stack into one batch tensor
        vector<torch::Tensor> tensors;
        tensors.reserve(current_batch_size);
        for (auto& s : states)
            tensors.push_back(convert_to_tensor(s, model.device).squeeze(0));
        torch::Tensor state_tensor = torch::stack(tensors).to(torch::kFloat32);

        int prob_size = (int)action_probs[0].size();
        vector<float> flat_ap;
        flat_ap.reserve((size_t)current_batch_size * prob_size);
        for (const auto& probs : action_probs)
            flat_ap.insert(flat_ap.end(), probs.begin(), probs.end());

        torch::Tensor policy_targets =
            torch::from_blob(flat_ap.data(), { current_batch_size, prob_size }, torch::kFloat32)
            .clone().to(model.device);

        torch::Tensor value_target =
            torch::from_blob(rewards.data(), { current_batch_size, 1 }, torch::kFloat32)
            .clone().to(model.device);

        auto [out_policy, out_value] = model.forward(state_tensor);
        if (out_value.dim() == 1)
            out_value = out_value.unsqueeze(1);

        torch::Tensor policy_loss = torch::nn::functional::kl_div(
            torch::log_softmax(out_policy, 1),
            policy_targets,
            torch::nn::functional::KLDivFuncOptions().reduction(torch::kBatchMean));

        torch::Tensor value_loss = torch::nn::functional::mse_loss(out_value, value_target);
        torch::Tensor loss = policy_loss + value_loss;

        cout << "policy_loss: " << policy_loss.item<float>()
            << "  value_loss: " << value_loss.item<float>()
            << "  total_loss: " << loss.item<float>() << "\n";

        optimizer.zero_grad();
        loss.backward();
        optimizer.step();
    }
}

// ---------------------------------------------------------------------------
// learn
// ---------------------------------------------------------------------------
template<typename State, typename MoveContainer, typename EncodedState>
void AlphaZeroParallel<State, MoveContainer, EncodedState>::learn()
{
    for (int i = 0; i < num_iterations; i++) {
        cout << "\nIteration " << i + 1 << "/" << num_iterations << "\n";

        vector<Experience> flat_memory;

        model.eval();
        {
            torch::NoGradGuard no_grad;
            for (int j = 0; j < selfPlay_iterations / num_parallel_games; j++) {
                cout << "Self-play batch " << j + 1
                    << "/" << selfPlay_iterations / num_parallel_games << "\n";
                vector<Experience> game_exp = self_play();
                flat_memory.insert(flat_memory.end(),
                    make_move_iterator(game_exp.begin()),
                    make_move_iterator(game_exp.end()));
            }
        }

        shuffle(flat_memory.begin(), flat_memory.end(), gen);

        for (int j = 0; j < num_epochs; j++) {
            cout << "Training epoch " << j + 1 << "/" << num_epochs << "\n";
            model.train();
            train(flat_memory);
        }

        cout << "Saving model...\n";
        torch::serialize::OutputArchive archive;
        model.save(archive);
        archive.save_to("model_" + to_string(i) + game.get_name() + ".pt");

        torch::serialize::OutputArchive opt_archive;
        optimizer.save(opt_archive);
        opt_archive.save_to("optimizer_" + to_string(i) + game.get_name() + ".pt");
    }
}