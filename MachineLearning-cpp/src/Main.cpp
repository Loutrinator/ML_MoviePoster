#include <iostream>
#include "NeuralNetwork.h"
#include "Data.h"

int main()
{
	NeuralNetwork network;
	network.addLayer(2, OutputFunction::LINEAR);
	network.addLayer(1, OutputFunction::LINEAR);
	
	std::vector<Data> dataset(30);
	for(Data& data : dataset)
	{
		data.input = {0.5,0.12};
		data.expectedOutput = {1};
	}
	
	network.train(dataset);
	
	for(Data& data : dataset)
	{
		std::cout << "My Output : \n";
		for(float out : data.output)
		{
			std::cout << out << ", ";
		}
		std::cout << std::endl;
	}
}
