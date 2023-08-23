
#include <fstream>
#include "Matrix.h"

// Generates training and testing data for neural network
// and saves it to file.
// The generated data computes the average of the input neurons.
void generateDataForAverage(const char* filePath)
{
	size_t numberOfExamples = 100'000;
	Matrix trDataInp{ numberOfExamples, 100 };
	Matrix trDataAns{ numberOfExamples, 10 };

	trDataInp.fill(0.0, 1.0);   // Set input.
	trDataAns.fill(0.0);
	for (size_t currExmp = 0; currExmp < numberOfExamples; ++currExmp)
	{
		// Find correct answer.
		double sumOfInput = 0.0;
		for (size_t i = 0; i < trDataInp.getCols(); ++i)
			sumOfInput += trDataInp.cgetMatrix()[currExmp][i];

		trDataAns.getMatrix()[currExmp][(size_t)(sumOfInput / 10.0)] = 1.0;  // Set answer.
	}

	std::ofstream file(filePath, std::ios::binary);
	file << trDataInp << trDataAns;
	file.close();
}
