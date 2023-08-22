
#include <fstream>
#include <utility>
#include "PixMap_Editor.h"

PixMap_Editor::PixMap_Editor(std::ifstream& ifile)
	: imageFile(ifile)
{}

Image_Editor* PixMap_Editor::clone() const {
	return new PixMap_Editor{ *this };
}

// Throws exception if error while writing to file.
void PixMap_Editor::saveToFile(const char* fileName) {
	if (!fileName)
		throw "Given file name is nullptr!";
	
	imageFile.saveToFile(fileName);
}

const PixMap_File& PixMap_Editor::getImageFile() const { return imageFile; }

void PixMap_Editor::convertToGrayscale() {
	// Loop through all the images in the file.
	for (unsigned i = 0; i < imageFile.getImageCount(); ++i) {

		if ((imageFile[i]).getImageType() == PixMap_Image::Image_Type::PPM) {    // Convert to grayscale only PPM images.

			// Go through every pixel and make it grayscale (every pixel is 3 bytes).
			for (unsigned row = 0; row < (imageFile[i]).getMatrixRows(); ++row) {
				for (unsigned col = 0; col < (imageFile[i]).getMatrixCols(); col += 3)    // This can be made faster for sure.
				{
					unsigned char* const* pixels = (imageFile[i]).getPixelValues();    // Pixel matrix. This is just for readability.
					
					// Take the average brightnes of Red, Green and Blue
					// and assign that brightnes to each colour.
					unsigned average  = pixels[row][col];
					         average += pixels[row][col + 1];
					         average += pixels[row][col + 2];
							 average /= 3;

					pixels[row][col]     = average;
					pixels[row][col + 1] = average;
					pixels[row][col + 2] = average;
				}
			}
		}
	}
}
void PixMap_Editor::convertToMonochrome() {

	for (unsigned i = 0; i < imageFile.getImageCount(); ++i) {

		// Convert to monochrome only PGM and PPM images.
		if ((imageFile[i]).getImageType() == PixMap_Image::Image_Type::PGM) {

			// Go through all pixels (every pixel is 1 byte).
			for (unsigned row = 0; row < (imageFile[i]).getMatrixRows(); ++row) {
				for (unsigned col = 0; col < (imageFile[i]).getMatrixCols(); ++col) {

					unsigned char* const* pixels = (imageFile[i]).getPixelValues();    // Pixel matrix.

					// If the pixel is at less than 50% brightness, make it 0%.
					// If the pixel is at more than 50% brightness, make it 100%.
					pixels[row][col] = (pixels[row][col] < (imageFile[i]).getColourDepth() / 2) ? 0 : (imageFile[i]).getColourDepth();

					/*
						// Equivalent to the above line:

						if (pixels[row][col] < (imageFile[i]).getColourDepth() / 2) {
							pixels[row][col] = 0;
						}
						else {
							pixels[row][col] = (imageFile[i]).getColourDepth();
						}
					*/
				}
			}
		}
		else if ((imageFile[i]).getImageType() == PixMap_Image::Image_Type::PPM) {

			// Go through all pixels (every pixel is 3 bytes).
			for (unsigned row = 0; row < (imageFile[i]).getMatrixRows(); ++row) {
				for (unsigned col = 0; col < (imageFile[i]).getMatrixCols(); col += 3) {
					
					unsigned char* const* pixels = (imageFile[i]).getPixelValues();    // Pixel matrix. This is just for readability.

					// Take the average brightnes of Red, Green and Blue.
					unsigned average  = pixels[row][col];
					         average += pixels[row][col + 1];
					         average += pixels[row][col + 2];
					         average /= 3;

					if (average < (imageFile[i]).getColourDepth() / 2) {    // < 50% brightnes
						pixels[row][col]     = 0;
						pixels[row][col + 1] = 0;
						pixels[row][col + 2] = 0;
					}
					else {    // >= 50% brightness
						pixels[row][col]     = (imageFile[i]).getColourDepth();
						pixels[row][col + 1] = (imageFile[i]).getColourDepth();
						pixels[row][col + 2] = (imageFile[i]).getColourDepth();
					}
				}
			}

		}
	}
}
void PixMap_Editor::negativeOfImage() {

	for (unsigned i = 0; i < imageFile.getImageCount(); ++i) {

		if ((imageFile[i]).getImageType() == PixMap_Image::Image_Type::PBM) {
			negativeOf_PBM(imageFile[i]);
		}
		else {
			negativeOf_PGM_PPM(imageFile[i]);
		}
	}
}
void PixMap_Editor::rotateLeft() {
	for (unsigned i = 0; i < imageFile.getImageCount(); ++i) {
		switch ((imageFile[i]).getImageType()) {
		case PixMap_Image::Image_Type::PBM:
			rotLeft_PBM(imageFile[i]);
			break;
		case PixMap_Image::Image_Type::PGM:
			rotLeft_PGM(imageFile[i]);
			break;
		case PixMap_Image::Image_Type::PPM:
			rotLeft_PPM(imageFile[i]);
			break;
		}
	}
}
void PixMap_Editor::rotateRight() {
	// !!! This is a temporary solution !!!
	// !!! Have to write separate functions for rotaring right.
	for (unsigned i = 0; i < imageFile.getImageCount(); ++i) {
		switch ((imageFile[i]).getImageType()) {
		case PixMap_Image::Image_Type::PBM:
			rotLeft_PBM(imageFile[i]);
			rotLeft_PBM(imageFile[i]);
			rotLeft_PBM(imageFile[i]);
			break;
		case PixMap_Image::Image_Type::PGM:
			rotLeft_PGM(imageFile[i]);
			rotLeft_PGM(imageFile[i]);
			rotLeft_PGM(imageFile[i]);
			break;
		case PixMap_Image::Image_Type::PPM:
			rotLeft_PPM(imageFile[i]);
			rotLeft_PPM(imageFile[i]);
			rotLeft_PPM(imageFile[i]);
			break;
		}
	}
}
void PixMap_Editor::flipHorizontal() {

	// !!! The 'flipHor' functions can be made better !!!

	for (unsigned i = 0; i < imageFile.getImageCount(); ++i) {

		switch ((imageFile[i].getImageType())) {
		case PixMap_Image::Image_Type::PBM:
			flipHor_PBM(imageFile[i]);
			break;
		case PixMap_Image::Image_Type::PGM:
			flipHor_PGM(imageFile[i]);
			break;
		case PixMap_Image::Image_Type::PPM:
			flipHor_PPM(imageFile[i]);
			break;
		}
	}
}
void PixMap_Editor::flipVertical() {
	for (unsigned i = 0; i < imageFile.getImageCount(); ++i) {
		flipVert_PBM_PGM_PPM(imageFile[i]);
	}
}

