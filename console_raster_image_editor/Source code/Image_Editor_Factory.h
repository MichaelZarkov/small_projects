#pragma once
#include "Image_Editor.h"

class Image_Editor_Factory
{
public:
	Image_Editor* createEditor(const char* fileName) const;
};
