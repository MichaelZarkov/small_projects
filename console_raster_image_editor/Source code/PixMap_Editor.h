
#pragma once
#include "PixMap_File.h"
#include "Image_Editor.h"

// Works with PGM and PPM images with colour depth < 256 (singe byte colour).
class PixMap_Editor : public Image_Editor
{
public:
	PixMap_Editor(std::ifstream&);

	virtual Image_Editor* clone() const override;

	virtual void saveToFile(const char* fileName) override;

	const PixMap_File& getImageFile() const;

	// Image editing functions.
	virtual void convertToGrayscale() override;
	virtual void convertToMonochrome() override;
	virtual void negativeOfImage() override;
	virtual void rotateLeft() override;
	virtual void rotateRight() override;
	virtual void flipHorizontal() override;
	virtual void flipVertical() override;

private:
	void negativeOf_PBM(PixMap_Image&);
	void negativeOf_PGM_PPM(PixMap_Image&);
	
	void rotLeft_PBM(PixMap_Image&);
	void rotLeft_PGM(PixMap_Image&);
	void rotLeft_PPM(PixMap_Image&);
	
	void flipHor_PBM(PixMap_Image&);
	void flipHor_PGM(PixMap_Image&);
	void flipHor_PPM(PixMap_Image&);

	void flipVert_PBM_PGM_PPM(PixMap_Image&);

	// Maybe 'getPixel_PBM(...)' and 'setPixel_PBM(...)' should be implemented in the class 'PixMap_Image'.
	unsigned char getPixel_PBM(const PixMap_Image&, unsigned row, unsigned col) const;
	void setPixel_PBM(PixMap_Image&, unsigned row, unsigned col, unsigned val);
	void swap(unsigned char&, unsigned char&) const;

private:
	PixMap_File imageFile;

};

std::ifstream& operator>>(std::ifstream&, PixMap_Editor&);
std::ofstream& operator<<(std::ofstream&, const PixMap_Editor&);