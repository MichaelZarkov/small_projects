
#pragma warning (disable : 4996)   // For 'strcpy()'

#include <cstring>
#include <ctime>
#include <iostream>
#include <utility>
#include "Session.h"
#include "Session_Manager.h"


const char Session::COMMAND_NAMES[][18]
{
	"> grayscale",
	"> monochrome",
	"> negative",
	"> rotate left",
	"> rotate right",
	"> flip horizontal",
	"> flip vertical",

	"> undo",
	"> add",
	"> save",
	"> session info",
	"> list",
	"> switch",
	"> load",
	"> exit"
};
unsigned const Session::COMMAND_COUNT = 15;
unsigned Session::NEXT_UNUSED_SESSION_ID = 1;

unsigned const Session::BUFF_SIZE = 512;
const char Session::indentSymbols[sizeof("  ")] = "  ";


// Adds initial image from file.
// Throws exception if problem reading from file.
Session::Session(const char* fileName) : sessionID(NEXT_UNUSED_SESSION_ID) {
	add(fileName);
	++NEXT_UNUSED_SESSION_ID;
}

Command_Data Session::consoleInput() {
	
	std::cout << '\n' << indentSymbols << "Session with ID = " << sessionID << " started.";
	std::cout << '\n' << indentSymbols << "Type '> list' to see all available commands.";
	std::cout << '\n' << indentSymbols << "Type a command:\n";

	char buff[BUFF_SIZE]; 
	buff[BUFF_SIZE - 1] = '\0';    // Null at the end of the buffer for redundancy.
	while (true) {
		
		std::cin.getline(buff, BUFF_SIZE - 1, '\n');     // Null at the end of the buffer will not be overwritten.
		clearCin();

		Commands enteredCommand = whichCommand(buff);
		switch (enteredCommand)
		{
		case Session::Commands::GRAYSCALE:
		case Session::Commands::MONOCHROME:
		case Session::Commands::NEGATIVE:
		case Session::Commands::ROTATE_LEFT:
		case Session::Commands::ROTATE_RIGHT:
		case Session::Commands::FLIP_HORIZONTAL:
		case Session::Commands::FLIP_VERTICAL:
			addImageTransformation(enteredCommand);
			std::cout << indentSymbols << "Transformation applied.";
			break;

		case Session::Commands::UNDO:
			try {
				undo();
				std::cout << indentSymbols << "Transformation undone.";
			}
			catch (const char* error) {
				std::cout << indentSymbols << error;
			}
			break;
		case Session::Commands::ADD:
			try {
				add(buff + 1 + strlen(COMMAND_NAMES[(unsigned)Commands::ADD]));
				std::cout << indentSymbols << "Image added.";
			}
			catch (const char* error) {
				std::cout << indentSymbols << "Image was not added! " << error;
			}
			catch (...) {
				std::cout << indentSymbols << "Image was not added! Unknown error!";
			}
			break;
		case Session::Commands::SAVE:
			try {
				applyChangesAndSave();
			}
			catch (const char* error) {
				std::cout << indentSymbols << error << " Images may not be saved!";
				break;
			}
			catch (...) {
				std::cout << indentSymbols << "Error has occured! Images may not be saved!";
				break;
			}
			std::cout << indentSymbols << "Images saved";
			break;
		case Session::Commands::SESSION_INFO:
			sessionInfo();
			break;
		case Session::Commands::LIST:
			list();
			break;
		case Session::Commands::SWITCH:
			// Return the ID of the new session and the command to Session Manager.
			return { buff + 1 + strlen(COMMAND_NAMES[(unsigned)Session::Commands::SWITCH]),
					 Session_Manager::Commands::SWITCH };
		case Session::Commands::LOAD:
			// Return the file name and command.
			return { buff + 1 + strlen(COMMAND_NAMES[(unsigned)Session::Commands::LOAD]),
					 Session_Manager::Commands::LOAD };
		case Session::Commands::EXIT:
			return { "EXIT", Session_Manager::Commands::EXIT};
		default:
			std::cout << indentSymbols << "Wrong command! Try again.";
		}

		std::cout << "\n\n" << indentSymbols << "Type a command:\n";
	}
}

void Session::applyChangesAndSave() {
	applyImageTransformations();
	save();
}

unsigned Session::getID() const { return sessionID; }

