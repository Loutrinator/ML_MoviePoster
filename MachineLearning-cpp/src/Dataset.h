#pragma once

#include <span>
#include <vector>
#include "Data.h"
#include "API.h"

class Dataset
{
public:
	Dataset() = default;
	
	Dataset(const Dataset& other) = delete;
	Dataset& operator=(const Dataset& other) = delete;
	Dataset(Dataset&& other) = delete;
	Dataset& operator=(Dataset&& other) = delete;
	
	void addData(std::vector<float>&& input, std::vector<float>&& expectedOutput);
	
	std::vector<Data>& data();

private:
	std::vector<Data> _data;
};

API_EXPORT Dataset* Dataset_Create();
API_EXPORT void Dataset_Destroy(Dataset* ptr);
API_EXPORT void Dataset_AddData(Dataset* ptr, float* input, int inputSize, float* expectedOutput, int outputSize);
API_EXPORT int Dataset_GetDataCount(Dataset* ptr);
API_EXPORT int Dataset_GetInputCount(Dataset* ptr);
API_EXPORT int Dataset_GetOutputCount(Dataset* ptr);
API_EXPORT float Dataset_GetInput(Dataset* ptr, int dataIndex, int inputIndex);
API_EXPORT float Dataset_GetExpectedOutput(Dataset* ptr, int dataIndex, int expectecOutputIndex);
API_EXPORT float Dataset_GetOutput(Dataset* ptr, int dataIndex, int outputIndex);
