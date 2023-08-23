
#include <algorithm>    // std::find
#include <utility>
#include "NeuralNet.h"

NeuralNet::NeuralNet() {}
NeuralNet::NeuralNet(std::vector<size_t>&& neuronCountInEachLayer)
{
	isValidNeuralNet(neuronCountInEachLayer);
	this->neuronCountInEachLayer = neuronCountInEachLayer;
	initWeightsAndBiases();
	initNeuronActivations();
}
NeuralNet::NeuralNet
(std::vector<size_t>&& neuronCountInEachLayer, std::vector<Matrix>&& weights, std::vector<Matrix>&& biases)
{
	isValidNeuralNet(neuronCountInEachLayer, weights, biases);
	this->neuronCountInEachLayer = neuronCountInEachLayer;
	this->weights = weights;
	this->biases = biases;
	initNeuronActivations();
}
NeuralNet::NeuralNet(std::ifstream& ifile)
{
	ifile >> *this;
}

// 'input' is a vector column of the activations of the neurons in the input layer.
// Returns the index of the neuron with the highest activation.
size_t NeuralNet::getOutputAsIndex(const Matrix& input) const
{
	if (input.getRows() != neuronCountInEachLayer[0] || input.getCols() != 1)
		throw "Invalid matrix input layer!";

	const Matrix& outLayer = getOutputAsVector(input);

	// Find which neuron has the highest activation.
	double highestActivationValue = *outLayer.cgetRow(0);
	size_t neuronIndex = 0;
	for (size_t i = 1; i < neuronActivations.back().getRows(); ++i)
	{
		if (highestActivationValue < *outLayer.cgetRow(i))
		{
			highestActivationValue = *outLayer.cgetRow(i);
			neuronIndex = i;
		}
	}
	return neuronIndex;
}
// 'input' is a vector column of the activations of the neurons in the input layer.
// Returns the vector of the output neuron activations.
const Matrix& NeuralNet::getOutputAsVector(const Matrix& input) const
{
	if (input.getRows() != neuronCountInEachLayer[0] || input.getCols() != 1)
		throw "Invalid matrix input layer!";

	/*
		Casting to (std::vector<Matrix>&) because 'getOutputAsVector()' must be a const function.
		It is not changing anything in the object, it just uses 'neuronActivations' for calculating the output.
	*/
	std::vector<Matrix>& na = (std::vector<Matrix>&)neuronActivations;
	// Run the input through the neural net.
	// 'input' is separate so this is not in the below for loop.
	Matrix::matrixMultiplication(weights[0], input, (Matrix&)na[0]);
	(Matrix&)neuronActivations[0] += biases[0];
	applySigmoidToMatrix((Matrix&)neuronActivations[0]);
	// After using 'input' use only 'neuronActivations' array.
	for (size_t i = 1; i < weights.size(); ++i)
	{
		Matrix::matrixMultiplication(weights[i], neuronActivations[i - 1], (Matrix&)neuronActivations[i]);
		(Matrix&)neuronActivations[i] += biases[i];

		applySigmoidToMatrix((Matrix&)neuronActivations[i]);
	}

	return neuronActivations.back();
}

const std::vector<size_t>& NeuralNet::cgetNeuronCounts() const { return neuronCountInEachLayer; }
const std::vector<Matrix>& NeuralNet::cgetWeights() const { return weights; }
const std::vector<Matrix>& NeuralNet::cgetBiases() const { return biases; }
Matrix& NeuralNet::getWeightMatrix(size_t index) { return weights[index]; }
Matrix& NeuralNet::getBiasVector(size_t index) { return biases[index]; }

