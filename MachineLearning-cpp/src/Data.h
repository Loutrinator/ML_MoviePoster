#pragma once

#include <vector>

struct Data
{
	std::vector<float> input; //de la meme taille que les inputs de notre network
	std::vector<float> expectedOutput; //de la meme taille que les outputs de notre network
	std::vector<float> output; //pareil
	float error = -1; //egal au taux d'erreur
};