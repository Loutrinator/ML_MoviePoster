#include "NeuralNetwork.h"
#include <random>
#include <iostream>
#include "JsonHelper.h"

static void printDeltas(const std::vector<std::vector<float>>& deltasVector)
{
	for(int i = 0; i < deltasVector.size(); i++)
	{
		std::vector<float> deltas = deltasVector[i];
		std::cout << "Deltas["<< i << "] : (" << deltas[0];
		for(int j= 1; j < deltas.size();j++){
			std::cout << " , " << deltas[j];
		}
		std::cout << ")" << std::endl;
	}
}

static void printValues(const std::vector<float>& values)
{
	std::cout << "(" << values[0];
	for(int j= 1; j < values.size()-1;j++)
	{
		std::cout << " , " << values[j];
	}
	std::cout << ")" << std::endl;
}

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

//Loss functions
/*
 * MEAN_SQUARE_ERROR,
    MEAN_ABSOLUTE_ERROR,
    MEAN_BIAS_ERROR,
    CROSS_ENTROPY
 * */
static float meanSquareError(Dataset& dataset)
{
    float sum = 0;
    for (Data& data : dataset.data())
    {
        float dataSum = 0;
        for (int i = 0; i < data.output.size() ; i++){
            float outputVal = data.output[i];
            float expectedVal = data.expectedOutput[i];
            dataSum += std::pow(outputVal-expectedVal,2);
        }
        sum += dataSum / (float)data.output.size();
    }
    float res = sum/(float)dataset.data().size();
    std::cout<< "sum " << sum << " | dataset.data().size() " << dataset.data().size() << " = " << res << std::endl;
    return res;
}

static float meanAbsoluteError(Dataset& dataset)
{
    float sum = 0;
    for (Data& data : dataset.data())
    {
        float dataSum = 0;
        for (int i = 0; i < data.output.size() ; i++){
            float outputVal = data.output[i];
            float expectedVal = data.expectedOutput[i];
            dataSum += std::abs(outputVal-expectedVal);
        }
        sum += dataSum / (float)data.output.size();
    }
    return sum/(float)dataset.data().size();
}
static float meanBiasError(Dataset& dataset)
{
    float sum = 0;
    for (Data& data : dataset.data())
    {
        float dataSum = 0;
        for (int i = 0; i < data.output.size() ; i++){
            float outputVal = data.output[i];
            float expectedVal = data.expectedOutput[i];
            dataSum += outputVal-expectedVal;
        }
        sum += dataSum / (float)data.output.size();
    }
    return sum/(float)dataset.data().size();
}

