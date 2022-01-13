#pragma once

#include <vector>
#include <span>

struct Data
{
	std::vector<float> input; //de la meme taille que les inputs de notre network
	std::vector<float> expectedOutput; //de la meme taille que les outputs de notre network
	std::vector<float> output; //pareil
	
	Data(std::vector<float>&& input, std::vector<float>&& expectedOutput):
	input(std::forward<std::vector<float>>(input)), expectedOutput(std::forward<std::vector<float>>(expectedOutput))
	{
		output.resize(this->expectedOutput.size());
	}
};
