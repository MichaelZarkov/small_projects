
#include <cstring>
#include <iostream>
#include <utility>
#define NOMINMAX             // Disables 'min' and 'max' macros for 'windows.h'.
#include <windows.h>         // For console text formatting. 
#include "NeuralNetTrainer.h"

#pragma warning (disable : 4996)
#pragma warning (disable : 6319)   // Comma-operator in tested expression.

const size_t NeuralNetTrainer::BUFF_SIZE = 512;
const char NeuralNetTrainer::START_TITLE_SYMBOLS[][8] = { "///--- ", " ---///" };
const char NeuralNetTrainer::END_TITLE_SYMBOLS[][8] = { "\\\\\\--- ", " ---\\\\\\" };

NeuralNetTrainer::NeuralNetTrainer()
{
	initializeNeuralNetFromConsole();
	inputTrainingParameters();
	testNeuralNet();
	trainNeuralNet();
	testNeuralNet();
	saveToFile();
}


// Console input, output and formating.

void NeuralNetTrainer::initializeNeuralNetFromConsole()
{
	initializationMessage();

	// TO DO: add explanation.

	printStartTitle("Initialize the neural net");

	inputTrainingData();
	size_t hiddenLayerCount = inputHiddenLayerCount();                   // Input layer count.
	std::vector<size_t> neuronCountInEachLayer(hiddenLayerCount + 2);    // Set layer count (+2 because input and output layers).
	neuronCountInEachLayer.front() = trainingData.getCols();             // Set input layer.
	neuronCountInEachLayer.back() = trainingDataAnswers.getCols();       // Set output layer.
	inputNeuronCountInHiddenLayers(neuronCountInEachLayer);              // Input hidden layers.

	net = { std::move(neuronCountInEachLayer) };    // Initialize the dimensions of the neural network.

	printEndTitle("Initialization complete");
}
// Reads file name from console and saves the file in 'trainingData' and 'trainingDataAnswers'.
void NeuralNetTrainer::inputTrainingData()
{
	do
	{
		printText("Input file name with training data and answers: ");
		setConsoleTextUser();
		char buff[BUFF_SIZE];
		std::cin.getline(buff, BUFF_SIZE, '\n');
		if (std::cin.fail())
		{
			clearSTDcin();
			printError("Characters in file path is too long! Try different file path.");
			continue;
		}

		std::ifstream file(buff, std::ios::binary);
		if (!file.is_open())
		{
			printError("Cannot open file! File path may be wrong. Try again.");
			continue;
		}

		try
		{
			file >> trainingData >> trainingDataAnswers;
			if (trainingData.getRows() != trainingDataAnswers.getRows())
			{
				throw "This training file has a different number of examples and answers! Try again.";
			}
			file.close();
			printText("Training data loaded successfully.\n");
			return;
		}
		catch (const char* err)
		{
			printError(err);
			file.close();
			continue;
		}
		catch (...)
		{
			printError("Error while reading training data file!");
			file.close();
		}

	} while (true);
}
// Reads from console and returns it.
size_t NeuralNetTrainer::inputHiddenLayerCount() const
{
	size_t hiddenLayerCount = 0;
	while (printText("Input hidden layer count: "), setConsoleTextUser(), !(std::cin >> hiddenLayerCount))
	{
		clearSTDcin();
		printError("Invalid layer count! Try again.");
	}
	return hiddenLayerCount;
}
// Reads from console and saves in 'neuronCountInEachLayer'.
void NeuralNetTrainer::inputNeuronCountInHiddenLayers(std::vector<size_t>& neuronCountInEachLayer) const
{
	printText("Neurons in each hidden layer:\n");
	for (size_t i = 1; i < neuronCountInEachLayer.size() - 1; ++i)    // Go through only the hidden layers.
	{
		// Get valid neuron count for layer 'i'.
		while (setConsoleTextNeuralNet(), std::cout << "L" << i << ": ", setConsoleTextUser(),
			!(std::cin >> neuronCountInEachLayer[i]) || !neuronCountInEachLayer[i])
		{
			clearSTDcin();
			printError("Neuron count cannot be 0! Try again.");
		}
	}
}

