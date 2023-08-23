
#include <cstring>
#include <new>       // std::bad_alloc, std::nothrow
#include <random>
#include <stdexcept> // std::out_of_range
#include <utility>   // std::move()
#include "Matrix.h"


Matrix::Matrix(size_t rows, size_t cols)
	: matrix(allocateMatrix(rows, cols)), rows(rows), cols(cols)
{}
Matrix::Matrix(const Matrix& other)
{
	copy(other);
}
Matrix::Matrix(Matrix&& rvalue) noexcept
{
	copy(std::move(rvalue));
}
// Exception safety: strong guarantee.
Matrix& Matrix::operator=(const Matrix& other)
{
	if (this != &other)
	{
		// If they are the same dimensions not need to realloc mem.
		if (rows == other.rows && cols == other.cols)
		{
			memcpy(*matrix, *other.cgetMatrix(), sizeof(double) * rows * cols);
			return *this;
		}

		Matrix temp{ other };
		free();
		copy(std::move(temp));
	}
	return *this;
}
Matrix& Matrix::operator=(Matrix&& rvalue) noexcept
{
	if (this != &rvalue)
	{
		free();
		copy(std::move(rvalue));
	}
	return *this;
}
Matrix::~Matrix()
{
	free();
}

double const* const* Matrix::cgetMatrix() const
{
	return matrix;
}
double* const* Matrix::getMatrix()
{
	return matrix;
}
double const* Matrix::cgetRow(size_t index) const
{
	return matrix[index];
}
double* Matrix::getRow(size_t index)
{
	return matrix[index];
}
size_t Matrix::getRows() const
{
	return rows;
}
size_t Matrix::getCols() const
{
	return cols;
}

void Matrix::fill(double value)
{
	double* p = *matrix;
	while (p != matrix[rows - 1] + cols)
	{
		*p = value;
		++p;
	}
}
// Throws 'std::invalid_argument' if 'from >= to'.
void Matrix::fill(double from, double to)
{
	if (from >= to)
	{
		std::invalid_argument invArg("Invalid range of real values in function 'fill()' from class Matrix!");
		throw invArg;
	}

	std::random_device rd;
	std::uniform_real_distribution<double> dis{ from, to };

	double* p = *matrix;
	while (p != matrix[rows - 1] + cols)
	{
		*p = dis(rd);
		++p;
	}
}

bool Matrix::operator==(const Matrix& other)
{
	if (rows != other.rows || cols != other.cols)
		throw "Cannot compare matrices with different dimensions!";

	return !memcmp(*matrix, *other.cgetMatrix(), sizeof(double) * rows * cols);
}
Matrix& Matrix::operator*=(double scalar)
{
	double* p = *matrix;
	while (p != matrix[rows - 1] + cols)
	{
		*p *= scalar;
		++p;
	}
	return *this;
}
Matrix& Matrix::operator/=(double scalar)
{
	this->operator*=(1.0 / scalar);
	return *this;
}
Matrix& Matrix::operator+=(const Matrix& other)
{
	if (rows != other.rows || cols != other.cols)
		throw "Cannot sum matrices with different dimensions!";

	// The idea here is that pointers are faster than indexing, which might not be true;
	double* thisPtr = *matrix;
	double* otherPtr = *other.matrix;
	while (thisPtr != matrix[rows - 1] + cols)
	{
		*thisPtr += *otherPtr;
		++thisPtr;
		++otherPtr;
	}
	return *this;
}

