#pragma once
#include <cmath>
#include <fstream>
#include <vector>
#include "Matrix.h"

class NeuralNet
{
public:
	NeuralNet();
	NeuralNet(std::vector<size_t>&& neuronCountInEachLayer);
	NeuralNet(std::vector<size_t>&& neuronCountInEachLayer, std::vector<Matrix>&& weights, std::vector<Matrix>&& biases);
	NeuralNet(std::ifstream&);

public:
	size_t getOutputAsIndex(const Matrix& input) const;
	const Matrix& getOutputAsVector(const Matrix& input) const;

	const std::vector<size_t>& cgetNeuronCounts() const;
	const std::vector<Matrix>& cgetWeights() const;
	const std::vector<Matrix>& cgetBiases() const;
	Matrix& getWeightMatrix(size_t index);       // Does not check if index is valid.
	Matrix& getBiasVector(size_t index);         // Does not check if index is valid.

public:
	// Activation function. 
	// sigma(x) = 1 / (1 + e^(-x)), where 'e' is Euler's constant.
	static double sigmoidFunction(double x) { return 1.0 / (1.0 + exp(-x)); }
	// The derivative of the activation function.
	// sigma'(x) = e^(-x) / (1 + e^(-x))^2
	// The above can also be written as: sigma'(x) = sigma(x) * (1 - sigma(x))
	static double sigmoidDerivative(double x)
	{
		double sigmaOfX = sigmoidFunction(x);
		return sigmaOfX * (1 - sigmaOfX);
	}
	static void applySigmoidToMatrix(Matrix& m)
	{
		double* p = m.getRow(0);
		while (p != m.getRow(m.getRows() - 1) + m.getCols())
		{
			*p = sigmoidFunction(*p);
			++p;
		}
	}

private:
	void initWeightsAndBiases();
	void initNeuronActivations();

	void isValidNeuralNet(const std::vector<size_t>& neuronCountInEachLayer) const;
	void isValidNeuralNet(const std::vector<size_t>& neuronCountInEachLayer,
		                 const std::vector<Matrix>& weights,
		                 const std::vector<Matrix>& biases) const;

private:
	std::vector<size_t> neuronCountInEachLayer;    // If the size of this vector is 0 the object is considered uninitialized.
	std::vector<Matrix> weights;
	std::vector<Matrix> biases;

	// Used for calculating the output of the neural network. Not part of the
	// data of the neural net.
	// It is a member of the class (and not in some calculateOutput() function)
	// because allocating memory every time we calculate the output is slow.
	std::vector<Matrix> neuronActivations;    
};

std::ifstream& operator>>(std::ifstream&, NeuralNet&);        // 'file' should be opened in binary.
std::ofstream& operator<<(std::ofstream&, const NeuralNet&);  // 'file' should be opened in binary.
