#pragma once

#include "Matrix.h"
#include <vector>
#include <utility>
#include <functional>
#include <vector>
#include "OutputFunction.h"

#define API_EXPORT extern "C" __declspec(dllexport)

class NeuralNetwork
{
public:
	NeuralNetwork() = default;
	
	NeuralNetwork(const NeuralNetwork& other) = delete;
	NeuralNetwork& operator=(const NeuralNetwork& other) = delete;
	NeuralNetwork(NeuralNetwork&& other) = delete;
	NeuralNetwork& operator=(NeuralNetwork&& other) = delete;
	
	void addLayer(int neuronCount, OutputFunction outputFunction);
	
	int getOutputSize() const;
	void compute(float* input, int inputSize, float* output);
	
	void debug_setValue(int matrixIndex, int x, int y, float value);

private:
	std::vector<std::pair<int, std::function<void(std::vector<float>&)>>> _layers;
	std::vector<Matrix<float>> _matrices;
};

API_EXPORT NeuralNetwork* NeuralNetwork_Create();
API_EXPORT void NeuralNetwork_Destroy(NeuralNetwork* ptr);
API_EXPORT void NeuralNetwork_AddLayer(NeuralNetwork* ptr, int neuronCount, OutputFunction outputFunction);
API_EXPORT int NeuralNetwork_GetOutputSize(NeuralNetwork* ptr);
API_EXPORT void NeuralNetwork_Compute(NeuralNetwork* ptr, float* input, int inputSize, float* output);
API_EXPORT void NeuralNetwork_Debug_SetValue(NeuralNetwork* ptr, int matrixIndex, int x, int y, float value);
