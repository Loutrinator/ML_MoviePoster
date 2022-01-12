#include "NeuralNetwork.h"
#include <random>
#include <iostream>

static void linear(std::vector<float>& values)
{
	for (float& value : values)
	{
		value = value;
	}
}

static void sigmoid(std::vector<float>& values)
{
    for (float& value : values)
    {
        value = tanh(value);
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
        case OutputFunction::SIGMOID:
            function = sigmoid;
            break;
	}
	
	_layers.emplace_back(std::make_pair(neuronCount, function));
	
	if (_layers.size() > 1)
	{
		int matrixWidth = _layers[_layers.size()-2].first + 1;
		int matrixHeight = _layers[_layers.size()-1].first;

        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        std::uniform_real_distribution<float> distr(-1, 1);
		Matrix<float>& mat = _matrices.emplace_back(matrixWidth, matrixHeight);
        for(int x = 0; x < mat.width(); x++){
            for(int y = 0; y < mat.height(); y++){
                mat(x,y) = distr(gen);
            }
        }
	}
}

void NeuralNetwork::compute(std::span<float> input, std::span<float> output)
{
	std::vector<float> values(input.begin(), input.end());

    _valuesVector.push_back(values);//on stoque les inputs dans values
    values.emplace_back(1.0f);//inputs + biais
	
	for (int i = 0; i < _matrices.size(); i++)
	{
		values = _matrices[i] * values; //calcul
        if(debugMode){
            std::cout << "Values before function : ";
            printValues(values);
        }
        _layers[i].second(values);//application de la fonction sigmo/lineaire
        if(debugMode){
            std::cout << "Values after function : ";
            printValues(values);
        }

        _valuesVector.push_back(values);//on stoque le resultat

        values.emplace_back(1.0f);//on rajoute 1 pour le prochain
	}
	
	std::copy(values.begin(), values.end()-1, output.begin());
}

int NeuralNetwork::getOutputSize() const
{
	return _layers.back().first;
}

void NeuralNetwork::debug_setValue(int matrixIndex, int x, int y, float value)
{
	_matrices[matrixIndex](x, y) = value;
}

void NeuralNetwork::train(std::vector<Data>& dataset, int iterations, float alpha, bool isClassification)
{
    for (Data& data : dataset)
    {
        data.output.resize(getOutputSize());
    }

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, dataset.size()- 1);

    std::vector<std::vector<float>> deltasVector;
    deltasVector.reserve(nbLayers()+1);//on alloue la place requise
    for (const auto& layer : _layers)
    {
        deltasVector.emplace_back(layer.first);
    }

    int itBetweenLogs = iterations/10;
	for (int it = 0; it < iterations; ++it)
	{
        if(debugMode)std::cout << std::endl << "Itteration : " << it << std::endl;
        Data& data = dataset[distr(gen)];
		compute(data.input, data.output);
        backpropagate(data.expectedOutput,data.output, deltasVector, alpha, isClassification);
        //if(debugMode)printDeltas(deltasVector);
        //if(debugMode)printNetwork();
        _valuesVector.clear();
        if(it % itBetweenLogs == 0)
        {
            std::wcout << "["<< (static_cast<float>(it) / static_cast<float>(iterations))*100 << "% done]" << std::endl;
        }
	}
    std::wcout << "[100% done]" << std::endl;
}


float NeuralNetwork::evaluate(std::vector<Data>& dataset, int iterations, float diffThreshold)
{
    for (Data& data : dataset)
    {
        data.output.resize(getOutputSize());
    }

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, dataset.size()- 1);

    std::vector<std::vector<float>> deltasVector;
    deltasVector.reserve(nbLayers()+1);//on alloue la place requise
    for (const auto& layer : _layers)
    {
        deltasVector.emplace_back(layer.first);
    }

    float totalGoodPrediction = 0;
    int itBetweenLogs = iterations/10;
    for (int it = 0; it < iterations; ++it) {
        if (debugMode)std::cout << std::endl << "Itteration : " << it << std::endl;
        Data& data = dataset[distr(gen)];
        compute(data.input, data.output);
        _valuesVector.clear();

        bool error = false;
        for (int i = 0; i < data.output.size(); i++) {
            float diff = abs(data.output[i] - data.expectedOutput[i]);
            if (diff > diffThreshold) {
                error = true;
                break;
            }
        }
        if (!error) {
            totalGoodPrediction++;
        }

        if (it % itBetweenLogs == 0) {
            std::wcout << "[" << (static_cast<float>(it) / static_cast<float>(iterations)) * 100 << "% done]"
                       << std::endl;
        }
    }
    std::wcout << "[100% done]" << std::endl;
    return totalGoodPrediction/static_cast<float>(dataset.size());
}

