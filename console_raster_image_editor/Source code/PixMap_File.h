
#pragma once
#include "PixMap_Image.h"

// Contains the data from a single PixMap file.
// There may be more than one image in a single file.
class PixMap_File
{
public:
	PixMap_File();
	PixMap_File(std::ifstream&);
	PixMap_File(const PixMap_File&);
	PixMap_File(PixMap_File&&) noexcept;
	PixMap_File& operator=(const PixMap_File&);
	PixMap_File& operator=(PixMap_File&&) noexcept;
	~PixMap_File();

	PixMap_Image& operator[](unsigned index) const;

	unsigned getImageCount() const;

	void addImage(PixMap_Image&&);

	void saveToFile(const char* fileName) const;

private:
	void copy(const PixMap_File&);
	void copy(PixMap_File&&);
	void free();

public:
	PixMap_Image** images;
	unsigned imageCount;
};


std::ifstream& operator>>(std::ifstream&, PixMap_File&);        // Has to be 'std::ios::binary'.
std::ofstream& operator<<(std::ofstream&, const PixMap_File&);