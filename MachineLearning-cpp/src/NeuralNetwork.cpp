#include "NeuralNetwork.h"

static void linear(std::vector<float>& values)
{
	for (float& value : values)
	{
		value = value;
	}
}

void NeuralNetwork::addLayer(int neuronCount, OutputFunction outputFunction)
{
	std::function<void(std::vector<float>&)> function;
	
	switch(outputFunction)
	{
		case OutputFunction::LINEAR:
			function = linear;
			break;
	}
	
	_layers.emplace_back(std::make_pair(neuronCount, function));
	
	if (_layers.size() > 1)
	{
		int matrixWidth = _layers[_layers.size()-2].first + 1;
		int matrixHeight = _layers[_layers.size()-1].first;
		
		_matrices.emplace_back(matrixWidth, matrixHeight);
	}
}

void NeuralNetwork::compute(float* input, int inputSize, float* output)
{
	std::vector<float> values(input, input + inputSize);
	
	for (int i = 0; i < _matrices.size(); i++)
	{
		_layers[i].second(values);
		
		values.emplace_back(1.0f);
		
		values = _matrices[i] * values;
	}
	
	_layers.back().second(values);
	
	std::copy(values.begin(), values.end(), output);
}

int NeuralNetwork::getOutputSize() const
{
	return _layers.back().first;
}

void NeuralNetwork::debug_setValue(int matrixIndex, int x, int y, float value)
{
	_matrices[matrixIndex](x, y) = value;
}

NeuralNetwork* NeuralNetwork_Create()
{
	return new NeuralNetwork;
}

void NeuralNetwork_Destroy(NeuralNetwork* ptr)
{
	delete ptr;
}

void NeuralNetwork_AddLayer(NeuralNetwork* ptr, int neuronCount, OutputFunction outputFunction)
{
	ptr->addLayer(neuronCount, outputFunction);
}

int NeuralNetwork_GetOutputSize(NeuralNetwork* ptr)
{
	return ptr->getOutputSize();
}

void NeuralNetwork_Compute(NeuralNetwork* ptr, float* input, int inputSize, float* output)
{
	ptr->compute(input, inputSize, output);
}

void NeuralNetwork_Debug_SetValue(NeuralNetwork* ptr, int matrixIndex, int x, int y, float value)
{
	ptr->debug_setValue(matrixIndex, x, y, value);
}
