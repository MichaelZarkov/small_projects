
#pragma once

#include <string>
#include "Image_Editor.h"

// Container for the Image Editor, file name and firts transformation to be applied.
class Image_Editor_Info
{
public:
	Image_Editor_Info(const char* fileName, unsigned firstTransformation);
	Image_Editor_Info(std::string& fileName, unsigned firstTransformation);
	Image_Editor_Info(const Image_Editor_Info&);
	Image_Editor_Info(Image_Editor_Info&&) noexcept;
	Image_Editor_Info& operator=(const Image_Editor_Info&);
	Image_Editor_Info& operator=(Image_Editor_Info&&) noexcept;
	~Image_Editor_Info();

	unsigned getFirstTransformation() const;
	const std::string& getFileName() const;
	Image_Editor* getEditor();

	void setFirstTransformation(unsigned newFirst);

private:
	void copy(const Image_Editor_Info&);
	void copy(Image_Editor_Info&&) noexcept;

private:
	std::string fileName;             // The name of the currently loaded file.
	Image_Editor* editor{ nullptr };
	unsigned firstTransformation{ 0 };    // The first (image)transformation that needs to be applied.
                                          // If the image was added at the start this would be 0.
};

