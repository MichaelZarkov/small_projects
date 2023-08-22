
#include <exception>
#include <fstream>
#include <new>            // 'std::nothrow'
#include <utility>        // 'std::move()'
#include "PixMap_Image.h"


const unsigned long long PixMap_Image::MAX_IMAGE_SIZE = 4294967295;    // 2^32 - 1


PixMap_Image::PixMap_Image(unsigned IW, unsigned IH, unsigned CD, Image_Type IT)
{
	if (IW == 0 || IH == 0 || CD > 255)
		throw "Invalid PixMap image data!";

	matrixCols = calculateMatrixColumns(IW, IT);
	matrixRows = IH;

	if (MAX_IMAGE_SIZE < (unsigned long long)matrixCols * (unsigned long long)matrixRows)
		throw "Image size too big!";

	imageWidth = IW;
	imageHeight = IH;
	colourDepth = CD;    // Can be 0.
	imageType = IT;

	pixelValues = allocateMatrix(matrixRows, matrixCols);
}
PixMap_Image::PixMap_Image(std::ifstream& ifile)
	: pixelValues(nullptr)
{
	operator>>(ifile, *this);
}
PixMap_Image::PixMap_Image(const PixMap_Image& other) {
	copy(other);
}
PixMap_Image::PixMap_Image(PixMap_Image&& rvalue) noexcept {
	copy(std::move(rvalue));
}
PixMap_Image& PixMap_Image::operator=(const PixMap_Image& other) {
	if (this != &other) {
		PixMap_Image temp{ other };    // Strong exception safety.
		operator=(std::move(temp));    // Call move operator=
	}
	return *this;
}
PixMap_Image& PixMap_Image::operator=(PixMap_Image&& rvalue) noexcept {
	if (this != &rvalue) {
		free();
		copy(std::move(rvalue));
	}
	return *this;
}
PixMap_Image::~PixMap_Image() {
	free();
}

// Note the return type!
// The pixel values can be changed but the pointers to the start of every row cannot.
unsigned char* const* PixMap_Image::getPixelValues() { return pixelValues; }
unsigned char const* const* PixMap_Image::getPixelValues() const { return pixelValues; }
unsigned PixMap_Image::getMatrixCols() const  { return matrixCols; }
unsigned PixMap_Image::getMatrixRows() const  { return matrixRows; }
unsigned PixMap_Image::getImageWidth() const  { return imageWidth; }
unsigned PixMap_Image::getImageHeight() const { return imageHeight; }
unsigned PixMap_Image::getColourDepth() const { return colourDepth; }
PixMap_Image::Image_Type PixMap_Image::getImageType() const { return imageType; }

// Ex: If 'IW == 20' and 'IT == PPM' then 'MC == 60',
// every pixel has 1 byte per colour (RGB), in total 3 bytes per pixel.
// Function expects 'IW > 0'.
unsigned PixMap_Image::calculateMatrixColumns(unsigned IW, Image_Type IT) {
	switch (IT) {
	case Image_Type::PBM:
		return IW / 8 + (IW % 8 ? 1 : 0);    // There are padding bits in the last byte
				                             // if 'IW' is not a multiple of 8.
	case Image_Type::PGM:
		return IW;        // 1 byte per pixel

	case Image_Type::PPM:
		return IW * 3;    // 3 bytes per pixel
	}
	return 0;
}

void PixMap_Image::copy(const PixMap_Image& other) {
	pixelValues = allocateMatrix(other.matrixRows, other.matrixCols);    // allocate matrix
	for (unsigned i = 0; i < other.matrixRows * other.matrixCols; ++i)
		pixelValues[0][i] = other.pixelValues[0][i];    // Copy pixels.

	matrixCols = other.matrixCols;
	matrixRows = other.matrixRows;
	imageWidth = other.imageWidth;
	imageHeight = other.imageHeight;
	colourDepth = other.colourDepth;
	imageType = other.imageType;
}
void PixMap_Image::copy(PixMap_Image&& rvalue) {
	pixelValues = rvalue.pixelValues;
	matrixCols = rvalue.matrixCols;
	matrixRows = rvalue.matrixRows;
	imageWidth = rvalue.imageWidth;
	imageHeight = rvalue.imageHeight;
	colourDepth = rvalue.colourDepth;
	imageType = rvalue.imageType;

	rvalue.pixelValues = nullptr;
}
void PixMap_Image::free() {
	deleteMatrix();
}

