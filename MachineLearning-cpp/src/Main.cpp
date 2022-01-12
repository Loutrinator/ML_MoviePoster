#include <iostream>
#include "NeuralNetwork.h"
#include "Data.h"

int main()
{
	NeuralNetwork network;
    network.debugMode = false;
	network.addLayer(2, OutputFunction::SIGMOID);
    network.addLayer(3, OutputFunction::SIGMOID);
	network.addLayer(1, OutputFunction::SIGMOID);

    //Cas de test d'un AND logique
	std::vector<Data> dataset(4);
    dataset[0].input = {0, 0};
    dataset[0].expectedOutput = {0};
    dataset[1].input = {1, 0};
    dataset[1].expectedOutput = {1};
    dataset[2].input = {0, 1};
    dataset[2].expectedOutput = {1};
    dataset[3].input = {1, 1};
    dataset[3].expectedOutput = {0};

	
	network.train(dataset,100000,0.01,true);
	
	for(Data& data : dataset)
	{

        std::cout << "[" << data.input[0];
        for(int i = 1; i < data.input.size(); ++i){
            std::cout << ", " << data.input[i];
        }
        std::cout << "] -> [" << data.output[0];
        for(int i = 1; i < data.output.size(); ++i){
            std::cout << ", " << data.output[i];
        }
        std::cout << "]" << std::endl;
	}
}
