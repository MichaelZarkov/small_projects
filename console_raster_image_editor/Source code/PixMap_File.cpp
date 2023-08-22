
#pragma warning (disable : 4996)   // For 'strcpy()'

#include <cstring>
#include <exception>
#include <fstream>
#include <new>            // 'std::nothrow'
#include <utility>        // 'std::move()'
#include "PixMap_File.h"


PixMap_File::PixMap_File()
	: images(nullptr), imageCount(0)
{}
PixMap_File::PixMap_File(std::ifstream& ifile)
	: images(nullptr), imageCount(0)
{
	operator>>(ifile, *this);
}
PixMap_File::PixMap_File(const PixMap_File& other) {
	copy(other);
}
PixMap_File::PixMap_File(PixMap_File&& rvalue) noexcept {
	copy(std::move(rvalue));
}
PixMap_File& PixMap_File::operator=(const PixMap_File& other) {
	if (this != &other) {
		PixMap_File temp{ other };     // Strong exception safety.
		operator=(std::move(temp));    // Move 'temp' in '*this'.
	}
	return *this;
}
PixMap_File& PixMap_File::operator=(PixMap_File&& rvalue) noexcept {
	if (this != &rvalue) {
		free();
		copy(std::move(rvalue));
	}
	return *this;
}
PixMap_File::~PixMap_File() {
	free();
}

PixMap_Image& PixMap_File::operator[](unsigned index) const {
	if (index >= imageCount)
		throw "Out of bounds in 'PixMap_File'!";
	return *images[index];
}

unsigned PixMap_File::getImageCount() const { return imageCount; }

void PixMap_File::addImage(PixMap_Image&& rvalImage) {
	PixMap_Image** newImages = new PixMap_Image * [imageCount + 1];      // Make room for one more image.
	newImages[imageCount] = new(std::nothrow) PixMap_Image{ std::move(rvalImage) };    // Move 'rvalImage' in last place.
	if (!newImages[imageCount]) {
		delete[] newImages;
		std::bad_alloc ba;
		throw ba;
	}
	for (unsigned i = 0; i < imageCount; ++i)
		newImages[i] = images[i];
	
	delete[] images;
	images = newImages;
	++imageCount;
}

// Throws exception if error writing to file.
void PixMap_File::saveToFile(const char* fileName) const {
	if (!fileName)
		throw "Given file name is nullptr!";

	char nameAndExtension[512];
	strcpy(nameAndExtension, fileName);
	switch (images[0]->getImageType())
	{
	case PixMap_Image::Image_Type::PBM:
		strcat(nameAndExtension, ".pbm");
		break;
	case PixMap_Image::Image_Type::PGM:
		strcat(nameAndExtension, ".pgm");
		break;
	case PixMap_Image::Image_Type::PPM:
		strcat(nameAndExtension, ".ppm");
		break;
	}

	std::ofstream ofile(nameAndExtension, std::ios::binary);
	if (!ofile.is_open())
		throw "Cannot open file!";

	try {
		operator<<(ofile, *this);
		ofile.close();
	}
	catch (const char* error) {
		ofile.close();
		throw error;
	}
	catch (...) {
		ofile.close();
		throw "Error writing to Pix Map file!";
	}
}

void PixMap_File::copy(const PixMap_File& other) {
	images = new PixMap_Image * [other.imageCount];
	
	for (unsigned i = 0; i < other.imageCount; ++i)
	{
		images[i] = new(std::nothrow) PixMap_Image{ *other.images[i] };
		
		if (!images[i]) {    // Check if memory alloc has failed.
			for (unsigned k = 0; k < i; ++k) {
				delete images[k];
			}
			delete[] images;

			std::bad_alloc ba;
			throw ba;
		}
	}

	imageCount = other.imageCount;
}
void PixMap_File::copy(PixMap_File&& rvalue) {
	images = rvalue.images;
	imageCount = rvalue.imageCount;

	rvalue.images = nullptr;
	rvalue.imageCount = 0;
}
void PixMap_File::free() {
	if (images) {
		for (unsigned i = 0; i < imageCount; ++i)
			delete images[i];
	}
	delete[] images;
}


std::ifstream& operator>>(std::ifstream& ifile, PixMap_File& pmf) {
	if (!ifile.is_open())
		throw "No file is opened!";

	PixMap_File new_pmf;
	do {
		new_pmf.addImage({ ifile });    // Read image from file and add it.

	} while (ifile.peek() != EOF && ifile.good());    // 'peek()' sets the eof flag if it reads(peeks) past the end of file.

	if (ifile.eof())
		ifile.clear();
	else
		throw "Failure reading from image file!";

	pmf = std::move(new_pmf);

	return ifile;
}
std::ofstream& operator<<(std::ofstream& ofile, const PixMap_File& pmf) {
	if (!ofile.is_open())
		throw "No file is opened!";

	for (unsigned i = 0; i < pmf.getImageCount(); ++i)
		ofile << pmf[i];

	return ofile;
}