void NeuralNetTrainer::initializationMessage() const
{
	setConsoleTextNeuralNet();
	std::cout << "Welcome to Neural Network trainer.\n"
		<< "The training will be in 6 steps:\n"
		<< "   1. Initialize the new neural network with a training data file.\n"
		<< "   2. Input the training parameters.\n"
		<< "   3. Test the neural network with testing data file (before the training).\n"
		<< "   4. Train the neural network.\n"
		<< "   5. Test the neural network with testing data file (after the training).\n"
		<< "   6. Optionally save to file the trained neural network.\n";
}

void NeuralNetTrainer::inputTrainingParameters()
{
	printStartTitle("Training parameters input");

	par.weightAndBiasValueRange = inputValueRange();
	fill(par.weightAndBiasValueRange);
	par.batchSize = inputBatchSize();
	par.generationCount = inputGenerationCount();
	par.learningRate = inputLearningRate();
	par.learningRateDecrease = inputLearningRateDecrease();

	printEndTitle("Training parameters input is done");
}
// Reads from console a vaild interval of real numbers and returns it.
NeuralNetTrainer::Interval NeuralNetTrainer::inputValueRange() const
{
	Interval I;
	while (printText("Value range: "), setConsoleTextUser(),
		!(std::cin >> I.from >> I.to) || I.from >= I.to)
	{
		clearSTDcin();
		printError("Invalid interval! Try again.");
	}
	return I;
}
// The number of examples on which the gradient is computed at once.
// Must be > 0.
size_t NeuralNetTrainer::inputBatchSize() const
{
	size_t batchSize = 0;
	while (printText("Batch size: "), setConsoleTextUser(),
		!(std::cin >> batchSize) || batchSize == 0)
	{
		clearSTDcin();
		printError("Batch size must be > 0! Try again.");
	}
	return batchSize;
}
// This is how many times to go through all training examples.
// Must be > 0.
size_t NeuralNetTrainer::inputGenerationCount() const
{
	size_t generationCount = 0;
	while (printText("Generation count: "), setConsoleTextUser(),
		!(std::cin >> generationCount) || generationCount == 0)
	{
		clearSTDcin();
		printError("Generation count must be > 0! Try again.");
	}
	return generationCount;
}
// The length of step of the descend.
// Must be a positive real number.
double NeuralNetTrainer::inputLearningRate() const
{
	double learningRate = 0.0;
	while (printText("Learning rate: "), setConsoleTextUser(),
		!(std::cin >> learningRate) || learningRate <= 0.0)
	{
		clearSTDcin();
		printError("Learning rate must be > 0.0! Try again.");
	}
	return learningRate;
}
// After every generation the learning rate will be devided by 'learningRateDecrease'.
// The idea is that when we get closer to the function minimum the steps we have to take are smaller.
// Must be >= 1.0.
double NeuralNetTrainer::inputLearningRateDecrease() const
{
	double learningRateDecrease = 0.0;
	while (printText("Learning rate decrease: "), setConsoleTextUser(),
		!(std::cin >> learningRateDecrease) || learningRateDecrease < 1.0)
	{
		clearSTDcin();
		printError("Learning rate decrease must be >= 1.0! Try again.");
	}
	std::cin.ignore();
	return learningRateDecrease;
}


