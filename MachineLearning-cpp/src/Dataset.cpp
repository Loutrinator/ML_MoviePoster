#include "Dataset.h"

void Dataset::addData(std::vector<float>&& input, std::vector<float>&& expectedOutput)
{
	_data.emplace_back(std::forward<std::vector<float>>(input), std::forward<std::vector<float>>(expectedOutput));
}

std::vector<Data>& Dataset::data()
{
	return _data;
}

Dataset* Dataset_Create()
{
	return new Dataset;
}

void Dataset_Destroy(Dataset* ptr)
{
	delete ptr;
}

void Dataset_AddData(Dataset* ptr, float* input, int inputSize, float* expectedOutput, int outputSize)
{
	ptr->addData(std::vector<float>(input, input + inputSize), std::vector<float>(expectedOutput, expectedOutput + outputSize));
}

int Dataset_GetDataCount(Dataset* ptr)
{
	return ptr->data().size();
}

int Dataset_GetInputCount(Dataset* ptr)
{
	return ptr->data().front().input.size();
}

int Dataset_GetOutputCount(Dataset* ptr)
{
	return ptr->data().front().output.size();
}

float Dataset_GetInput(Dataset* ptr, int dataIndex, int inputIndex)
{
	return ptr->data()[dataIndex].input[inputIndex];
}

float Dataset_GetExpectedOutput(Dataset* ptr, int dataIndex, int expectecOutputIndex)
{
	return ptr->data()[dataIndex].expectedOutput[expectecOutputIndex];
}

float Dataset_GetOutput(Dataset* ptr, int dataIndex, int outputIndex)
{
	return ptr->data()[dataIndex].output[outputIndex];
}