static float crossEntropyLoss(Dataset& dataset)
{
    float sum = 0;
    for (Data& data : dataset.data())
    {
        float dataSum = 0;
        for (int i = 0; i < data.output.size() ; i++){
            float outputVal = (data.output[i] + 1) / 2.0;           // obligé de le passer de -1 1 à 0 1 car une valeur négative sur le log retourne NaN
            float expectedVal = (data.expectedOutput[i] + 1) / 2.0; // obligé de le passer de -1 1 à 0 1 car une valeur négative sur le log retourne NaN
            std::cout << "outputVal " << outputVal << "expectedVal " << expectedVal << std::endl;
            dataSum += -(outputVal * std::log(expectedVal) + (1-outputVal)*std::log(1-expectedVal));
        }
        sum += dataSum / (float)data.output.size();
    }
    return sum/(float)dataset.data().size();
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
	
	NeuralLayer& neuralLayer = _layers.emplace_back();
	neuralLayer.neuronCount = neuronCount;
	neuralLayer.outputFunction = NeuronOutputFunction{outputFunction, function};
	neuralLayer.values.resize(neuronCount + 1);
	
	if (_layers.size() > 1)//pas les inputs
	{
		int matrixWidth = _layers[_layers.size()-2].neuronCount + 1;
		int matrixHeight = _layers[_layers.size()-1].neuronCount;

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
	std::copy(input.begin(), input.end(), _layers.front().values.begin());
	
	for (int i = 0; i < _layers.size(); i++)
	{
		std::vector<float>& currentLayerValues = _layers[i].values;
		
		if (i > 0)
		{
			std::vector<float>& previousLayerValues = _layers[i-1].values;
			std::span<float> currentLayerValuesSpan = std::span<float>(currentLayerValues.begin(), currentLayerValues.begin() + _layers[i].neuronCount); // all elements except last one (the bias value)
			
			std::fill(currentLayerValuesSpan.begin(), currentLayerValuesSpan.end(), 0.0f);
			_matrices[i-1].multiply(previousLayerValues, currentLayerValuesSpan); //calcul
		}
		
		if(_debugMode)
		{
			std::cout << "Values before function : ";
			printValues(currentLayerValues);
		}
		
        _layers[i].outputFunction.function(currentLayerValues);//application de la fonction sigmo/lineaire
		
        if(_debugMode)
		{
            std::cout << "Values after function : ";
            printValues(currentLayerValues);
        }
		
		currentLayerValues.back() = 1.0f;//on set la value du neuron de biais
	}
	
	std::copy(_layers.back().values.begin(), _layers.back().values.end()-1, output.begin());
}

int NeuralNetwork::getOutputSize() const
{
	return _layers.back().neuronCount;
}

void NeuralNetwork::debug_setValue(int matrixIndex, int x, int y, float value)
{
	_matrices[matrixIndex](x, y) = value;
}

void NeuralNetwork::save(const std::filesystem::path& path, bool beautify) const
{
	nlohmann::json jsonRoot;
	
	nlohmann::json jsonLayers;
	for (const NeuralLayer& layer : _layers)
	{
		nlohmann::json jsonLayer;
		jsonLayer["neuron_count"] = layer.neuronCount;
		
		std::string outputFunctionName;
		switch (layer.outputFunction.functionType)
		{
			case OutputFunction::LINEAR:
				outputFunctionName = "linear";
				break;
			case OutputFunction::SIGMOID:
				outputFunctionName = "sigmoid";
				break;
		}
		jsonLayer["output_function"] = outputFunctionName;
		
		jsonLayers.push_back(jsonLayer);
	}
	jsonRoot["layers"] = jsonLayers;
	
	nlohmann::json jsonMatrices;
	for (const Matrix<float>& matrix : _matrices)
	{
		jsonMatrices.push_back(matrix.getData());
	}
	jsonRoot["matrices"] = jsonMatrices;
	
	JsonHelper::saveJsonToFile(jsonRoot, path, beautify);
}

void NeuralNetwork::load(const std::filesystem::path& path)
{
	const nlohmann::json jsonRoot = JsonHelper::loadJsonFromFile(path);
	
	const nlohmann::json& jsonLayers = jsonRoot["layers"];
	for (const nlohmann::json& jsonLayer : jsonLayers)
	{
		OutputFunction outputFunction;
		std::string outputFunctionName = jsonLayer["output_function"].get<std::string>();
		if (outputFunctionName == "linear")
		{
			outputFunction = OutputFunction::LINEAR;
		}
		else if (outputFunctionName == "sigmoid")
		{
			outputFunction = OutputFunction::SIGMOID;
		}
		else
		{
			throw std::runtime_error("Unknown output function name.");
		}
		
		addLayer(jsonLayer["neuron_count"].get<int>(), outputFunction);
	}
	
	const nlohmann::json& jsonMatrices = jsonRoot["matrices"];
	for (int i = 0; i < jsonMatrices.size(); i++)
	{
		const nlohmann::json& jsonMatrix = jsonMatrices[i];
		Matrix<float>& matrix = _matrices[i];
		
		assert(jsonMatrix.size() == matrix.getData().size());
		
		std::copy(jsonMatrix.begin(), jsonMatrix.end(), matrix.getData().begin());
	}
}

void NeuralNetwork::train(Dataset& dataset, int iterations, float alpha, bool isClassification)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, dataset.data().size()- 1);

    std::vector<std::vector<float>> deltasVector;
    deltasVector.reserve(nbLayers()+1);//on alloue la place requise
    for (const auto& layer : _layers)
    {
        deltasVector.emplace_back(layer.neuronCount);
    }

    int itBetweenLogs = iterations/10;
	for (int it = 0; it < iterations; ++it)
	{
        Data& data = dataset.data()[distr(gen)];
		compute(data.input, data.output);
        backpropagate(data.expectedOutput,data.output, deltasVector, alpha, isClassification);
        if(_debugMode)printDeltas(deltasVector);
        if(_debugMode)printNetwork();
        if(it % itBetweenLogs == 0)
        {
            std::cout << "["<< (static_cast<float>(it) / static_cast<float>(iterations))*100 << "% done]" << std::endl;
        }
	}
    std::cout << "[100% done]" << std::endl;
}


