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
    Regression(Matrix<float> matrice) : _matrice(matrice) {};
    void compute(Dataset *data);
    void estimate_coeff(Dataset *data);
    Matrix<float> setMatrice(Dataset *data);
    float evaluate(Dataset& dataset, LossFunction lossFunction);

private:
    Matrix<float> _matrice;
    std::vector<float> _valuesVector;
    std::vector<float> coeff;
};