void Session::save() {
	// Go through all images and save them with timestamp in the name.
	for (unsigned i = 0; i < images.size(); ++i) {
		char newName[BUFF_SIZE];
		strcpy(newName, images[i].getFileName().c_str());

		char* startOfFileExtension = newName + images[i].getFileName().size();
		while (*startOfFileExtension != '.')    // find the '.' for the file extencion
			--startOfFileExtension;

		currentTime(startOfFileExtension);     // Add timestamp.

		images[i].getEditor()->saveToFile(newName);
	}
}
void Session::addImageTransformation(Session::Commands cmd) {
	executedCommands.push_back(cmd);
}
void Session::applyImageTransformations() {
	
	for (unsigned i = 0; i < images.size(); ++i) {
		unsigned k = undoDownTo < images[i].getFirstTransformation() ? images[i].getFirstTransformation() : undoDownTo;
		for (;k < executedCommands.size(); ++k) {
			switch (executedCommands[k])
			{
			case Session::Commands::GRAYSCALE:
				images[i].getEditor()->convertToGrayscale();
				break;
			case Session::Commands::MONOCHROME:
				images[i].getEditor()->convertToMonochrome();
				break;
			case Session::Commands::NEGATIVE:
				images[i].getEditor()->negativeOfImage();
				break;
			case Session::Commands::ROTATE_LEFT:
				images[i].getEditor()->rotateLeft();
				break;
			case Session::Commands::ROTATE_RIGHT:
				images[i].getEditor()->rotateRight();
				break;
			case Session::Commands::FLIP_HORIZONTAL:
				images[i].getEditor()->flipHorizontal();
				break;
			case Session::Commands::FLIP_VERTICAL:
				images[i].getEditor()->flipVertical();
				break;
			}
		}
	}
	undoDownTo = executedCommands.size();    // Update the commands which cannot be undone.
}
// Undo the last command.
// If the changes are already applied with '> save', throws an exception.
void Session::undo() {
	if (executedCommands.size() && executedCommands.size() > undoDownTo) {
		executedCommands.pop_back();
		return;
	}
	else {
		throw "Cannot undo the last commands because the changes are already applied!";
	}
}
// Add image from file.
// Throw exception if problem occurs while reading or the file does not exist.
void Session::add(const char* fileName) {

	Image_Editor_Info newImage{ fileName, (unsigned)executedCommands.size() };
	images.push_back(std::move(newImage));

	// Not writen line this:
	//    images.push_back({ fileName, executedCommands.size() })
	// Because if there is a problem with the file, no images will be added to the vector.
}
void Session::sessionInfo() const {
	std::cout << indentSymbols << "Session info:";
	std::cout << "\n\t" << indentSymbols << "Session ID: " << sessionID;
	
	std::cout << "\n\t" << indentSymbols << "Files in the session:";
	for (unsigned i = 0; i < images.size(); ++i)
		std::cout << "\n\t\t" << i + 1 << ". " << images[i].getFileName();

	std::cout << "\n\t" << indentSymbols << "Applied transformations:";
	for (unsigned i = 0; i < executedCommands.size(); ++i)
		std::cout << "\n\t\t" << i+1 << ". " << COMMAND_NAMES[(size_t)executedCommands[i]];
	if (executedCommands.size() == 0)
		std::cout << " no transformations applied.";
}
// Prints all available commands to the console and what they do.
void Session::list() const {
	std::cout << indentSymbols << "Available commands:";
	std::cout << "\n\t" << indentSymbols << "Image editing commands:";
	for (int i = 0; i < 7; ++i)
		std::cout << "\n\t\t" << i+1 << ". " << COMMAND_NAMES[i];

	std::cout << "\n\t" << indentSymbols << "Other commands:";
	std::cout << "\n\t\t> undo - undo last image transformation in the current session."
		<< "\n\t\t> add <file_name> - add image from file."
		<< "\n\t\t> save - save changes to all images from current session."
		<< "\n\t\t> session info"
		<< "\n\t\t> list - print available commands."
		<< "\n\t\t> switch <session_ID> - switch to another session."
		<< "\n\t\t> load <file_name> - start a new session and add the given file."
		<< "\n\t\t> exit - exit the program and save all images in all sessions.";
}

// If the given string does not match any commands, returns UNDEF.
Session::Commands Session::whichCommand(const char* str) const {

	// Go through all 'COMMAND_NAMES' and find the matching one.
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
		return Session::Commands::GRAYSCALE;
	case 1:
		return Session::Commands::MONOCHROME;
	case 2:
		return Session::Commands::NEGATIVE;
	case 3:
		return Session::Commands::ROTATE_LEFT;
	case 4:
		return Session::Commands::ROTATE_RIGHT;
	case 5:
		return Session::Commands::FLIP_HORIZONTAL;
	case 6:
		return Session::Commands::FLIP_VERTICAL;
	case 7:
		return Session::Commands::UNDO;
	case 8:
		return Session::Commands::ADD;
	case 9:
		return Session::Commands::SAVE;
	case 10:
		return Session::Commands::SESSION_INFO;
	case 11:
		return Session::Commands::LIST;
	case 12:
		return Session::Commands::SWITCH;
	case 13:
		return Session::Commands::LOAD;
	case 14:
		return Session::Commands::EXIT;
	default:
		return Session::Commands::UNDEF;
	}
}

void Session::clearCin() const {
	if (!std::cin.good()) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max());
	}
}

// Writes current time to string in format '_HH_MM_SS'
// Puts terminating null char at the end.
void Session::currentTime(char* t) const {
	// '_HH'
	t[0] = '_';
	unsigned hour = ((time(0) / 3600) + 3) % 24;
	t[2] = lastDigitToChar(hour);
	hour /= 10;
	t[1] = lastDigitToChar(hour);

	// '_MM'
	t[3] = '_';
	unsigned min = (time(0) / 60) % 60;
	t[5] = lastDigitToChar(min);
	min /= 10;
	t[4] = lastDigitToChar(min);

	// '_SS'
	t[6] = '_';
	unsigned sec = time(0) % 60;
	t[8] = lastDigitToChar(sec);
	sec /= 10;
	t[7] = lastDigitToChar(sec);
	t[9] = '\0';
}
char Session::lastDigitToChar(unsigned num) const { return '0' + num % 10; }