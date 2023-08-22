
#include <cstring>
#include <fstream>
#include <new>
#include <string>
#include "Image_Editor_Factory.h"
#include "PixMap_Editor.h"

Image_Editor* Image_Editor_Factory::createEditor(const char* fileName) const {
	if (!fileName)
		throw "Dereferencing nullptr!";

	const char* endOfString = strchr(fileName, '\0');
	if (!strcmp(endOfString - 4, ".pbm") ||
		!strcmp(endOfString - 4, ".pgm") ||
		!strcmp(endOfString - 4, ".ppm"))
	{
		std::ifstream ifile(fileName, std::ios::binary);
		try {
			PixMap_Editor* obj = new PixMap_Editor{ ifile };
			ifile.close();
			return obj;
		}
		catch (std::bad_alloc ba) {
			ifile.close();
			throw ba;
		}
		catch (const char* expt) {
			ifile.close();
			throw expt;
		}
		catch (...) {
			ifile.close();
			throw "Unknown error!";
		}
	}

	throw "File format not supported!";
}