#pragma once

#ifndef ResNetModel
#define ResNetModel

#include "torch/torch.h"
#include "TicTacToe.h"

class ResNet : public torch::nn::Module {
	torch::nn::Sequential startBlock{ nullptr };
	torch::nn::ModuleList backBone{ nullptr };
	torch::nn::Sequential policyHead{ nullptr };
	torch::nn::Sequential valueHead{ nullptr };

public:
	torch::Device device;

	template<typename State, typename MoveContainer, typename EncodedState>
	ResNet(IGame<State, MoveContainer, EncodedState> &game, int num_resBlocks, int num_hidden, torch::Device& device);

	pair<torch::Tensor, torch::Tensor> forward(torch::Tensor &x);

};

class ResBlock : public torch::nn::Module {
	torch::nn::Conv2d conv1{nullptr};
	torch::nn::BatchNorm2d bn1{ nullptr };

	torch::nn::Conv2d conv2{ nullptr };
	torch::nn::BatchNorm2d bn2{ nullptr };

public:
	ResBlock(int num_hidden){
		conv1 = register_module("conv1", torch::nn::Conv2d(torch::nn::Conv2dOptions(num_hidden, num_hidden, 3).padding(1)));
		bn1 = register_module("bn1", torch::nn::BatchNorm2d(num_hidden));
		conv2 = register_module("conv2", torch::nn::Conv2d(torch::nn::Conv2dOptions(num_hidden, num_hidden, 3).padding(1)));
		bn2 = register_module("bn2", torch::nn::BatchNorm2d(num_hidden));
	}

	torch::Tensor forward(torch::Tensor x) {
		torch::Tensor residual = x;
		x = torch::relu(bn1(conv1(x)));
		x = bn2(conv2(x));

		x += residual;
		return torch::relu(x);
	}
};


inline torch::Tensor convert_to_tensor(vector<vector<vector<float>>> encoded_state, torch::Device& device) {
	int C = encoded_state.size();
	int H = encoded_state[0].size();
	int W = encoded_state[0][0].size();

	// Create a tensor of the correct shape first
	torch::Tensor tensor = torch::zeros({ 1, C, H, W }, torch::kFloat32);

	// Copy the data into the tensor's memory directly
	auto accessor = tensor.accessor<float, 4>();
	for (int c = 0; c < C; ++c) {
		for (int h = 0; h < H; ++h) {
			for (int w = 0; w < W; ++w) {
				accessor[0][c][h][w] = encoded_state[c][h][w];
			}
		}
	}
	return tensor.to(device);
}

inline pair<float, vector<float>> evaluate_model(ResNet& model, vector<vector<vector<float>>> encoded_state) {
	torch::Tensor tensor_state = convert_to_tensor(encoded_state, model.device);
	pair<torch::Tensor, torch::Tensor> output = model.forward(tensor_state);
	torch::Tensor policy_tensor = torch::softmax(output.first, 1).squeeze(0).detach().cpu().to(torch::kFloat32).contiguous();

	float* policy_ptr = policy_tensor.data_ptr<float>();
	std::vector<float> policy(policy_ptr, policy_ptr + policy_tensor.numel());
	float value = output.second.squeeze().item<float>();
	return { value, policy };
}

inline std::pair<std::vector<float>, std::vector<std::vector<float>>>evaluate_model_parallel(ResNet& model,const vector<vector<vector<vector<float>>>>& encoded_states)
{
	// encoded_states layout: [batch, channels, rows, cols]
	int batch = (int)encoded_states.size();
	if (batch == 0) return { {}, {} };

	std::vector<torch::Tensor> tensors;
	tensors.reserve(batch);
	for (const auto& state : encoded_states) {
		tensors.push_back(convert_to_tensor(state, model.device).squeeze(0));
	}

	torch::Tensor state_tensor = torch::stack(tensors, 0).to(torch::kFloat32);
	auto [policy_tensor_raw, value_tensor_raw] = model.forward(state_tensor);

	torch::Tensor policy_tensor = torch::softmax(policy_tensor_raw, 1)
		.detach().cpu().to(torch::kFloat32);

	policy_tensor = policy_tensor.contiguous();

	torch::Tensor value_tensor = value_tensor_raw.squeeze(-1)  // -> [batch]
		.detach().cpu().to(torch::kFloat32).contiguous();

	int action_size = (int)policy_tensor.size(1);
	const float* policy_ptr = policy_tensor.data_ptr<float>();
	const float* value_ptr = value_tensor.data_ptr<float>();

	std::vector<float> values(value_ptr, value_ptr + batch);

	std::vector<std::vector<float>> policies(batch);
	for (int i = 0; i < batch; i++) {
		policies[i].assign(policy_ptr + i * action_size,
			policy_ptr + i * action_size + action_size);
	}

	return { values, policies };
}



template<typename State, typename MoveContainer, typename EncodedState>
ResNet::ResNet(IGame<State, MoveContainer, EncodedState>& game, int num_resBlocks, int num_hidden, torch::Device &device) : device(device) {

	backBone = register_module("backBone", torch::nn::ModuleList());

	startBlock = register_module("startBlock", torch::nn::Sequential(
		torch::nn::Conv2d(torch::nn::Conv2dOptions(3, num_hidden, 3).padding(1)),
		torch::nn::BatchNorm2d(num_hidden),
		torch::nn::ReLU()
	));

	for (int i = 0; i < num_resBlocks; i++) {
		backBone->push_back(ResBlock(num_hidden));
	}

	policyHead = register_module("policyHead", torch::nn::Sequential(
		torch::nn::Conv2d(torch::nn::Conv2dOptions(num_hidden, 32, 3).padding(1)),
		torch::nn::BatchNorm2d(32),
		torch::nn::ReLU(),
		torch::nn::Flatten(),
		torch::nn::Linear(32 * game.row_count * game.col_count, game.action_size)
	));

	valueHead = register_module("valueHead", torch::nn::Sequential(
		torch::nn::Conv2d(torch::nn::Conv2dOptions(num_hidden, 3, 3).padding(1)),
		torch::nn::BatchNorm2d(3),
		torch::nn::ReLU(),
		torch::nn::Flatten(),
		torch::nn::Linear(3 * game.row_count * game.col_count, 1),
		torch::nn::Tanh()
	));

	this->to(device);
};

#endif
