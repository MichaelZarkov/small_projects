
#pragma once
#include "SquareMaze.h"

class MazeSolver
{
public:

	static size_t solveMaze(const SquareMaze_TwoBits&);    // Returns true if the maze is solvable.

private:

	static bool goRight(const SquareMaze_TwoBits&, size_t&, size_t&);
	static bool goFront(const SquareMaze_TwoBits&, size_t&, size_t&);
	static bool goLeft(const SquareMaze_TwoBits&, size_t&, size_t&);
	static bool goBack(const SquareMaze_TwoBits&, size_t&, size_t&);

private:
	MazeSolver();    // Private constructor because this class doesn't make sense to be
	                 // instantiated (it only contains static functions).
	                 // Maybe it should be a nemespace and not a class then.
};

