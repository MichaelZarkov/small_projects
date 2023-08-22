
#include <exception>
#include <fstream>
#include <iostream>
#include <new>           // std::nothrow
#include <utility>       // std::move
#include "SquareMaze.h"

unsigned char** allocMatrix(size_t rows, size_t cols) {
	if (rows == 0 || cols == 0)
		throw "Invalid size (matrix)";

	unsigned char** matrix = new(std::nothrow) unsigned char* [rows];
	if (!matrix)
		throw "Fail to alloc memory (matrix)!";
	matrix[0] = new(std::nothrow) unsigned char[rows * cols];
	if (!matrix[0]) {
		delete[] matrix;
		throw "Fail to alloc memory (matrix)!";
	}
	for (size_t i = 1; i < rows; ++i)
		matrix[i] = matrix[i - 1] + cols;
	return matrix;
}


//class SquareMaze_OneBit----------------------------------------------------------------------------

SquareMaze_OneBit::SquareMaze_OneBit(size_t size)
	: columns(size / 8 + (size % 8 ? 1 : 0)), size(size), start{ size - 1, size - 1 }, end{0, 0}, maze(allocMatrix(size, columns))
{
	fillWith(0);
}
SquareMaze_OneBit::SquareMaze_OneBit(const SquareMaze_OneBit& other) {
	copy(other);
}
SquareMaze_OneBit::SquareMaze_OneBit(SquareMaze_OneBit&& rvalue) noexcept {
	copy(std::move(rvalue));
}
SquareMaze_OneBit& SquareMaze_OneBit::operator=(const SquareMaze_OneBit& other) {
	if (this != &other) {
		SquareMaze_OneBit temp{ other };    // Strong exception safety.
		free();
		copy(std::move(temp));
	}
	return *this;
}
SquareMaze_OneBit& SquareMaze_OneBit::operator=(SquareMaze_OneBit&& rvalue) noexcept {
	if (this != &rvalue) {
		free();
		copy(std::move(rvalue));
	}
	return *this;
}
SquareMaze_OneBit::~SquareMaze_OneBit() {
	free();
}

size_t SquareMaze_OneBit::getSize() const {
	return size;
}
// Maybe should change this function to return unsigned char?
bool SquareMaze_OneBit::getPixel(size_t row, size_t col) const {
	if (row >= size || col >= size)
		throw "Out of bounds in class 'SquareMaze_OneBit'!";

	// Extracts the bit in the given position using bit shifts.
	// If the bit is 0, returns 0.
	// If the bit is 1, returns 1.
	// unsigned char cast because when doing bitwise operations the compiler
	// casts it to a 4 byte value (don't know how else to deal with that).
	return (unsigned char)(maze[row][col / 8] << col % 8) >> 7; 
}
size_t SquareMaze_OneBit::getStartRow() const { return start.x; }
size_t SquareMaze_OneBit::getStartCol() const { return start.y; }
size_t SquareMaze_OneBit::getEndRow() const { return end.x; }
size_t SquareMaze_OneBit::getEndCol() const { return end.y; }

void SquareMaze_OneBit::setPixel(size_t row, size_t col, bool value) const {
	if (row >= size || col >= size)
		throw "Out of bounds in class 'SquareMaze_OneBit'!";

	// Maybe this could be made better.
	if (value) {
		// 'start' and 'end' cannot be 1 (walls).
		if ((row == start.x && col == start.y) || (row == end.x && col == end.y))
		{
			return;
		}

		// 'A = 128 >> col % 8' - make a byte with a 1 at the position of the pixel and all other positions 0.
		// Do a bitwise OR on 'A' and the byte in which the pixel is (this changes ONLY the bit at the position of the pixel to 1).
		// Ex: 
		// Position = 3 (0 <= position <= 7)   
		//   
		//     10111000        00001100
		//     00010000        00010000
		//  OR --------     OR --------
		// RES 10111000    RES 00011100
		maze[row][col / 8] = maze[row][col / 8] | (128 >> (col % 8));
	}
	else {
		// 'A = 128 >> col % 8' - make a byte with a 1 at the position of the pixel and all other positions 0.
		// Do a bitwise NOT on 'A'.
		// Do a bitwise AND on 'A' and the byte in which the pixel is (this changes only the bit at the position of the pixel to 0).
		maze[row][col / 8] = maze[row][col / 8] & ~(128 >> col % 8);
	}
}
void SquareMaze_OneBit::setStart(size_t row, size_t col) {
	if (row >= size || col >= size)
		throw "Out of bounds in class 'SquareMaze_OneBit'!";
	start.x = row;
	start.y = col;
	setPixel(row, col, 0);    // 'start' has to be 0 (empty space).
}
void SquareMaze_OneBit::setEnd(size_t row, size_t col) {
	if (row >= size || col >= size)
		throw "Out of bounds in class 'SquareMaze_OneBit'!";
	end.x = row;
	end.y = col;
	setPixel(row, col, 0);    // 'end' has to be 0 (empty space).
}