float NeuralNetwork::evaluate(Dataset& dataset, float diffThreshold, LossFunction lossFunction)
{
    std::vector<std::vector<float>> deltasVector;
    deltasVector.reserve(nbLayers()+1);//on alloue la place requise
    for (const auto& layer : _layers)
    {
        deltasVector.emplace_back(layer.neuronCount);
    }

    for (int it = 0; it < dataset.data().size(); ++it)
	{
        if (_debugMode) std::cout << std::endl << "Itteration : " << it << std::endl;
        Data& data = dataset.data()[it];
        compute(data.input, data.output);

    }
    float loss = 0;
    switch (lossFunction) {
        case LossFunction::MEAN_ABSOLUTE_ERROR:
            std::cout << "MEAN_ABSOLUTE_ERROR ";
            loss = meanAbsoluteError(dataset);
            break;
        case LossFunction::MEAN_BIAS_ERROR:
            std::cout << "MEAN_BIAS_ERROR ";
            loss = meanBiasError(dataset);
            break;
        case LossFunction::MEAN_SQUARE_ERROR:
            std::cout << "MEAN_SQUARE_ERROR ";
            loss = meanSquareError(dataset);
            break;
        case LossFunction::CROSS_ENTROPY:
            std::cout << "CROSS_ENTROPY ";
            loss = crossEntropyLoss(dataset);
            break;
    }
    return loss;
}

void NeuralNetwork::backpropagate(std::vector<float>& expectedOutput, std::vector<float>& output, std::vector<std::vector<float>>& deltasVector, float alpha, bool isClassification)
{
    //ON COMMENCE PAR CONSTRUIRE LES DELTAS DE LA DERNIERE COUCHE
    //On parcours toutes les sorties du layer d'output
    //deltasVector contient tous les vectors d'erreurs pour chaque couche
    std::vector<float>& outputLayerDeltas = deltasVector.back();
    std::vector<float>& outputLayerValues = _layers.back().values;//les valeurs de la couche de sortie
    for(int j = 0; j < _layers.back().neuronCount; ++j)
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
        std::vector<float>& previousValues = _layers[l-1].values;
        Matrix<float>& currentMatrix = _matrices[l-1];

        //on parcours autant de fois que le layer précédent est grand
        for(int i = 0; i < _layers[l-1].neuronCount; i++)//i = l'input TODO: peut etre retirer - 1
        {
            float total = 0;

            //on parcours chaque neurone
            for(int j = 0; j < _layers[l].neuronCount; j++)// j = au neurone
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
        std::vector<float>& previousValues = _layers[l-1].values;

        for(int i = 0; i < _layers[l-1].neuronCount+1; i++)//nb de neurones des inputs de ma couche + biais
        {
            float val = i < _layers[l-1].neuronCount ? previousValues[i] : 1;//permet de gérer le cas du biais
            for(int j = 0; j < _layers[l].neuronCount; j++)
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

void NeuralNetwork::printNetwork() const
{
    for(int i = 0 ; i < _matrices.size(); i++)
	{
        std::cout << "Layer " << i << std::endl;
        Matrix<float> mat = _matrices[i];
        for(int y = 0 ; y < mat.height(); y++)
		{
            std::cout << "(" << mat(0,y);
            for(int x = 1 ; x < mat.width(); x++)
			{
                std::cout << " , " << mat(x,y);
            }
            std::cout << ")" << std::endl;
        }
        std::cout << std::endl;
    }
}

NeuralNetwork::NeuralNetwork(bool debug):
	_debugMode(debug)
{

}

NeuralNetwork* NeuralNetwork_Create(bool debug)
{
	return new NeuralNetwork(debug);
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

void NeuralNetwork_Train(NeuralNetwork* ptr, Dataset* dataset, int iterations, float alpha, bool isClassification)
{
	ptr->train(*dataset, iterations, alpha, isClassification);
}

float NeuralNetwork_Evaluate(NeuralNetwork* ptr, Dataset* dataset, float diffThreshold, LossFunction lossFunction)
{
	return ptr->evaluate(*dataset, diffThreshold, lossFunction);
}

void NeuralNetwork_Save(NeuralNetwork* ptr, const char* path, bool beautify)
{
	ptr->save(std::filesystem::path(path), beautify);
}

void NeuralNetwork_Load(NeuralNetwork* ptr, const char* path)
{
	ptr->load(std::filesystem::path(path));
}
