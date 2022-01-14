cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "vcpkg/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET "x64-windows-static")
set(CMAKE_DEBUG_POSTFIX d)

cmake_policy(SET CMP0091 NEW)

project(MachineLearning)

#add_executable(MachineLearning src/Main.cpp src/NeuralNetwork.cpp src/NeuralNetwork.h src/Matrix.h src/OutputFunction.h src/Data.h src/API.h src/Dataset.cpp src/Dataset.h)
add_library(MachineLearning SHARED src/NeuralNetwork.cpp src/NeuralNetwork.h src/Matrix.h src/OutputFunction.h src/Data.h src/API.h src/Dataset.cpp src/Dataset.h)

set_property(TARGET MachineLearning PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_compile_options(MachineLearning PRIVATE /Zc:__cplusplus)
target_compile_features(MachineLearning PRIVATE cxx_std_20)