void SquareMaze_OneBit::fillWith(bool val) const {
	unsigned char fill = 0;  // 00000000
	if (val)
		fill = 255;    // 11111111

	for (size_t i = 0; i < size * columns; ++i)
		maze[0][i] = fill;

	setPixel(start.x, start.y, 0);    // 'start' has to be 0 (empty space).
	setPixel(end.x, end.y, 0);        // 'end' has to be 0 (empty space).
}
void SquareMaze_OneBit::border() {
	if (size <= 2)    // The border does not make sense for 1x1 or 2x2 maze (the hole maze will be walls).
		return;

	// Move 'start' and 'end' inside the border.
	setStart(size - 2, size - 2);
	setEnd(1, 1);

	for (size_t i = 0; i < size; ++i) {
		setPixel(0, i, 1);        // Top
		setPixel(size - 1, i, 1); // Bottom
		setPixel(i, 0, 1);        // Left
		setPixel(i, size - 1, 1); // Right
	}
}

void SquareMaze_OneBit::print() const {
	std::cout << "Maze size: " << size << " by " << size << std::endl;
	std::cout << "Start point is (" << start.x << ", " << start.y << ")" << std::endl;
	std::cout << "End point is (" << end.x << ", " << end.y << ")" << std::endl;
	for (size_t i = 0; i < size; ++i) {
		for (size_t j = 0; j < size; ++j) {
			if (getPixel(i, j))
				std::cout << char(219);    // White square.
			else
				std::cout << '-';
		}
		std::cout << std::endl;
	}
}

void SquareMaze_OneBit::free() {
	if (maze)
		delete[] * maze;
	delete[] maze;
}
void SquareMaze_OneBit::copy(const SquareMaze_OneBit& other) {
	// If 'size' of the mazes is the same, there is no need to reallocate
	// the same matrix, just change the matrix values with the loop below.
	if (size != other.size) {
		maze = allocMatrix(other.size, other.columns);
	}

	for (size_t i = 0; i < other.size * other.columns; ++i)
		maze[0][i] = other.maze[0][i];
	
	columns = other.columns;
	size = other.size;
	start = other.start;
	end = other.end;
}
void SquareMaze_OneBit::copy(SquareMaze_OneBit&& rvalue) {
	columns = rvalue.columns;
	size = rvalue.size;
	start = rvalue.start;
	end = rvalue.end;
	maze = rvalue.maze;

	rvalue.maze = nullptr;
}



//class SquareMaze_TwoBits---------------------------------------------------------------------------

SquareMaze_TwoBits::SquareMaze_TwoBits(size_t size)
	: columns(size / 4 + (size % 4 ? 1 : 0)), size(size), start{ size - 1, size - 1 }, end{ 0, 0 }, maze(allocMatrix(size, columns))
{
	fillWith(0);
}
SquareMaze_TwoBits::SquareMaze_TwoBits(const SquareMaze_TwoBits& other) {
	copy(other);
}
SquareMaze_TwoBits::SquareMaze_TwoBits(SquareMaze_TwoBits&& rvalue) noexcept {
	copy(std::move(rvalue));
}
SquareMaze_TwoBits& SquareMaze_TwoBits::operator=(const SquareMaze_TwoBits& other) {
	if (this != &other) {
		SquareMaze_TwoBits temp{ other };    // Strong exception safety.
		free();
		copy(std::move(temp));
	}
	return *this;
}
SquareMaze_TwoBits& SquareMaze_TwoBits::operator=(SquareMaze_TwoBits&& rvalue) noexcept {
	if (this != &rvalue) {
		free();
		copy(std::move(rvalue));
	}
	return *this;
}
SquareMaze_TwoBits::~SquareMaze_TwoBits() {
	free();
}

