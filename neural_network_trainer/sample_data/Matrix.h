#pragma once
#include <fstream>

class Matrix
{
public:
	
	Matrix(size_t rows = 1, size_t cols = 1);
	Matrix(const Matrix&);
	Matrix(Matrix&&) noexcept;
	Matrix& operator=(const Matrix&);
	Matrix& operator=(Matrix&&) noexcept;
	~Matrix();

	double const* const* cgetMatrix() const;
	double* const* getMatrix();
	double const* cgetRow(size_t index) const;     // Does not check if 'index' is valid.
	double* getRow(size_t index);                  // Does not check if 'index' is valid.
	size_t getRows() const;
	size_t getCols() const;

	void fill(double value);
	void fill(double from, double to);             // Fills matrix with random values in the given range.

	bool operator==(const Matrix& other);          // Returns true if all elements in the matrices are equal.
	Matrix& operator*=(double scalar);
	Matrix& operator/=(double scalar);
	Matrix& operator+=(const Matrix& other);       // Rows and cols of 'other' have to be the same as 'this'.
	                                               // Throws const char* if they are not.
public:
	static void matrixMultiplication(const Matrix& m1, const Matrix& m2, Matrix& result);
	static void matrixMultByTranspose(const Matrix& m1, const Matrix& m2, Matrix& result);

private:

	double** allocateMatrix(size_t rows, size_t cols) const;

	void copy(const Matrix&);
	void copy(Matrix&&) noexcept;
	void free();

private:
	
	double** matrix{ nullptr };
	size_t rows{ 0 };
	size_t cols{ 0 };
};


std::ifstream& operator>>(std::ifstream&, Matrix&);        // 'file' should be opened in binary.
std::ofstream& operator<<(std::ofstream&, const Matrix&);  // 'file' should be opened in binary.

