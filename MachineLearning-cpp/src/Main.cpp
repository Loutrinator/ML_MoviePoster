#include <iostream>
#include "NeuralNetwork.h"

int main()
{
	NeuralNetwork network;
	network.addLayer(2, OutputFunction::LINEAR);
	network.addLayer(1, OutputFunction::LINEAR);
	
	network.debug_setValue(0, 0, 0, 0.42);
	network.debug_setValue(0, 1, 0, 0.69);
	
	std::vector<float> input = {0.122415f, 0.878};
	float output;
	network.compute(input.data(), 2, &output);
	
	std::cout << output << std::endl;
}