void NeuralNetwork::backpropagate(std::vector<float>& expectedOutput, std::vector<float>& output, std::vector<std::vector<float>>& deltasVector, float alpha, bool isClassification)
{
    //ON COMMENCE PAR CONSTRUIRE LES DELTAS DE LA DERNIERE COUCHE
    //On parcours toutes les sorties du layer d'output
    std::vector<float>& outputLayerDeltas = deltasVector.back();
    std::vector<float>& outputLayerValues = _valuesVector.back();//les valeurs de la couche de sortie
    for(int j = 0; j < outputLayerValues.size(); ++j)
    {
        //on calcule la diff entre la valeur et celle attendue pour le Jeme neurone
        outputLayerDeltas[j] = outputLayerValues[j] - expectedOutput[j];

        //si on est en retropropagation : on s'arrete là
        //sinon on rajoute ca :
        if(isClassification)
            outputLayerDeltas[j] *= 1 - std::powf(outputLayerValues[j], 2);
    }

    //ON CALCULE LES DELTAS SUIVANTS A PARTIR DU PREMIER
    for(int l = nbLayers(); l >= 2; --l)//l = la couche
    {
        std::vector<float>& currentDeltas = deltasVector[l];
        std::vector<float>& previousDeltas = deltasVector[l-1];
        std::vector<float>& previousValues = _valuesVector[l-1];
        Matrix<float>& currentMatrix = _matrices[l-1];

        //on parcours autant de fois que le layer précédent est grand
        for(int i = 0; i < _layers[l-1].first; i++)//i = l'input TODO: peut etre retirer - 1
        {
            float total = 0;

            //on parcours chaque neurone
            for(int j = 0; j < _layers[l].first; j++)// j = au neurone
            {
                total += currentMatrix(i,j) * currentDeltas[j];//TODO: attention peut etre inverser j et i
            }

            total *= (1 - powf(previousValues[i],2));
            previousDeltas[i] = total;
        }
    }
    /* input est de taille 3
     * le layer a 2 neurones
     * in   0.1 0.2 0.3
     *      _______________
     * mat  0.4 0.5 0.4 0.8
     *      0.1 0.8 0.7 0.2
     *      _______________
     * */

    /* input est de taille 3
     * le layer a 2 neurones
     * in   1   0.1 0.2 0.3
     *      _______________
     *      1   1    1   1
     * mat  0.4 0.5 0.4 0.8
     *      0.1 0.8 0.7 0.2
     *      _______________
     * */

    //ON APPLIQUE TOUS LES DELTAS A CHAQUE POIDS DE CHAQUE NEURONE DE CHAQUE COUCHE
    for(int l = 1; l <= nbLayers(); ++l)//pour toutes les couches sauf input
    {
        Matrix<float>& matrix = _matrices[l-1];
        std::vector<float>& currentDeltas = deltasVector[l];
        std::vector<float>& previousValues = _valuesVector[l-1];

        for(int i = 0; i < _layers[l-1].first+1; i++)
        {
            float val = i < _layers[l-1].first ? previousValues[i] : 1;//permet de gérer le cas du biais
            for(int j = 0; j < _layers[l].first; j++)
            {

                matrix(i,j) -= alpha * val * currentDeltas[j];
            }
        }
    }
}
//Retourne le nombre de couches sans compter l'input
int NeuralNetwork::nbLayers()
{
    return _layers.size() -1;
}

void NeuralNetwork::printNetwork() {
    for(int i = 0 ; i < _matrices.size(); i++){
        std::cout << "Layer " << i << std::endl;
        Matrix<float> mat = _matrices[i];
        for(int y = 0 ; y < mat.height(); y++){
            std::cout << "(" << mat(0,y);
            for(int x = 1 ; x < mat.width(); x++){
                std::cout << " , " << mat(x,y);
            }
            std::cout << ")" << std::endl;
        }
        std::cout << std::endl;
    }
}

void NeuralNetwork::printDeltas(const std::vector<std::vector<float>>& deltasVector) {
    for(int i = 0; i < deltasVector.size();i++){
        std::vector<float> deltas = deltasVector[i];
        std::cout << "Deltas["<< i << "] : (" << deltas[0];
        for(int j= 1; j < deltas.size();j++){
            std::cout << " , " << deltas[j];
        }
        std::cout << ")" << std::endl;
    }
}

void NeuralNetwork::printValues(const std::vector<float>& values) {
    std::cout << "(" << values[0];
    for(int j= 1; j < values.size();j++){
        std::cout << " , " << values[j];
    }
    std::cout << ")" << std::endl;
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

void NeuralNetwork_Compute(NeuralNetwork* ptr, float* input, int inputSize, float* output, int outputSize)
{
	ptr->compute(std::span<float>(input, inputSize), std::span<float>(output, outputSize));
}

void NeuralNetwork_Debug_SetValue(NeuralNetwork* ptr, int matrixIndex, int x, int y, float value)
{
	ptr->debug_setValue(matrixIndex, x, y, value);
}
