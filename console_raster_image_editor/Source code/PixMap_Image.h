
#pragma once

// Contains the data from a single PixMap image.
// Supports 8-bit colour. Does not support 16-bit colour.
class PixMap_Image
{
public:
	enum class Image_Type {
		PBM,
		PGM,
		PPM
	};

public:
	PixMap_Image(unsigned IW, unsigned IH, unsigned CD, Image_Type IT);
	PixMap_Image(std::ifstream&);
	PixMap_Image(const PixMap_Image&);
	PixMap_Image(PixMap_Image&&) noexcept;
	PixMap_Image& operator=(const PixMap_Image&);
	PixMap_Image& operator=(PixMap_Image&&) noexcept;
	~PixMap_Image();

	unsigned char* const* getPixelValues();    // Return type: pointer to const pointer to unsigned char.
	unsigned char const* const* getPixelValues() const;
	unsigned getMatrixCols() const;
	unsigned getMatrixRows() const;
	unsigned getImageWidth() const;
	unsigned getImageHeight() const;
	unsigned getColourDepth() const;
	Image_Type getImageType() const;

private:
	unsigned calculateMatrixColumns(unsigned IW, Image_Type IT);

	void copy(const PixMap_Image&);
	void copy(PixMap_Image&&);
	void free();

	unsigned char** allocateMatrix(unsigned rows, unsigned cols) const;
	void deleteMatrix();

private:
	static const unsigned long long MAX_IMAGE_SIZE;    // In bytes.

	unsigned char** pixelValues;
	unsigned matrixCols;
	unsigned matrixRows;
	unsigned imageWidth;
	unsigned imageHeight;
	unsigned colourDepth;
	Image_Type imageType;
};

std::ifstream& operator>>(std::ifstream&, PixMap_Image&);          // File has to be opened in 'std::ios::binary'.
std::ofstream& operator<<(std::ofstream&, const PixMap_Image&);    // Saves as binary variants of the files only.