//
// Created by hung9 on 13/02/2022.
//

#include "Regression.h"

#include <random>
#include <iostream>
#include "JsonHelper.h"

void Regression::compute(std::span<float> input, std::span<float> output){
    std::vector<float> values(input.begin(), input.end());

    for(int i = 0; i < values.size(); i++){
        _valuesVector.push_back(this->b0 + this->b1 * values.at(i));
    }
}

void Regression::estimate_coeff(std::span<float> input){
    float meanX = 0;
    float meanY = 0;
    float meanXY = 0;
    float meanXX = 0;

    for (int i = 0; i < input.size(); i++)
    {
        meanX += input[i];
        for(int j = 0; j < output.size(); j++){
            meanY += output[j];

            meanXY += input[i] * output[j];
            meanXX += input[i] * input[i];
        }
    }

    meanX /= input.size();
    meanY /= output.size();

    float ss_xy = meanXY - input.size() * meanY * meanX;
    float ss_xx = meanXX - input.size() * meanX * meanX;

    float b0 = 0;
    if(ss_xx != 0) b0 = ss_xy / ss_xx;
    float b1 = meanY - b0 * meanX;

    this->b1 = b1;
    this->b0 = b0;
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

float Regression::evaluate(Dataset& dataset, float diffThreshold, LossFunction lossFunction){

    for(int it = 0; it < dataset.data().size(); ++it){
        Data& data = dataset.data()[it];
        compute(data.input, data.output);
        _valuesVector.clear();
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
