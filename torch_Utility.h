//
//#include "torch/torch.h"
//#include <vector>
//#include "ResNet_Model.h"
//
//using namespace std;
//
//torch::Tensor convert_to_tensor(vector<vector<vector<float>>> encoded_state) {
//	int C = encoded_state.size();
//	int H = encoded_state[0].size();
//	int W = encoded_state[0][0].size();
//
//	std::vector<float> flat;
//	flat.reserve(C * H * W);
//	for (auto& matrix : encoded_state) {
//		for (auto& row : matrix)
//			flat.insert(flat.end(), row.begin(), row.end());
//	}
//	return torch::from_blob(flat.data(), { C, H, W }, torch::kFloat32)
//		.clone()
//		.unsqueeze(0);
//}
//
//pair<float, vector<float>> evaluate_model(ResNet& model, vector<vector<vector<float>>> encoded_state) {
//	torch::Tensor tensor_state = convert_to_tensor(encoded_state);
//	pair<torch::Tensor, torch::Tensor> output = model.forward(tensor_state);
//	torch::Tensor policy_tensor = torch::softmax(output.first, 1).squeeze().detach().cpu().to(torch::kFloat32).contiguous();
//
//	float* policy_ptr = policy_tensor.data_ptr<float>();
//	std::vector<float> policy(policy_ptr, policy_ptr + policy_tensor.numel());
//	float value = output.second.squeeze().item<float>();
//	return { value, policy };
//}