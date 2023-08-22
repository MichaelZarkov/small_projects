
#include <cstdlib>    // atio()
#include <cstring>
#include <iostream>
#include <limits>
#include <utility>
#include "Session_Manager.h"

const char Session_Manager::COMMAND_NAMES[][7]
{
	"> load",
	"> exit"
};
const char Session_Manager::COMMAND_COUNT = 2;

const unsigned Session_Manager::BUFF_SIZE = 512;
const char Session_Manager::indentSymbols[sizeof("  ")] = "  ";


Session_Manager::Session_Manager() {
	consoleInput();
}

void Session_Manager::consoleInput() {

	std::cout << indentSymbols << "Welcome to Simple Image Editor!";
		
	char buff[BUFF_SIZE];
	buff[BUFF_SIZE - 1] = '\0';    // Null at the end of the buffer for redundancy.
	while (true) {
		std::cout << '\n' << indentSymbols << "Type '> load <file_name>' to add and image and start a new session."
			<< '\n' << indentSymbols << "Type '> exit' to close program.\n";

		std::cin.getline(buff, BUFF_SIZE - 1, '\n');     // Null at the end of the buffer will not be overwritten.
		clearCin();

		Commands enteredCommand = whichCommand(buff);
		switch (enteredCommand)
		{
		case Commands::LOAD:
			try {
				manageSessions(buff + 1 + strlen(COMMAND_NAMES[(unsigned)Commands::LOAD]));
				return;
			}
			catch (const char* error) {
				print(error);
			}
			catch (...) {
				printUnknownError();
			}
			break;
		case Commands::EXIT:
			std::cout << indentSymbols << "Closing Simple Image Editor ...";
			return;
		default:
			std::cout << indentSymbols << "Incorrect command! Try again.";
		}
	}
}

// Throws exception if preblem with reading file.
void Session_Manager::manageSessions(const char* fileName) {
	load(fileName);   // Create first session.
	
	unsigned index = 0;
	do {
		Command_Data cmd = sessions[index].consoleInput();

		switch (cmd.cmd)
		{
		case Commands::LOAD:
			try {
				load(cmd.str.c_str());
				++index;
				std::cout << indentSymbols << "Starting new session ...\n";
			}
			catch (const char* error) {
				print(error);
				std::cout << '\n' << indentSymbols << "Returning to the previous session.\n";
			}
			catch (...) {
				printUnknownError();
			}
			break;
		case Commands::EXIT:
			save();
			std::cout << indentSymbols << "Applying changes and saving images ..."
				<< '\n' << indentSymbols << "Closing ...\n";
			return;
		case Commands::SWITCH:
			try {
				unsigned sessionID = atoi(cmd.str.c_str());
				index = findSessionIndex(sessionID);
				std::cout << indentSymbols << "Switching to session with ID " << sessionID << " ...\n";
			}
			catch (const char* error) {
				print(error);
				std::cout << '\n' << indentSymbols << "Returning to the previous session.\n";
			}
		}

	} while (true);
}

void Session_Manager::save() {
	for (unsigned i = 0; i < sessions.size(); ++i)
		sessions[i].applyChangesAndSave();
}
// Throws exception if problem creating session.
void Session_Manager::load(const char* fileName) {
	Session firstSession{ fileName };
	sessions.push_back(std::move(firstSession));
}

void Session_Manager::print(const char* str) const {
	std::cout << indentSymbols << str;
}
void Session_Manager::printUnknownError() const {
	std::cout << indentSymbols << "Unknown error!";
}


// Throws exception if no session with given ID was found.
unsigned Session_Manager::findSessionIndex(unsigned sessionID) const {
	for (unsigned i = 0; i < sessions.size(); ++i)
		if (sessions[i].getID() == sessionID)
			return i;
	
	throw "No session with given ID!";
}

Session_Manager::Commands Session_Manager::whichCommand(const char* str) const {

	int i = 0;
	for (; i < COMMAND_COUNT; ++i) {
		if (!strncmp(COMMAND_NAMES[i], str, strlen(COMMAND_NAMES[i])))
		{
			break;
		}
	}

	switch (i)
	{
	case 0:
		return Commands::LOAD;
	case 1:
		return Commands::EXIT;
	default:
		return Commands::UNDEF;
	}
}

void Session_Manager::clearCin() const {
	if (!std::cin.good()) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max());
	}
}