void PixMap_Editor::negativeOf_PBM(PixMap_Image& im) {

	// Just do a bitwise NOT on every pixel so white becomes black and black becomes white.
	// Single for loop goes through the whole image because the pixel matrix is allocated in a 
	// single block of memory (not row by row).
	for (unsigned k = 0; k < im.getMatrixRows() * im.getMatrixCols(); ++k)
		im.getPixelValues()[0][k] = ~im.getPixelValues()[0][k];
}
void PixMap_Editor::negativeOf_PGM_PPM(PixMap_Image& im) {

	// Single for loop goes through every pixel because the pixel matrix is allocated in a 
	// single block of memory (not row by row).
	for (unsigned k = 0; k < im.getMatrixRows() * im.getMatrixCols(); ++k) {
		// Subtract the current pixel value from the max pixel value(colour depth).
		im.getPixelValues()[0][k] = im.getColourDepth() - im.getPixelValues()[0][k];
	}
}

void PixMap_Editor::rotLeft_PBM(PixMap_Image& im) {
	PixMap_Image new_im{ im.getImageHeight(), im.getImageWidth(), im.getColourDepth(), im.getImageType() };

	for (unsigned row = 0; row < im.getImageHeight(); ++row) {
		for (unsigned col = 0; col < im.getImageWidth(); ++col) {
			unsigned value = getPixel_PBM(im, row, col);
			setPixel_PBM(new_im, new_im.getImageHeight() - col - 1, row, value);
		}
	}

	im = std::move(new_im);
}
void PixMap_Editor::rotLeft_PGM(PixMap_Image& im) {
	PixMap_Image new_im{ im.getImageHeight(), im.getImageWidth(), im.getColourDepth(), im.getImageType() };

	for (unsigned row = 0; row < im.getMatrixRows(); ++row) {
		for (unsigned col = 0; col < im.getMatrixCols(); ++col) {
			new_im.getPixelValues()[new_im.getMatrixRows() - col - 1][row] = im.getPixelValues()[row][col];
		}
	}

	im = std::move(new_im);
}
void PixMap_Editor::rotLeft_PPM(PixMap_Image& im) {
	PixMap_Image new_im{ im.getImageHeight(), im.getImageWidth(), im.getColourDepth(), im.getImageType() };

	for (unsigned row = 0; row < im.getImageHeight(); ++row) {
		for (unsigned col = 0; col < im.getImageWidth(); ++col) {
			new_im.getPixelValues()[new_im.getImageHeight() - col - 1][row * 3]     = im.getPixelValues()[row][col * 3];
			new_im.getPixelValues()[new_im.getImageHeight() - col - 1][row * 3 + 1] = im.getPixelValues()[row][col * 3 + 1];
			new_im.getPixelValues()[new_im.getImageHeight() - col - 1][row * 3 + 2] = im.getPixelValues()[row][col * 3 + 2];
		}
	}

	im = std::move(new_im);
}

