
#pragma once

// Abstract base class for image editors used in 'Editing_Session'.
class Image_Editor
{
public:

	virtual Image_Editor* clone() const = 0;

	virtual void saveToFile(const char*) = 0;

	// Image editing functions.
	virtual void convertToGrayscale() = 0;
	virtual void convertToMonochrome() = 0;
	virtual void negativeOfImage() = 0;
	virtual void rotateLeft() = 0;
	virtual void rotateRight() = 0;
	virtual void flipHorizontal() = 0;
	virtual void flipVertical() = 0;

public:
	virtual ~Image_Editor() = default;    // Virtual destructor because I will delete polymorphically.
};