
#pragma once

// The maze is a square grid of pixels.
// Every pixel in the maze is one bit. It is basically a square PBM image.
// '0' - empty space / no wall / white
// '1' - wall / black
class SquareMaze_OneBit
{
public:

	SquareMaze_OneBit(size_t size = 4);
	SquareMaze_OneBit(const SquareMaze_OneBit&);
	SquareMaze_OneBit(SquareMaze_OneBit&&) noexcept;
	SquareMaze_OneBit& operator=(const SquareMaze_OneBit&);
	SquareMaze_OneBit& operator=(SquareMaze_OneBit&&) noexcept;
	~SquareMaze_OneBit();

	size_t getSize() const;
	bool getPixel(size_t row, size_t col) const;
	size_t getStartRow() const;
	size_t getStartCol() const;
	size_t getEndRow() const;
	size_t getEndCol() const;

	void setPixel(size_t row, size_t col, bool value) const;
	void setStart(size_t row, size_t col);
	void setEnd(size_t row, size_t col);
	
	void fillWith(bool) const;    // Fills the maze/matrix with 0s or 1s.
	void border();                // Puts one pixel wide border around the perimeter. Moves 'start' and 'end' inside the border.
	
	void print() const;

private:

	void free();
	void copy(const SquareMaze_OneBit&);
	void copy(SquareMaze_OneBit&&);

private:

	struct Point {
		size_t x, y;
	};

	size_t columns;          // Number of columns in the matrix 'maze' in bytes. Can be calculated from 'size':
							 // 'columns == size / 8 + (size % 8 ? 1 : 0)'
	size_t size;             // Height and width of the maze. Number of rows in matrix 'maze' in bytes.
	Point start;             // Default start is at the bottom right corner. Has to be 0 (empty space).
	Point end;               // Default end is at the top left corner. Has to be 0 (empty space).
	unsigned char** maze;    // Pixels of the maze.
};


//  - The maze is a square grid of pixels.
//  - Every pixel in the maze is two bits.
//  - If one row of pixels doesn't occupy a whole number of bytes, 
//    padding bits are left at the last byte of the row, so every row starts in a new byte.
// '0' - empty space / no wall / white
// '1' - wall / black
// '2' and '3' - not used for anything in this class. Can be used to mark a solution path of the maze.
class SquareMaze_TwoBits
{
public:

	SquareMaze_TwoBits(size_t size = 4);
	SquareMaze_TwoBits(const SquareMaze_TwoBits&);
	SquareMaze_TwoBits(SquareMaze_TwoBits&&) noexcept;
	SquareMaze_TwoBits& operator=(const SquareMaze_TwoBits&);
	SquareMaze_TwoBits& operator=(SquareMaze_TwoBits&&) noexcept;
	~SquareMaze_TwoBits();

	size_t getSize() const;
	unsigned char getPixel(size_t row, size_t col) const;
	size_t getStartRow() const;
	size_t getStartCol() const;
	size_t getEndRow() const;
	size_t getEndCol() const;

	void setPixel(size_t row, size_t col, unsigned char value) const;
	void setStart(size_t row, size_t col);
	void setEnd(size_t row, size_t col);
	
	void fillWith(unsigned char) const;  // Fills the maze/matrix.
	void border();                       // Puts one pixel wide border around the perimeter. Moves 'start' and 'end' inside the border.

	void saveAsPPM(const char* filePath) const;

	void print() const;

private:

	void free();
	void copy(const SquareMaze_TwoBits&);
	void copy(SquareMaze_TwoBits&&);

private:

	struct Point {
		size_t x, y;
	};

	size_t columns;          // Number of columns in the matrix 'maze' in bytes. Can be calculated from 'size':
							 // 'columns == size / 4 + (size % 4 ? 1 : 0)'
	size_t size;             // Height and width of the maze. Number of rows in matrix 'maze' in bytes.
	Point start;             // Default start is at the bottom right corner. Has to be 0 (empty space).
	Point end;               // Default end is at the top left corner. Has to be 0 (empty space).
	unsigned char** maze;    // Pixels of the maze.
	
};