void NeuralNetTrainer::saveToFile() const
{
	printStartTitle("Save to file");

	if (wantToSave())
	{
		char buff[BUFF_SIZE];
		inputFileName(buff);
		save(buff);

		printEndTitle("Saving done");
		printText("\nExiting the program . . .");
	}
	else
	{
		printText("\nExiting without saving . . .");
	}
}
// Returns true if the user wants to save to file.
bool NeuralNetTrainer::wantToSave() const
{
	printText("Type 1 if you want to save the neural net to file.");
	printText("\nType anything else to exit without saving: ");

	setConsoleTextUser();
	char buff[BUFF_SIZE] = "\0";
	if (!std::cin.getline(buff, BUFF_SIZE, '\n'))
		clearSTDcin();
	
	return *buff == '1';
}
void NeuralNetTrainer::inputFileName(char buff[]) const
{
	// TO DO: check if the file can be opened.
	while (printText("Enter file name: "), setConsoleTextUser(),
		   !std::cin.getline(buff, sizeof(buff), '\n'))
	{
		clearSTDcin();
		printError("File name too long! Try shorter one.");
	}
}
// Function expects the file to exist.
/*
	Saves two files:
	  - file with file extension .nnt is the neural network trainig file.
	    * first in the file are the parameters used in training.
		* next is the actual neural network.
	  - file with file extension .nn is only the neural network file.
*/
void NeuralNetTrainer::save(const char* fileName) const
{
	char withEx[BUFF_SIZE + 4];    // Buffer to add the file extension.
	strcpy(withEx, fileName);
	strcat(withEx, ".nn");

	// Write '.nn' file.
	std::ofstream ofile(withEx, std::ios::binary);
	ofile << net;
	ofile.close();

	// Write '.nnt' file.
	strcat(withEx, "t");
	ofile.open(withEx, std::ios::binary);
	ofile.write((char*)&par, sizeof(par));
	ofile << net;
	ofile.close();
}

// Returns true if a file with the given name exists.
bool NeuralNetTrainer::exist(const char* fileName) const
{
	// No need to call 'close()' explicitly because std::ifstream does it
	// when it goes out of scope.
	return std::ifstream(fileName).good();
}

void NeuralNetTrainer::printStartTitle(const char* str) const
{
	setConsoleTextNeuralNet();
	std::cout << '\n' << START_TITLE_SYMBOLS[0] << str << START_TITLE_SYMBOLS[1] << '\n';
}
void NeuralNetTrainer::printEndTitle(const char* str) const
{
	setConsoleTextNeuralNet();
	std::cout << END_TITLE_SYMBOLS[0] << str << END_TITLE_SYMBOLS[1] << '\n';
}
void NeuralNetTrainer::printError(const char* str) const
{
	setConsoleTextError();
	std::cout << str << std::endl;
}
void NeuralNetTrainer::printText(const char* str) const
{
	setConsoleTextNeuralNet();
	std::cout << str;
}

void NeuralNetTrainer::setConsoleTextUser() const
{
	// Colour is currently set to white.
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}
void NeuralNetTrainer::setConsoleTextNeuralNet() const
{
	// Colour is currently set to cyan.
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}
void NeuralNetTrainer::setConsoleTextError() const
{
	// Colour is currently set to red.
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
}

