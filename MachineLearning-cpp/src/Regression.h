//
// Created by hung9 on 13/02/2022.
//

#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <vector>
#include <span>
#include <filesystem>

#include "Matrix.h"
#include "Data.h"
#include "OutputFunction.h"
#include "API.h"
#include "Dataset.h"
#include "LossFunction.h"


class Regression {
public:
    Regression(){};
    void compute(std::span<float> input, std::span<float> output);
    void estimate_coeff(std::span<float> input);
    float evaluate(Dataset& dataset, float diffThreshold, LossFunction lossFunction);

    std::vector<float> _valuesVector;
private:
    std::vector<Matrix<float>> _matrices;
    float b1;
    float b0;
};