size_t SquareMaze_TwoBits::getSize() const {
	return size;
}
unsigned char SquareMaze_TwoBits::getPixel(size_t row, size_t col) const {
	if (row >= size || col >= size)
		throw "Out of bounds in class 'SquareMaze_TwoBits'!";

	// Extracts the pair of bits in the given position using bit shifts.
	// unsigned char cast because when doing bitwise operations the compiler
	// casts it to a 4 byte value (don't know how else to deal with that).
	/*
		Moves the two pixel bits to the left making them the two most significant bits,
		then moves them to the right, making them the two lest significant bits.
		
		|Ex 1:                     |Ex 2:
		|                          |
		| col == 3                 | col == 1
		|                          |
		| maze byte: 00000011      | maze byte: 00110000
        | step 1:    11000000      | step 1:    11000000
		| step 2:    00000011      | step 2:    00000011
		| return:    00000011      | return:    00000011

	*/
	return (unsigned char)(maze[row][col / 4] << ((col % 4) * 2)) >> 6;
}
size_t SquareMaze_TwoBits::getStartRow() const { return start.x; }
size_t SquareMaze_TwoBits::getStartCol() const { return start.y; }
size_t SquareMaze_TwoBits::getEndRow() const { return end.x; }
size_t SquareMaze_TwoBits::getEndCol() const { return end.y; }

void SquareMaze_TwoBits::setPixel(size_t row, size_t col, unsigned char value) const {
	if (row >= size || col >= size)
		throw "Out of bounds in class 'SquareMaze_TwoBits'!";

	if (value > 3)
		throw "Invalid pixel value in class 'SquareMaze_TwoBits'!";

	// 'start' and 'end' cannot be 1 (walls).
	if (value == 1 && ((row == start.x && col == start.y) || (row == end.x && col == end.y)))
		return;
	
	int pos = (3 - (col % 4)) * 2;
	unsigned char mask = 3 << pos;    // 00110000
	maze[row][col / 4] = (maze[row][col / 4] & ~mask) | (value << pos);

	/*
		Explanation:

			// 1. Original byte.
			unsigned char originalByte = maze[row][col / 4];

			// 2. Make '11' byte in pixel position.
			unsigned char mask = 192 >> ((col % 4) * 2);    // 192 == 11000000

			// 3. Change original to 00 in pixel position.
			originalByte = originalByte & ~mask;            // xx00xxxx

			// 4. Bring 'value' to position.
			value = value << ((3 - (col % 4)) * 2);    // 00xx0000

			// 5. Change to final.
			originalByte = originalByte | value;    // Done.


		Better explanation for one bit change:

			We first create a mask that has set bit only
			at given position using bit wise shift.
				  mask = 1 << position

			Then to change value of bit to b, we first
			make it 0 using below operation
				  value & ~mask

			After changing it 0, we change it to b by
			doing or of above expression with following
			(b << p) & mask, i.e., we return
				  ((n & ~mask) | (b << p))
	*/
}
void SquareMaze_TwoBits::setStart(size_t row, size_t col) {
	if (row >= size || col >= size)
		throw "Out of bounds in class 'SquareMaze_TwoBits'!";
	start.x = row;
	start.y = col;
	setPixel(row, col, 0);    // 'start' has to be not a wall.
}
void SquareMaze_TwoBits::setEnd(size_t row, size_t col) {
	if (row >= size || col >= size)
		throw "Out of bounds in class 'SquareMaze_TwoBits'!";
	end.x = row;
	end.y = col;
	setPixel(row, col, 0);    // 'end' has to be not a wall.
}