// Overwrites the matrix values in 'result'.
// Throws an exception if the dimensions of the matrices are incorrect.
// Currently I think that this function is slow. Should try optimizing it!!!
void Matrix::matrixMultiplication(const Matrix& m1, const Matrix& m2, Matrix& result)
{
	// Check if dimensions are correct.
	if (m1.getCols() != m2.getRows() || result.getRows() != m1.getRows() || result.getCols() != m2.getCols())
	{
		throw "Cannot multiply matrices because the dimensions are incorrect!";
	}

	for (size_t i = 0; i < result.getRows(); ++i)
	{
		for (size_t j = 0; j < result.getCols(); ++j)
		{
			result.getMatrix()[i][j] = 0;
			for (size_t k = 0; k < m1.getCols(); ++k)
			{
				result.getMatrix()[i][j] += m1.cgetMatrix()[i][k] * m2.cgetMatrix()[k][j];
			}
		}
	}
}
// Multiplies the transpose of 'm1' by 'm2' (just 'm1' is transposed).
// Overwrites the matrix values in 'result'.
// Throws an exception if the dimensions of the matrices are incorrect.
// Currently I think that this function is slow. Should try optimizing it!!!
void Matrix::matrixMultByTranspose(const Matrix& m1, const Matrix& m2, Matrix& result)
{
	if (m1.getRows() != m2.getRows() || result.getRows() != m1.getCols() || result.getCols() != m2.getCols())
	{
		throw "Cannot multiply matrices because the dimensions are incorrect!";
	}

	for (size_t i = 0; i < result.getRows(); ++i)
	{
		for (size_t j = 0; j < result.getCols(); ++j)
		{
			result.getMatrix()[i][j] = 0;
			for (size_t k = 0; k < m2.getRows(); ++k)
			{
				result.getMatrix()[i][j] += m1.cgetMatrix()[k][i] * m2.cgetMatrix()[k][j];
			}
		}
	}
}

// Allocates the whole matrix at once.
// Throws std::bad_alloc if mem alloc fails.
// Throws std::out_of_range if 'rows' or 'cols' is 0.
double** Matrix::allocateMatrix(size_t rows, size_t cols) const
{
	if (rows == 0 || cols == 0)
	{
		std::out_of_range err("Cannot allocate matrix with 0 rows or columns!");
		throw err;
	}

	double** matrix = new double* [rows];
	*matrix = new(std::nothrow) double[rows * cols];
	if (!*matrix)
	{
		delete[] matrix;
		std::bad_alloc ba;
		throw ba;
	}
	for (size_t i = 1; i < rows; ++i)
		matrix[i] = matrix[i - 1] + cols;

	return matrix;
}

void Matrix::copy(const Matrix& other)
{
	this->matrix = allocateMatrix(other.rows, other.cols);
	this->rows = other.rows;
	this->cols = other.cols;
	memcpy(*matrix, *other.cgetMatrix(), sizeof(double) * rows * cols);
}
void Matrix::copy(Matrix&& rvalue) noexcept
{
	this->matrix = rvalue.matrix;
	this->rows = rvalue.rows;
	this->cols = rvalue.cols;

	rvalue.matrix = nullptr;
	rvalue.rows = 0;
	rvalue.cols = 0;
}
void Matrix::free()
{
	if (matrix)
	{
		delete[] * matrix;
		delete[] matrix;
	}		
}

// File specification for Matrix:
// - 8 bytes of unsigned int (rows) (if size_t is 8 bytes)
// - 8 bytes of unsigned int (cols) (if size_t is 8 bytes)
// - rows * cols * sizeof(double) bytes for the matrix elements.
// 
// Strong exception safety.
// @exception: const char*
std::ifstream& operator>>(std::ifstream& file, Matrix& m)
{
	if (!file.is_open())
	{
		throw "Cannot read matrix! No file is opened!";
	}

	size_t rows = 0, cols = 0;
	file.read((char*)&rows, sizeof(rows));
	file.read((char*)&cols, sizeof(cols));

	if (!rows || !cols)
	{
		throw "File is corrupted! Invalid matrix dimensions! 0 rows or columns!";
	}
	if (!file.good())
	{
		throw "Failure reading from file!";
	}

	Matrix matrixFromFile{ rows, cols };
	file.read((char*)matrixFromFile.getRow(0), rows * cols * sizeof(double));    // Read elements of matrix.

	m = std::move(matrixFromFile);

	return file;
}
std::ofstream& operator<<(std::ofstream& file, const Matrix& m)
{
	if (!file.is_open())
	{
		throw "Cannot write matrix to file! No file is opened!";
	}

	size_t rows = m.getRows(), cols = m.getCols();
	file.write((const char*)&rows, sizeof(rows));
	file.write((const char*)&cols, sizeof(cols));
	file.write((const char*)m.cgetRow(0), rows * cols * sizeof(double));
	
	// Should I check here if the stream is OK?

	return file;
}