void NeuralNetTrainer::clearSTDcin() const
{
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


// Test how many examples the network gets right from given test dataset.
void NeuralNetTrainer::testNeuralNet() const
{
	printStartTitle("Test neural network");

	Matrix testDataInputs, testDataAnswers;
	inputTestingDataFromFile(testDataInputs, testDataAnswers);

	size_t correctAnswers = testNeuralNet(testDataInputs, testDataAnswers);

	setConsoleTextNeuralNet();
	std::cout << "\nThe neural net got " << correctAnswers << " of the " << testDataInputs.getRows() << " examples correctly.\n";
	std::cout << "That is " << (double)correctAnswers / (double)testDataInputs.getRows() * 100 << "%\n";

	printEndTitle("Test neural network is done");
}
void NeuralNetTrainer::inputTestingDataFromFile(Matrix& testDataInputs, Matrix& testDataAnswers) const
{
	do
	{
		printText("Input file name with testing data and answers: ");
		//std::cin.ignore();   // Is this necessary?
		setConsoleTextUser();
		char buff[BUFF_SIZE];
		std::cin.getline(buff, BUFF_SIZE, '\n');
		if (std::cin.fail())
		{
			clearSTDcin();
			printError("Characters in file path is too long! Try different file path.");
			continue;
		}

		std::ifstream file(buff, std::ios::binary);
		if (!file.is_open())
		{
			printError("Cannot open file! File path may be wrong. Try again.");
			continue;
		}

		try
		{
			file >> testDataInputs >> testDataAnswers;
			file.close();
			// Check if number of neurons is correct.
			if (testDataInputs.getRows() != testDataAnswers.getRows() ||
				testDataInputs.getCols() != net.cgetNeuronCounts().front() ||
				testDataAnswers.getCols() != net.cgetNeuronCounts().back())
			{
				throw "The testing file and the neural net are not with the same number of neurons! Try again.";
			}
			printText("Testing data loaded successfully.\n");
			return;
		}
		catch (const char* err)
		{
			printError(err);
			file.close();
			continue;
		}
		catch (...)
		{
			printError("Error while reading testing data file!");
			file.close();
		}

	} while (true);
}
// Returns how many correct answers.
size_t NeuralNetTrainer::testNeuralNet(const Matrix& testDataInputs, const Matrix& testDataAnswers) const
{
	Matrix input{ testDataInputs.getCols(), 1 };    // Container for a singe input.

	// Go through all testing examples.
	size_t howManyCorrect = 0;
	for (size_t i = 0; i < testDataInputs.getRows(); ++i)
	{
		// Set input layer.
		memcpy(input.getRow(0), testDataInputs.cgetRow(i), sizeof(double) * (input.getRows() + 1));

		// Find correct answer.
		size_t answerIndex = 0;
		while (testDataAnswers.cgetMatrix()[i][answerIndex] == 0 && answerIndex < testDataAnswers.getCols())
			++answerIndex;
		
		// Compare output on the neural net and the true answer.
		if (answerIndex == net.getOutputAsIndex(input))
			++howManyCorrect;
	}
	return howManyCorrect;
}


/*

 'Z' - array of vector columns(Nx1) corresponding to the neurons in the layers after the input layer.
	 - 'Z' contains 'layer count - 1' vector columns.
	 - stores the activations of the neurons BEFORE 'sigmoidFunction()' has been applied (these activations
	   are necessary to compute the "error" which is stored in 'D').
	 - Ex:
			   If we have the neural net:
						  6
				4        /
				 \	   O      2
				   O   O     /
				   O ~ O ~ O
				   O ~ O ~ O
				   O   O     \
				 /	   O     out.
			  inp.       \
						  hidden

				* with 3 layers (input, one hidden and output)
				* 4 neurons in input layer
				* 6 neurons in hidden layer
				* 2 neurons in output layer

			Then 'Z' will have 2 elements:
			  * 'Z[0]' is a vector column with dimensions 6x1
			  * 'Z[1]' is a vector column with dimensions 2X1
			In general 'Z[0]' is a vector column for the second layer and 'Z[last]'
			is a vector column for the output layer (with the appropriate dimensions of course).


 'A' - array of vector columns(Nx1) with the exact same dimensions as in 'Z' but this time
	   stores the activations of the neurons AFTER 'sigmoidFunction()' has been applied.


 'D' - array of vector columns(Nx1) with the exact same dimensions as in 'Z' and 'A'.
	 - stores the so called "error".


 'dCdw' - the gradient vector of the weights is stored here.
		- exact same dimensions as the weights matrices of the neural network.


 'dCdb' - the gredient vector of the biases is stored here.
	    - exact same dimensions as the biases vectors of the neural network.


 Requirements:
	* 1 < 'batchSize'
	* 'genCount' > 0
	* 'learningRate' > 0.0
	* 'learningRateDecrease' >= 1.0
*/
void NeuralNetTrainer::trainNeuralNet()
{
	printStartTitle("Training the neural network");

	std::vector<Matrix> Z, A, D;
	setDimensionsZAD(Z, A, D);

	std::vector<Matrix> dCdw{ net.cgetWeights() };
	std::vector<Matrix> dCdb{ net.cgetBiases() };

	// Store the current training example in these vector columns.
	// This makes it easier to operate with them.
	// Used in 'trainSingleBatch()' in order to not allocate memory every time for them.
	Matrix trExInp{ trainingData.getCols(), 1 };
	Matrix trExAnswer{ trainingDataAnswers.getCols(), 1 };

	double learnRate = par.learningRate;    // New variable because learning rate gets modified.

	// Generation loop.
	for (size_t generation = 0; generation < par.generationCount; ++generation)
	{
		// Batch loop.
		size_t from = 0, to = par.batchSize;
		while (from < trainingData.getRows())
		{
			if (to > trainingData.getRows())
				to = trainingData.getRows();

			trainSingleBatch(Z, A, D, dCdw, dCdb, learnRate, from, to, trExInp, trExAnswer);

			from += par.batchSize;
			to += par.batchSize;
		}

		learnRate /= par.learningRateDecrease;  // Lower the learning rate after every generation.
	}

	printEndTitle("Training the neural network is done");
}
// 'trExmpInp' and 'trExmpAnswer' are given by reference so I don't have to reallocate memory for them every time this function is called.
// They are given just for being containers (no data is used from them). 
void NeuralNetTrainer::trainSingleBatch
(std::vector<Matrix>& Z, std::vector<Matrix>& A, std::vector<Matrix>& D, std::vector<Matrix>& dCdw, std::vector<Matrix>& dCdb,
	double learningRate, size_t from, size_t to, Matrix& trExmpInp, Matrix& trExmpAnswer)
{
	// Prepare the gradient vector.
	for (size_t i = 0; i < dCdw.size(); ++i)  // Should really use 'std::for_each' here.
	{
		dCdw[i].fill(0.0);
		dCdb[i].fill(0.0);
	}

	// Go through the training examples in the batch.
	for (size_t currTrExmp = from; currTrExmp < to; ++currTrExmp)
	{
		setTrainingExample(trExmpInp, trExmpAnswer, currTrExmp);

		// Calcucate gradient for current training example.
		computeZA(Z, A, trExmpInp);
		computeD(Z, A, D, trExmpAnswer);
		computeGradient(dCdw, dCdb, A, D, trExmpInp);
	}

	// Average tha gradient vector for the training examples in the batch.
	if (par.batchSize > 1)
	{
		for (size_t i = 0; i < dCdw.size(); ++i)
		{
			dCdw[i] /= (double)par.batchSize;
			dCdb[i] /= (double)par.batchSize;
		}
	}

	descend(learningRate, dCdw, dCdb);
}

// Computes 'Z' and 'A' for the given training example.
void NeuralNetTrainer::computeZA(std::vector<Matrix>& Z, std::vector<Matrix>& A, const Matrix& trExmpInp) const
{
	// Compute the first vector column of 'Z' and 'A'. It is dependent on the training example.
	Matrix::matrixMultiplication(net.cgetWeights().front(), trExmpInp, Z.front());
	Z.front() += net.cgetBiases().front();
	A.front() = Z.front();
	NeuralNet::applySigmoidToMatrix(A.front());

	// Compute the rest of 'Z' and 'A'.
	for (size_t i = 1; i < Z.size(); ++i)
	{
		Matrix::matrixMultiplication(net.cgetWeights()[i], A[i - 1], Z[i]);
		Z[i] += net.cgetBiases()[i];

		A[i] = Z[i];
		NeuralNet::applySigmoidToMatrix(A[i]);
	}
}
void NeuralNetTrainer::computeD
(const std::vector<Matrix>& Z, const std::vector<Matrix>& A, std::vector<Matrix>& D, const Matrix& trExmpAnswer) const
{
	// Compute last vector in 'D'.
	// All of the below vectors are the same dimension.
	double* d = D.back().getRow(0);              // Pointer to the last vector in 'D'.
	const double* z = Z.back().cgetRow(0);       // Last vector in 'Z'.
	const double* a = A.back().cgetRow(0);       // Last vector in 'A'.
	const double* ans = trExmpAnswer.cgetRow(0); // Answer for the current training example.

	// Go through every element of the vector 'd' and compute it.
	while (d != D.back().cgetRow(0) + D.back().getRows())
	{
		*d = (*a - *ans) * NeuralNet::sigmoidDerivative(*z);

		++d; ++z; ++a; ++ans;
	}

	// Compute the rest of the vectors in 'D'.
	for (int i = (int)D.size() - 2; i >= 0; --i)
	{
		Matrix::matrixMultByTranspose(net.cgetWeights()[i + 1], D[i + 1], D[i]);

		d = D[i].getRow(0);
		z = Z[i].cgetRow(0);
		while (d != D[i].cgetRow(0) + D[i].getRows())
		{
			*d *= NeuralNet::sigmoidDerivative(*z);

			++d; ++z;
		}
	}
}
// Calculates the gradient for a single training example.
// VERY IMPORTANT: Adds the result to 'dCdw' and 'dCdb'. This allows for computing the gradient for several training examples
// and later averaging them out (aka stochastic gradient descent).
// The elements of 'dCdw' and 'dCdb' have to be set to 0 if we wish to calculate the gradient for a new training batch.
void NeuralNetTrainer::computeGradient
(std::vector<Matrix>& dCdw, std::vector<Matrix>& dCdb, const std::vector<Matrix>& A, const std::vector<Matrix>& D, const Matrix& trExmpInp) const
{
	// Calculate the gradient for the first weight matrix and the first bias matrix.
	// This is separate because the gradient of first weight matrix is dependent on 'trExmpInp'.
	// Go through rows.
	for (size_t i = 0; i < dCdw.front().getRows(); ++i)
	{
		*dCdb.front().getRow(i) += *D.front().cgetRow(i);    // Calculate bias change. Note the +=

		// Go through columns.
		for (size_t j = 0; j < dCdw.front().getCols(); ++j)
		{
			dCdw.front().getMatrix()[i][j] += *trExmpInp.cgetRow(j) * *D.front().cgetRow(i);   // Calculate weight change. Note the +=
		}
	}

	// Go through all weights and bias matrices after the first.
	for (size_t l = 1; l < dCdw.size(); ++l)
	{
		// Go through rows.
		for (size_t i = 0; i < dCdw[l].getRows(); ++i)
		{
			*dCdb[l].getRow(i) += *D[l].cgetRow(i);    // Calculate bias change. Note the +=

			// Go through columns.
			for (size_t j = 0; j < dCdw[l].getCols(); ++j)
			{
				dCdw[l].getMatrix()[i][j] += *A[l - 1].cgetRow(j) * *D[l].cgetRow(i);   // Calculate weight change. Note the +=
			}
		}
	}
}


// Update the weights and biases with the given gradient vector and learning rate.
// Changes 'dCdw' and 'dDcb'!!!
// 'learningRate' has to be positive.
void NeuralNetTrainer::descend(double learningRate, std::vector<Matrix>& dCdw, std::vector<Matrix>& dCdb)
{
	for (size_t i = 0; i < net.cgetWeights().size(); ++i)
	{
		dCdw[i] *= -learningRate;
		dCdb[i] *= -learningRate;

		net.getWeightMatrix(i) += dCdw[i];
		net.getBiasVector(i) += dCdb[i];
	}
}

void NeuralNetTrainer::setDimensionsZAD(std::vector<Matrix>& Z, std::vector<Matrix>& A, std::vector<Matrix>& D) const
{
	// Set number of vector columns.
	Z.resize(net.cgetNeuronCounts().size() - 1);
	A.resize(net.cgetNeuronCounts().size() - 1);
	D.resize(net.cgetNeuronCounts().size() - 1);

	// Set the dimensions of the vector columns.
	for (size_t i = 0; i < Z.size(); ++i)
	{
		Z[i] = { net.cgetNeuronCounts()[i + 1], 1 };
		A[i] = { net.cgetNeuronCounts()[i + 1], 1 };
		D[i] = { net.cgetNeuronCounts()[i + 1], 1 };
	}
}
void NeuralNetTrainer::setTrainingExample(Matrix& trExmpInp, Matrix& trExmpAnswer, size_t index) const
{
	// Set 'trExmpInp'.
	memcpy(*trExmpInp.getMatrix(), trainingData.cgetRow(index), sizeof(double) * trainingData.getCols());

	// Set 'trExmpAnswer'.
	memcpy(*trExmpAnswer.getMatrix(), trainingDataAnswers.cgetRow(index), sizeof(double) * trainingDataAnswers.getCols());
}


void NeuralNetTrainer::fill(Interval a)
{
	for (size_t i = 0; i < net.cgetWeights().size(); ++i)
	{
		net.getWeightMatrix(i).fill(a.from, a.to);
		net.getBiasVector(i).fill(a.from, a.to);
	}
}