void SquareMaze_TwoBits::fillWith(unsigned char value) const {
	// Make a correct byte.
	for (size_t i = 0; i < 3; ++i)
		value = value * 2 + value;

	for (size_t i = 0; i < size * columns; ++i)
		maze[0][i] = value;

	setPixel(start.x, start.y, 0);    // 'start' has to be not a wall.
	setPixel(end.x, end.y, 0);        // 'end' has to be not a wall.
}
// The same function as in class 'SquareMaze_OneBit'.
void SquareMaze_TwoBits::border() {
	if (size <= 2)    // The border does not make sense for 1x1 or 2x2 maze (the hole maze will be walls).
		return;

	// Move 'start' and 'end' inside the border.
	setStart(size - 2, size - 2);
	setEnd(1, 1);

	for (size_t i = 0; i < size; ++i) {
		setPixel(0, i, 1);        // Top
		setPixel(size - 1, i, 1); // Bottom
		setPixel(i, 0, 1);        // Left
		setPixel(i, size - 1, 1); // Right
	}
}

// Save as Raw PixMap image.
void SquareMaze_TwoBits::saveAsPPM(const char* filePath) const {
	std::ofstream file(filePath, std::ios::binary);
	if (!file)
		throw "Can not open file in class 'SquareMaze_TwoBits!";

	// Magic number.
	file << "P6" << '\r';
	// Comment.
	file << "# Maze size = " << size << "; start(" << start.x << ", " << start.y << "), end(" << end.x << ", " << end.y << ")\r";
	// Image width.
	file << size << "\r";
	// Image height.
	file << size << "\r";
	// Colour depth.
	file << 255 << "\r";

	// Pixel values.
	for (size_t i = 0; i < size; ++i)
	{
		for (size_t j = 0; j < size; ++j)
		{
			char one = -1;    // 11111111
			char zero = 0;    // 00000000
			switch (getPixel(i, j))
			{
			case 0:
				// Black
				file.write(&zero, 1);    // Red
				file.write(&zero, 1);    // Green
				file.write(&zero, 1);    // Blue
				break;
			case 1:
				// White
				file.write(&one, 1);    // Red
				file.write(&one, 1);    // Green
				file.write(&one, 1);    // Blue
				break;
			case 2:
				// Green
				file.write(&zero, 1);    // Red
				file.write(&one, 1);     // Green
				file.write(&zero, 1);    // Blue
				break;
			case 3:
				// Red
				file.write(&one, 1);    // Red
				file.write(&zero, 1);    // Green
				file.write(&zero, 1);    // Blue
			}
		}
	}

}

void SquareMaze_TwoBits::print() const {
	std::cout << "Maze size: " << size << " by " << size << std::endl;
	std::cout << "Start point is (" << start.x << ", " << start.y << ")" << std::endl;
	std::cout << "End point is (" << end.x << ", " << end.y << ")" << std::endl;
	for (size_t i = 0; i < size; ++i) {
		for (size_t j = 0; j < size; ++j) {
			switch (getPixel(i, j)) {
			case 0:
				std::cout << '-';
				break;
			case 1:
				std::cout << char(219);    // White square.
				break;
			case 2:
				std::cout << '#';
				break;
			case 3:
				std::cout << '|';
			}
		}
		std::cout << std::endl;
	}
}

void SquareMaze_TwoBits::free() {
	if (maze)
		delete[] * maze;
	delete[] maze;
}
void SquareMaze_TwoBits::copy(const SquareMaze_TwoBits& other) {
	// If 'size' of the mazes is the same, there is no need to reallocate
	// the same matrix, just change the matrix values with the loop below.
	if (size != other.size) {
		maze = allocMatrix(other.size, other.columns);
	}
	
	for (size_t i = 0; i < other.size * other.columns; ++i)
		maze[0][i] = other.maze[0][i];
	
	columns = other.columns;
	size = other.size;
	start = other.start;
	end = other.end;
}
void SquareMaze_TwoBits::copy(SquareMaze_TwoBits&& rvalue) {
	columns = rvalue.columns;
	size = rvalue.size;
	start = rvalue.start;
	end = rvalue.end;
	maze = rvalue.maze;

	rvalue.maze = nullptr;
}