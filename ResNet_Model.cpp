#include "ResNet_Model.h"


pair<torch::Tensor, torch::Tensor> ResNet::forward(torch::Tensor x)
{
	x = startBlock->forward(x);
	for (auto& block : *backBone) {
		x = block->as<ResBlock>()->forward(x);
	}
	torch::Tensor policy = policyHead->forward(x);
	torch::Tensor value = valueHead->forward(x);

	return { policy, value };
}
