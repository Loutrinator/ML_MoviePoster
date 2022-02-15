//
// Created by hung9 on 13/02/2022.
//

#include "Regression.h"

#include <random>
#include <iostream>
#include "JsonHelper.h"

void Regression::compute(Dataset *data){
    //on applique le coeff à la ligne d'input associée et on le redonne dans les output
}

void Regression::estimate_coeff(Dataset *data){
    //Matrice de largeur 1 mais de longueur du nb d'output, rempli d'output ?
    Matrix<float> output(1, data->data().size());
    for(int i = 0; i < output.height(); i++){
        output(0,i) = data->data().at(i).expectedOutput.at(0);
    }

    Matrix<float> inputX(data->data().size()+1, data->data().data()->input.size());
    for(int i = 0; i < data->data().size(); i++){
        inputX(i,0) = 1.0f;
        for(int j = 0; j < data->data().data()->input.size(); j++){
            inputX(i,j) = data->data().at(i).input.at(j);
        }
    }

    Matrix<float> transposed( inputX.height(), inputX.width());
    for(int i = 0; i < inputX.height(); i++){
        for(int j = 0; j < inputX.width(); j++){
            transposed(i,j) = inputX(j,i);
        }
    }

    //multiplier transposed et inputX
    Matrix<float> xT_x();

    //prendre l'inverse
    Matrix<float> inverse();

    //(transposed * inputX)^-1 * transposed * output -> stocker dans coeff (coeff = {a,b})

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

float Regression::evaluate(Dataset& dataset, LossFunction lossFunction){
    compute(&dataset);
    _valuesVector.clear();

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
