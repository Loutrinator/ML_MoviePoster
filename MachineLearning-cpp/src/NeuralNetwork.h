#pragma once

#include "Matrix.h"
#include "Data.h"
#include <vector>
#include <utility>
#include <functional>
#include <vector>
#include <span>
#include "OutputFunction.h"
#include "API.h"
#include "Dataset.h"

class NeuralNetwork
{
public:
	NeuralNetwork(bool debug = false);
	
	NeuralNetwork(const NeuralNetwork& other) = delete;
	NeuralNetwork& operator=(const NeuralNetwork& other) = delete;
	NeuralNetwork(NeuralNetwork&& other) = delete;
	NeuralNetwork& operator=(NeuralNetwork&& other) = delete;
	
	void addLayer(int neuronCount, OutputFunction outputFunction);
	
	int getOutputSize() const;
	void compute(std::span<float> input, std::span<float> output);
	void train(Dataset& dataset, int iterations, float alpha, bool isClassification);
    float evaluate(Dataset& dataset, int iterations, float diffThreshold);

	void debug_setValue(int matrixIndex, int x, int y, float value);

private:
	std::vector<std::pair<int, std::function<void(std::vector<float>&)>>> _layers;
    std::vector<Matrix<float>> _matrices;
    std::vector<std::vector<float>> _valuesVector;
	bool _debugMode;
	
    int nbLayers();
	
	void backpropagate(std::vector<float>& expectedOutput, std::vector<float>& output, std::vector<std::vector<float>>& deltasVector, float alpha, bool isClassification);
    void printNetwork() const;

};

API_EXPORT NeuralNetwork* NeuralNetwork_Create(bool debug);
API_EXPORT void NeuralNetwork_Destroy(NeuralNetwork* ptr);
API_EXPORT void NeuralNetwork_AddLayer(NeuralNetwork* ptr, int neuronCount, OutputFunction outputFunction);
API_EXPORT int NeuralNetwork_GetOutputSize(NeuralNetwork* ptr);
API_EXPORT void NeuralNetwork_Compute(NeuralNetwork* ptr, float* input, int inputSize, float* output, int outputSize);
API_EXPORT void NeuralNetwork_Debug_SetValue(NeuralNetwork* ptr, int matrixIndex, int x, int y, float value);
API_EXPORT void NeuralNetwork_Train(NeuralNetwork* ptr, Dataset* dataset, int iterations, float alpha, bool isClassification);
API_EXPORT float NeuralNetwork_Evaluate(NeuralNetwork* ptr, Dataset* dataset, int iterations, float diffThreshold);