void PixMap_Editor::flipHor_PBM(PixMap_Image& im) {

	for (unsigned col = 0; col < im.getMatrixCols() / 2; ++col) {
		
		// Swaps the i-th col with the (N - i)-th col (N = number of colums - 1; 0 < i < N / 2).
		for (unsigned row = 0; row < im.getMatrixRows(); ++row) {

			unsigned char temp = getPixel_PBM(im, row, col);
			setPixel_PBM(im, row, col, getPixel_PBM(im, row, im.getMatrixCols() - col - 1));
			setPixel_PBM(im, row, im.getMatrixCols() - col - 1, temp);
		}
	}
}
void PixMap_Editor::flipHor_PGM(PixMap_Image& im) {
	for (unsigned col = 0; col < im.getImageWidth() / 2; ++col) {
		for (unsigned row = 0; row < im.getImageHeight(); ++row) {
			swap(im.getPixelValues()[row][col], im.getPixelValues()[row][im.getMatrixCols() - col - 1]);
		}
	}
}
void PixMap_Editor::flipHor_PPM(PixMap_Image& im) {
	for (unsigned col = 0; col < im.getMatrixCols() / 2; col += 3) {
		for (unsigned row = 0; row < im.getMatrixRows(); ++row) {
			swap(im.getPixelValues()[row][col], im.getPixelValues()[row][im.getMatrixCols() - col - 1]);
			swap(im.getPixelValues()[row][col + 1], im.getPixelValues()[row][im.getMatrixCols() - col - 2]);
			swap(im.getPixelValues()[row][col + 2], im.getPixelValues()[row][im.getMatrixCols() - col - 3]);
		}
	}
}

void PixMap_Editor::flipVert_PBM_PGM_PPM(PixMap_Image& im) {
	for (unsigned row = 0; row < im.getMatrixRows() / 2; ++row) {
		for (unsigned col = 0; col < im.getMatrixCols(); ++col) {
			swap(im.getPixelValues()[row][col], im.getPixelValues()[im.getMatrixRows() - row - 1][col]);
		}
	}
}

// Extracts a single pixel from a PBM image.
// Returns 0 or 1.
unsigned char PixMap_Editor::getPixel_PBM(const PixMap_Image& im, unsigned row, unsigned col) const {
	// Extracts the bit in the given position using bit shifts.
	// If the bit is 0, returns 0.
	// If the bit is 1, returns 1.
	// unsigned char cast because when doing bitwise operations the compiler
	// casts it to a 4 byte value (don't know how else to deal with that).
	return (unsigned char)(im.getPixelValues()[row][col / 8] << col % 8) >> 7;
}
// Sets a single pixel in PBM image.
// 'val' has to be 0 or 1.
void PixMap_Editor::setPixel_PBM(PixMap_Image& im, unsigned row, unsigned col, unsigned val) {

	// 'pos'  - position of the pixel in the byte. 7 is most significant, 0 is least significant.
	// 'mask' - a byte with 1 at the position of the pixel and 0 at every other position.
	// 'val'  - either 00000001 or 00000000.
	// 
	// 'im.getPixelValues()[row][col / 8]' - the byte the pixel is in.
	//
	// 'im.getPixelValues()[row][col / 8] & ~mask' - sets the bit of the pixel to 0.

	unsigned pos = 7 - (col % 8);
	unsigned mask = 1 << pos;
	im.getPixelValues()[row][col / 8] = (im.getPixelValues()[row][col / 8] & ~mask) | (val << pos);
}
void PixMap_Editor::swap(unsigned char& c1, unsigned char& c2) const {
	unsigned char c = c1;
	c1 = c2;
	c2 = c;
}

std::ifstream& operator>>(std::ifstream& ifile, PixMap_Editor& obj) {
	obj = { ifile };    // Have to test if this calls the move operator= of 'PixMap_Editor'.

	return ifile;
}
std::ofstream& operator<<(std::ofstream& ofile, const PixMap_Editor& obj) {
	ofile << obj.getImageFile();

	return ofile;
}