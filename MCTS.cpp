//#include "MCTS.h"
//#include "Node.h"
//
//#include "torch/torch.h"
//#include "ResNet_Model.h"
//
//template<typename State, typename MoveContainer, typename EncodedState>
//vector<float> MCTS<State, MoveContainer, EncodedState>::search(vector<vector<int>> state)
//{
//	torch::NoGradGuard no_grad;
//	Node* root = new Node(game, state);
//	root->visit_count = 1; // to avoid division by zero in UCB calculation
//
//	// Adding Dirichlet noise to the root node to encourage exploration
//	pair<float, vector<float>> output = evaluate_model(model, game.get_encoded_state(root->state));
//
//	vector<float> policy = output.second;
//	vector<int> valid_moves = game.get_valid_moves(root->state);
//
//	torch::Tensor policy_tensor = torch::from_blob(policy.data(), { (int)policy.size() }, torch::kFloat32)
//		.clone()
//		.to(model.device);
//
//	torch::Tensor dirichlet = dirichlet_noise(policy.size(), DIRICHLET_ALPHA, model.device);
//	policy_tensor = (double)(1 - DIRICHLET_EPSILON) * policy_tensor + (double)DIRICHLET_EPSILON * dirichlet;
//
//	// Convert the modified policy tensor back to a vector<float>
//	policy = vector<float>(policy_tensor.data_ptr<float>(), policy_tensor.data_ptr<float>() + policy_tensor.numel());
//
//	for (int i = 0; i < game.action_size; i++) {
//		policy[i] *= valid_moves[i];
//	}
//
//	float sum = 0.0;
//	for (int i = 0; i < game.action_size; i++) {
//		sum += policy[i];
//	}
//
//	if (sum > 0) {
//		for (int i = 0; i < game.action_size; i++) {
//			policy[i] /= sum;
//		}
//	}
//
//	root->expand(policy); // expands the root node with the policy probabilities from the model
//
//	for (int i = 0; i < NUM_SEARCHES; i++) {
//
//		//cout << "Search " << i + 1 << endl;
//		Node* node = root;
//
//		// Selection Phase
//		while (node->is_fully_expanded()) { // goes down as far as the terminal state
//			node = node->select();
//		}
//
//		float value = 0;
//		bool is_terminal = 0; // if terminal it means opponent has won not the player of the node
//
//		tie(value, is_terminal) = game.get_value_and_terminated(node->state, node->action);
//		value = game.get_opponent_value(value); // since the value is from the perspective of the opponent, we need to get the opponent value to get the value from the perspective of the player of the node
//
//		// Expansion Phase
//		if (!is_terminal) {
//			
//			tie(value, policy) = evaluate_model(model, game.get_encoded_state(node->state));
//
//			//cout << "Evaluation Phase: Evaluated action " << node->action << " with value " << value << endl;
//
//			valid_moves = game.get_valid_moves(node->state);
//
//			for (int i = 0; i < game.action_size; i++) {
//				policy[i] *= valid_moves[i]; // mask invlaid moves to 0 probability
//			}
//
//			float sum = 0.0;
//			for (int i = 0; i < game.action_size; i++) {
//				sum += policy[i];
//			}
//
//			if (sum > 0) {
//				for (int i = 0; i < game.action_size; i++) {
//					policy[i] /= sum; // normalize the probabilities
//				}
//			}
//			
//			node->expand(policy);
//
//			/*cout << "Expansion Phase: Expanded action " << node->action << endl;*/
//		}
//
//		// Backpropagation Phase
//
//		node->backpropagate(value); // backpropagates the value up the tree, updating the visit count and value count of each node
//
//		//cout << "Backpropagation Phase: Backpropagated value " << node->action << endl;
//	}
//
//	vector<float> action_probabilities(game.action_size, 0);
//	sum = 0;
//
//	for (auto& child : root->children) {
//		action_probabilities[child->action] = (float)child->visit_count; // the probability of selecting an action is proportional to the visit count of the child node
//		sum += child->visit_count;
//	}
//
//	for (auto& child : root->children)
//		action_probabilities[child->action] /= sum; // normalize the probabilities
//
//	return action_probabilities; 
//}
//
//torch::Tensor dirichlet_noise(int64_t size, float alpha, torch::Device& device)
//{
//	torch::Tensor concentration = torch::full({ size }, alpha,
//		torch::TensorOptions().dtype(torch::kFloat32).device(device));
//
//	torch::Tensor gamma_samples = torch::zeros_like(concentration);
//	gamma_samples = at::_standard_gamma(concentration);
//
//	return gamma_samples / gamma_samples.sum();
//}