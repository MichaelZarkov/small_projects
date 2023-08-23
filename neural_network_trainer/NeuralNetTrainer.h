#pragma once
#include "NeuralNet.h"

/*
	For explanation of neural networks and the Gradient Descend algorigthm (which is implemented here)
	see the book "http://neuralnetworksanddeeplearning.com/index.html". Everithing I used to make the
	'NeuralNet' class is from this book.
*/


/*
	To do:
	  - after the user inputs the testing file before training, save it in the object
	    for testing after the training (because at the moment the user has to enter the
		same file twice).
*/
class NeuralNetTrainer
{
public:
	NeuralNetTrainer();

private:
	struct Interval   // Real number interval.
	{
		double from;
		double to;
	};
	struct TrainingParams    // Container for the training parameters.
	{
		Interval weightAndBiasValueRange;
		size_t batchSize;
		size_t generationCount;
		double learningRate;
		double learningRateDecrease;
	};

private:	// Console input, output, text formatting and initialization.

	// Console initialization.
	void initializeNeuralNetFromConsole();
	void inputTrainingData();
	size_t inputHiddenLayerCount() const;
	void inputNeuronCountInHiddenLayers(std::vector<size_t>&) const;

	void initializationMessage() const;

	// Training parameters input.
	void inputTrainingParameters();
	Interval inputValueRange() const;
	size_t inputBatchSize() const;
	size_t inputGenerationCount() const;
	double inputLearningRate() const;
	double inputLearningRateDecrease() const;

	// Save to file.
	void saveToFile() const;
	bool wantToSave() const;
	void inputFileName(char buff[]) const;
	void save(const char* fileName) const;

	bool exist(const char* fileName) const;

	// Printing and formatting.
	void printStartTitle(const char* str) const;
	void printEndTitle(const char* str) const;
	void printError(const char*) const;
	void printText(const char*) const;

	void setConsoleTextUser() const;            // Sets text format when the user is typing.
	void setConsoleTextNeuralNet() const;       // Sets text format when the class is writing to console.
	void setConsoleTextError() const;           // Sets text format when printing error message.

	void clearSTDcin() const;

private:	// Testing.

	void testNeuralNet() const;
	void inputTestingDataFromFile(Matrix& testDataInputs, Matrix& testDataAnswers) const;
	size_t testNeuralNet(const Matrix& testDataInputs, const Matrix& testDataAnswers) const;             // Returns how many correct answers.

private:	// Training logic.

	void trainNeuralNet();
	void trainSingleBatch
	(std::vector<Matrix>& Z, std::vector<Matrix>& A, std::vector<Matrix>& D, std::vector<Matrix>& dCdw, std::vector<Matrix>& dCdb,
	 double learningRate, size_t from, size_t to, Matrix& trExmpInp, Matrix& trExmpAnswer);

	void computeZA(std::vector<Matrix>& Z, std::vector<Matrix>& A, const Matrix& trExmpInp) const;
	void computeD(const std::vector<Matrix>& Z, const std::vector<Matrix>& A, std::vector<Matrix>& D, const Matrix& trExmpAnswer) const;
	void computeGradient(std::vector<Matrix>& dCdw, std::vector<Matrix>& dCdb, const std::vector<Matrix>& A, const std::vector<Matrix>& D, const Matrix& trExmpInp) const;

	void descend(double learningRate, std::vector<Matrix>& dCdw, std::vector<Matrix>& dCdb);

	void setDimensionsZAD(std::vector<Matrix>& Z, std::vector<Matrix>& A, std::vector<Matrix>& D) const;
	void setTrainingExample(Matrix& trExmpInp, Matrix& trExmpAnswer, size_t index) const;


private:
	void fill(Interval);     // Sets the weights and biases with random values from the given range.


private:
	NeuralNet net;

	Matrix trainingData;           // Inputs for input neuron layer.
	Matrix trainingDataAnswers;    // Correct outputs for 'trainingData'. 

	TrainingParams par;

	static const size_t BUFF_SIZE;
	static const char START_TITLE_SYMBOLS[][8];    // Used for styling up the console output.
	static const char END_TITLE_SYMBOLS[][8];      // Used for styling up the console output.
};

