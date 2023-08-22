
#pragma once

#include <vector>
#include <string>
#include "Session.h"

// Allows for multiple sessions at the same time.
class Session_Manager
{
public:
	enum class Commands {
		UNDEF = -1,
		LOAD,
		EXIT,
		SWITCH
	};

public:
	Session_Manager();

	void consoleInput();

private:

	void manageSessions(const char* fileName);

	void save();
	void load(const char* fileName);    // Add session with one starting file.

	void print(const char* str) const;
	void printUnknownError() const;

	unsigned findSessionIndex(unsigned sessionID) const;

	Commands whichCommand(const char* str) const;

	void clearCin() const;

private:
	std::vector<Session> sessions;

	static const char COMMAND_NAMES[][7];
	static const char COMMAND_COUNT;

	static const unsigned BUFF_SIZE;
	static const char indentSymbols[];
};

struct Command_Data {
	std::string str;
	Session_Manager::Commands cmd;
};