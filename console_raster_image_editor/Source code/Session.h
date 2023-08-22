
#pragma once

#include <vector>
#include "Image_Editor_Info.h"

struct Command_Data;

class Session
{
private:
	enum class Commands {
		UNDEF = -1,

		// Image transformations.
		GRAYSCALE,
		MONOCHROME,
		NEGATIVE,
		ROTATE_LEFT,
		ROTATE_RIGHT,
		FLIP_HORIZONTAL,
		FLIP_VERTICAL,

		// Other.
		UNDO,
		ADD,
		SAVE,
		SESSION_INFO,
		LIST,
		SWITCH,
		LOAD,
		EXIT
	};

public:
	Session(const char* fileName);

	Command_Data consoleInput();

	void applyChangesAndSave();

	unsigned getID() const;

private:
	void save();

	void addImageTransformation(Session::Commands cmd);
	void applyImageTransformations();
	void undo();
	void add(const char*);
	void sessionInfo() const;
	void list() const;

	Session::Commands whichCommand(const char* str) const;

	void clearCin() const;
	
	void currentTime(char*) const;
	char lastDigitToChar(unsigned num) const;

private:
	std::vector<Commands> executedCommands;    // All commands for transforming images the user
	                                           // entered from the beginning of the session.
	std::vector<Image_Editor_Info> images;
	unsigned const sessionID;
	unsigned undoDownTo{ 0 };        // The position after the last transformation command in 'executedCommands'
								     // which was executed and which cannot be undone.

	static const char COMMAND_NAMES[][18];
	static unsigned const COMMAND_COUNT;
	static unsigned NEXT_UNUSED_SESSION_ID;
	
	static unsigned const BUFF_SIZE;
	static const char indentSymbols[];
};