void NeuralNet::initWeightsAndBiases()
{
	weights.resize(neuronCountInEachLayer.size() - 1);
	biases.resize(neuronCountInEachLayer.size() - 1);

	for (size_t i = 0; i < weights.size(); ++i)
	{
		// Set matrices to the right dimensions.
		weights[i] = { neuronCountInEachLayer[i + 1], neuronCountInEachLayer[i] };
		biases[i] = { neuronCountInEachLayer[i + 1], 1 };
	}
}
void NeuralNet::initNeuronActivations()
{
	neuronActivations.resize(neuronCountInEachLayer.size() - 1);    // -1 because the neuron activations in
	                                                                // the input layer are given by the user.

	// Set appropriate dimensions.
	for (size_t i = 0; i < neuronActivations.size(); ++i)
		neuronActivations[i] = { neuronCountInEachLayer[i + 1], 1 };
}

// Throws exception if parameter doesn't represent a valid neural net.
void NeuralNet::isValidNeuralNet(const std::vector<size_t>& neuronCountInEachLayer) const
{
	// Must have at least input and output layer and neuron count in each layer must be > 0.
	if (neuronCountInEachLayer.size() < 2 ||
		std::find(neuronCountInEachLayer.begin(), neuronCountInEachLayer.end(), 0) != neuronCountInEachLayer.end())
		throw "Invalid neural net parameters!";
}
// Throws exception if parameters don't represent a valid neural net.
void NeuralNet::isValidNeuralNet
(const std::vector<size_t>& neuronCountInEachLayer, const std::vector<Matrix>& weights, const std::vector<Matrix>& biases) const
{
	if (neuronCountInEachLayer.size() < 2 || weights.size() != neuronCountInEachLayer.size() - 1 ||
		biases.size() != neuronCountInEachLayer.size() - 1)
	{
		throw "Invalid neural net parameters!";
	}

	for (size_t i = 0; i < weights.size(); ++i)
	{
		if (weights[i].getCols() != neuronCountInEachLayer[i] || weights[i].getRows() != neuronCountInEachLayer[i + 1] ||
			biases[i].getCols() != 1 || biases[i].getRows() != neuronCountInEachLayer[i + 1])
		{
			throw "Invalid neural net parameters!";
		}
	}
}

// File Specification for NeuralNet:
//  - 8 bytes of unsigned int (if size_t is 8 bytes) for the number_of_layers.
//  - array of size_t with length number_of_layers for the neuron counts in each layer.
//  - number_of_layers - 1 of alternating matrices of weights and biases with appropriate dimensions. 
std::ifstream& operator>>(std::ifstream& ifile, NeuralNet& net)
{
	if (!ifile.is_open())
		throw "Cannot read Neural network! No file is opened!";
	
	size_t layerCount = 0;
	ifile.read((char*)&layerCount, sizeof(size_t));
	std::vector<size_t> neuronCounts(layerCount);
	std::vector<Matrix> weights(layerCount - 1);
	std::vector<Matrix> biases(layerCount - 1);

	ifile.read((char*)&neuronCounts.front(), sizeof(size_t) * layerCount);
	for (size_t i = 0; i < weights.size(); ++i)
	{
		ifile >> weights[i] >> biases[i];
	}

	if (!ifile.good())
		throw "Corrupted neural net file!";

	net = { std::move(neuronCounts), std::move(weights), std::move(biases) };
}
std::ofstream& operator<<(std::ofstream& ofile, const NeuralNet& net)
{
	if (net.cgetNeuronCounts().size() == 0)
	{
		throw "Cannot save to file uninitialized object of type 'NeuralNet'! Make sure the object is initialized first.";
	}
	if (!ofile.is_open())
	{
		throw "Cannot write neural network to file! No file is opened!";
	}

	size_t layerCount = net.cgetNeuronCounts().size();
	ofile.write((const char*)&layerCount, sizeof(size_t));
	ofile.write((const char*)&net.cgetNeuronCounts().front(), sizeof(size_t) * layerCount);
	for (size_t i = 0; i < net.cgetWeights().size(); ++i)
	{
		ofile << net.cgetWeights()[i] << net.cgetBiases()[i];
	}
}