unsigned char** PixMap_Image::allocateMatrix(unsigned rows, unsigned cols) const {
	unsigned char** matrix = new unsigned char* [rows];	
	matrix[0] = new(std::nothrow) unsigned char[rows * cols];    // Whole matrix allocation.
	if (!matrix) {
		delete[] matrix;
		std::bad_alloc ba;
		throw ba;
	}
	for (unsigned i = 1; i < rows; ++i)
		matrix[i] = matrix[i - 1] + cols;    // Assign the pointers at the start of each row.

	return matrix;
}
void PixMap_Image::deleteMatrix() {
	if (pixelValues)
		delete[] pixelValues[0];

	delete[] pixelValues;
}


//----------------------------------------------------------------------------------------------------------


void read_PGM_PPM_header(std::ifstream&, unsigned& width, unsigned& height, unsigned& coulourDepth);
void read_PBM_header(std::ifstream&, unsigned& width, unsigned& height);
// !!! TO DO: Make support for reading the text variant of the files !!!
std::ifstream& operator>>(std::ifstream& ifile, PixMap_Image& image) {
	if (!ifile.is_open())
		throw "No file is opened!";

	if (ifile.get() != 'P')
		throw "Magic number missing! Cannot read file!";

	// Read Header.
	PixMap_Image::Image_Type imType;
	unsigned imWidth = 0, imHeight = 0, colourDepth = 0;
	switch (ifile.get()) {
	case '4':
		imType = PixMap_Image::Image_Type::PBM;
		read_PBM_header(ifile, imWidth, imHeight);
		break;
	case '5':
		imType = PixMap_Image::Image_Type::PGM;
		read_PGM_PPM_header(ifile, imWidth, imHeight, colourDepth);
		break;
	case '6':
		imType = PixMap_Image::Image_Type::PPM;
		read_PGM_PPM_header(ifile, imWidth, imHeight, colourDepth);
		break;
	default:
		throw "Magic number missing! Cannot read file!";
	}

	ifile.get();    // Go to the byte of the first pixel.
	                // If the file is not opened in 'std::ios::binary' more than one byte may be read.
	                // This is because if the file is not opened in binary and the two bytes are CR and LF
	                // then it is interpreted as a single LF (\n) characted, so 2 bytes may be read.

	PixMap_Image im{imWidth, imHeight, colourDepth, imType};
	ifile.read((char* const) * im.getPixelValues(), im.getMatrixRows() * im.getMatrixCols());    // Read pixels.

	if (ifile.eof()) {
		// If the file is not corrupted and no errors occured, the end of file
		// will not be reached.
		// If this is the last image in a file, reading will stop after the last
		// byte of the file is read.
		throw "Missing image data! File may be corrupted!";
	}
	if (!ifile.good())
		throw "Failure reading image file!";

	image = std::move(im);    // Strong exception safety.

	return ifile;
}
// !!! TO DO: Make support for writing the text variant of the files !!!
// Saves image in binary variant of the files.
std::ofstream& operator<<(std::ofstream& ofile, const PixMap_Image& image) {
	if (!ofile.is_open())
		throw "No file is opened!";

	// Write header.
	switch (image.getImageType())
	{
	case PixMap_Image::Image_Type::PBM:
		ofile << 'P' << '4';
		break;
	case PixMap_Image::Image_Type::PGM:
		ofile << 'P' << '5';
		break;
	case PixMap_Image::Image_Type::PPM:
		ofile << 'P' << '6';
		break;
	}
	ofile << '\r' << image.getImageWidth()
		<< '\r' << image.getImageHeight()
		<< '\r';

	// Only 'PGM' and 'PPM' files have colour depth.
	if (image.getImageType() != PixMap_Image::Image_Type::PBM)
		ofile << image.getColourDepth() << '\r';

	// Write pixel values.
	ofile.write((char*)*image.getPixelValues(), (unsigned long long)image.getMatrixRows() * (unsigned long long)image.getMatrixCols());

	if (!ofile.good())
		throw "Fail to write image to file!";

	return ofile;
}


// Functions for reading PixMap image Header.
bool isDigit(char ch) { return ('0' <= ch && ch <= '9'); }
void skipComment(std::ifstream& file) {
	if (file.peek() == '#')
		while (file.get() != '\n' && file);
}
void skipWhiteSpaces(std::ifstream& file) {
	while (file.peek() != '#' && !isDigit(file.peek()) && file)
		file.get();
}
unsigned extractNumber(std::ifstream& file) {

	while (file) {
		skipComment(file);
		skipWhiteSpaces(file);

		if (isDigit(file.peek())) {
			unsigned a = 0;
			file >> a;
			return a;
		}
	}

	return 0;
}
void read_PGM_PPM_header(std::ifstream& file, unsigned& width, unsigned& height, unsigned& colourDepth) {
	width = extractNumber(file);
	height = extractNumber(file);
	colourDepth = extractNumber(file);
}
void read_PBM_header(std::ifstream& file, unsigned& width, unsigned& height) {
	width = extractNumber(file);
	height = extractNumber(file);
}