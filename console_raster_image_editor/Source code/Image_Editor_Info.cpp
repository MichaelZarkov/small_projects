
#include <utility>     // std::move()
#include "Image_Editor_Info.h"
#include "Image_Editor_Factory.h"

Image_Editor_Info::Image_Editor_Info(const char* fileName, unsigned firstTransformation) {
	Image_Editor_Factory f;
	editor = f.createEditor(fileName);
	this->fileName = { fileName };
	this->firstTransformation = firstTransformation;
}
Image_Editor_Info::Image_Editor_Info(std::string& fileName, unsigned firstTransformation) {
	*this = { fileName.c_str(), firstTransformation };
}
Image_Editor_Info::Image_Editor_Info(const Image_Editor_Info& other) {
	copy(other);
}
Image_Editor_Info::Image_Editor_Info(Image_Editor_Info&& rvalue) noexcept {
	copy(std::move(rvalue));
}
Image_Editor_Info& Image_Editor_Info::operator=(const Image_Editor_Info& other) {
	if (this != &other) {
		Image_Editor_Info temp{ other };
		delete editor;
		copy(std::move(temp));
	}
	return *this;
}
Image_Editor_Info& Image_Editor_Info::operator=(Image_Editor_Info&& rvalue) noexcept {
	if (this != &rvalue) {
		delete editor;
		copy(std::move(rvalue));
	}
	return *this;
}
Image_Editor_Info::~Image_Editor_Info() {
	delete editor;
}

unsigned Image_Editor_Info::getFirstTransformation() const { return firstTransformation; }
const std::string& Image_Editor_Info::getFileName() const { return fileName; }
Image_Editor* Image_Editor_Info::getEditor() { return editor; }

void Image_Editor_Info::setFirstTransformation(unsigned newFirst) {
	firstTransformation = newFirst;
}

void Image_Editor_Info::copy(const Image_Editor_Info& other) {
	fileName = other.fileName;
	editor = other.editor->clone();
	firstTransformation = other.firstTransformation;
}
void Image_Editor_Info::copy(Image_Editor_Info&& rvalue) noexcept {
	fileName = rvalue.fileName;
	editor = rvalue.editor;
	firstTransformation = rvalue.firstTransformation;

	rvalue.fileName.clear();
	rvalue.editor = nullptr;
	rvalue.firstTransformation = 0;
}
