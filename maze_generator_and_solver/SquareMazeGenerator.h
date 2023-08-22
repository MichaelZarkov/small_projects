
#pragma once
#include "SquareMaze.h"

class SquareMazeGenerator
{
public:

	static void createMaze(const SquareMaze_TwoBits&);    // Console input for adding stuff(mainly walls) to the maze.
	
	static void addDots(const SquareMaze_TwoBits&);                                      // Console input.
	static void addDots(const SquareMaze_TwoBits&, size_t);                              // Maze manipulation logic.
	static void addLines(const SquareMaze_TwoBits&);                                     // Console input.
	static void addVerticalLines(const SquareMaze_TwoBits&, size_t count, size_t len);   // Maze manipulation logic.
	static void addHorizontalLines(const SquareMaze_TwoBits&, size_t count, size_t len); // Maze manipulation logic.

	static void randomMaze(const SquareMaze_TwoBits&);

	static void printCommands();

private:
	static const char COMMANDS[][11];
	static const char NUMBER_OF_COMMANDS;
	static const size_t BUFF_SIZE;

private:
	SquareMazeGenerator();    // Don't allow instances of this class (it only contains static members).
};
