#pragma once

#include <vector>
#include <memory>
#include "IGame.h"
#include "ResNet_Model.h"
#include "Node.h"  

using namespace std;

template<typename State>
struct MemoryEntry {
    State          state;
    vector<float>  action_probs;
    int            player;

    MemoryEntry(State state, vector<float> action_probs, int player)
        : state(move(state)), action_probs(move(action_probs)), player(player) {
    }
};

struct Experience {
    vector<vector<vector<float>>> encoded_state;
    vector<float>                 action_probs;
    float                         reward;

    Experience(vector<vector<vector<float>>> state,
        vector<float> action_probs,
        float reward)
        : encoded_state(move(state))
        , action_probs(move(action_probs))
        , reward(reward) {
    }
};

template<typename State, typename MoveContainer, typename EncodedState>
struct SPG {
    State state;

    vector<MemoryEntry<State>> memory;

    unique_ptr<Node<State, MoveContainer, EncodedState>> root_owner;
    Node<State, MoveContainer, EncodedState>* root = nullptr;
    Node<State, MoveContainer, EncodedState>* node = nullptr;  // observer only

    explicit SPG(IGame<State, MoveContainer, EncodedState>& game)
        : state(game.get_initial_state())
    {
        root_owner = make_unique<Node<State, MoveContainer, EncodedState>>(game, state);
        root = root_owner.get();
        node = root;
    }
};


inline void apply_dirichlet_noise(vector<float>& policy,
    float alpha, float epsilon,
    torch::Device& device)
{
    int64_t size = (int64_t)policy.size();
    torch::Tensor concentration = torch::full({ size }, alpha,
        torch::TensorOptions().dtype(torch::kFloat32).device(device));
    torch::Tensor gamma_samples = at::_standard_gamma(concentration);

    torch::Tensor dirichlet = (gamma_samples / gamma_samples.sum())
        .cpu().contiguous();

    const float* dir_ptr = dirichlet.data_ptr<float>();
    float keep = 1.0f - epsilon;
    for (int i = 0; i < (int)policy.size(); i++) {
        policy[i] = keep * policy[i] + epsilon * dir_ptr[i];
    }
}

template<typename State, typename MoveContainer, typename EncodedState>
class MCTSParallel {
    IGame<State, MoveContainer, EncodedState>& game;
    ResNet& model;  

    void mask_and_normalize(vector<float>& policy, const MoveContainer& valid_moves) {
        float sum = 0.0f;
        for (int i = 0; i < (int)policy.size(); i++) {
            policy[i] *= valid_moves[i];
            sum += policy[i];
        }
        if (sum > 0.0f) {
            float inv_sum = 1.0f / sum;
            for (auto& p : policy) p *= inv_sum;
        }
    }

public:
    MCTSParallel(IGame<State, MoveContainer, EncodedState>& game, ResNet& model)
        : game(game), model(model) {
    }

    void search(const vector<State>& states,
        vector<SPG<State, MoveContainer, EncodedState>>& spgames);
};

template<typename State, typename MoveContainer, typename EncodedState>
void MCTSParallel<State, MoveContainer, EncodedState>::search(
    const vector<State>& states,
    vector<SPG<State, MoveContainer, EncodedState>>& spgames)
{
    torch::NoGradGuard no_grad;

    pair<vector<float>, vector<vector<float>>> output = evaluate_model_parallel(model, game.get_encoded_state_parallel(states));
    vector<float> root_values = output.first;
    vector <vector<float>>root_policies = output.second;

    for (int i = 0; i < (int)spgames.size(); i++) {
        apply_dirichlet_noise(root_policies[i], DIRICHLET_ALPHA, DIRICHLET_EPSILON, model.device);

        spgames[i].root_owner =
            make_unique<Node<State, MoveContainer, EncodedState>>(game, states[i]);
        spgames[i].root = spgames[i].root_owner.get();
        spgames[i].root->visit_count = 1;

        MoveContainer valid_moves = game.get_valid_moves(states[i]);
        mask_and_normalize(root_policies[i], valid_moves);
        spgames[i].root->expand(root_policies[i]);
    }

    for (int sim = 0; sim < NUM_SEARCHES; sim++) { 
        for (int j = 0; j < (int)spgames.size(); j++) {
            spgames[j].node = nullptr;
            Node<State, MoveContainer, EncodedState>* node = spgames[j].root;

            while (node->is_fully_expanded()) {
                node = node->select();
            }

            auto [value, is_terminal] = game.get_value_and_terminated(node->state, node->action);
            value = game.get_opponent_value(value);

            if (is_terminal)
                node->backpropagate(value);
            else
                spgames[j].node = node;
        }

        vector<int> expandable_idx;
        expandable_idx.reserve(spgames.size());
        for (int j = 0; j < (int)spgames.size(); j++) {
            if (spgames[j].node != nullptr)
                expandable_idx.push_back(j);
        }

        if (expandable_idx.empty()) continue;

        vector<State> expand_states;
        expand_states.reserve(expandable_idx.size());
        for (int idx : expandable_idx)
            expand_states.push_back(spgames[idx].node->state);

        auto [eval_values, policies] =
            evaluate_model_parallel(model, game.get_encoded_state_parallel(expand_states));

        for (int k = 0; k < (int)expandable_idx.size(); k++) {
            Node<State, MoveContainer, EncodedState>* node = spgames[expandable_idx[k]].node;
            vector<float> spg_policy = policies[k];
            float spg_value = eval_values[k];

            MoveContainer valid_moves = game.get_valid_moves(node->state);
            mask_and_normalize(spg_policy, valid_moves);

            node->expand(spg_policy);
            node->backpropagate(spg_value);
        }
    }
}
