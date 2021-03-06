#include <iostream>
#include "NeuralNetwork.h"
#include "Data.h"

int main()
{
	NeuralNetwork network;
	network.addLayer(2, OutputFunction::LINEAR);
    network.addLayer(3, OutputFunction::SIGMOID);
	network.addLayer(1, OutputFunction::SIGMOID);

    //Cas de test d'un AND logique
	Dataset dataset;
	dataset.addData({0, 0}, {0});
	dataset.addData({1, 0}, {0});
	dataset.addData({0, 1}, {0});
	dataset.addData({1, 1}, {1});
	
	network.train(dataset,100000,0.01,true);

    Dataset test;
    test.addData({0, 0}, {0});
    test.addData({0.7, 0}, {0});
    test.addData({0, 0.5}, {0});
    test.addData({0.8, 0.9}, {1});

    float error = network.evaluate(test, 4, 0.25);
    std::cout << "Error " << error << std::endl;

	for(Data& data : test.data())
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
