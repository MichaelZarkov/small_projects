
#include "MazeSolver.h"

size_t MazeSolver::solveMaze(const SquareMaze_TwoBits& maze) {
	/*  ---------------
		| Explanation |
	    ---------------

		NOTE: | The phrases "perspective of the maze", "perspective of the person" are very repetitive
		      | and should replace them with sth. like "abs. pers.(absolute perspective)" and "rel.pers."
			  | for the sake of readability.

		This is "hug the right wall" algorithm.
		
		                 |---- YES, repeat |1|
		|1| go right ----|                             |---- YES, repeat |1|
		                 |---- NO, go to the front ----|                     |---- YES, repeat |1|
                                                       |---- NO, go left ----|
													                         |---- NO, go back, repeat |1|
		
		- 'YES' means that you occupy the point/pixel (there is no wall).
		- 'NO' means there is a wall in this point/pixel (you cannot occupy it).

		The directions are in the frame of reference of the "little person" solving the maze not the frame of reference of the maze.
		Ex:
		             front
		        ----------------             - the arrow is the direction the "person" is looking.
		    	|              |             - the "person's" front view is in the left direction of the maze.
	      left  |         <--  |  right      - the "person's" right view is in the front direction of the maze and so on...
			    |              |
	            ----------------
			          back

		
	*/

	// Array of the 'go' functions so I can iterate through them.
	bool (*go[4])(const SquareMaze_TwoBits&, size_t&, size_t&)
	{ &goRight, &goFront, &goLeft, &goBack };

	size_t direction = 0;                                         // Looking direction of the "person" from the perspective of the maze.
	size_t row = maze.getStartRow(), col = maze.getStartCol();    // Position of the "person" in the maze.
	size_t loopDetector = 0;
	size_t stepsToSolve = 0;
	do {
		
		maze.setPixel(row, col, 3);    // Colour the pixel the "person" is standing in.

		/*
			Iterate through 'goRight', 'goFront', 'goLeft', 'goBack' in this sequence
			from the perspective of the "PERSON". 
			'goDir' is in the perspective of the MAZE, but the SEQUENCE of function calls
			is in the persp. of the "person" - very important!!!
		*/ 
		size_t goDir = direction;    // The direction the "person" is trying to go from the perspective of the maze!!!
		do {

			if ((*go[goDir])(maze, row, col)){
				direction = (goDir + 3) % 4;    // The person went in some direction.
				break;
			}
			++goDir;
			goDir %= 4;
		} while (goDir != direction);
	
		if (row == maze.getStartRow() && col == maze.getStartCol())    // Did we return to the start.
			++loopDetector;
		if (loopDetector == 2)   // If we return to the start 2 times, the maze is unsolvable.
			return 0;    // Maze is unsolvable.

		++stepsToSolve;
	} while (row != maze.getEndRow() || col != maze.getEndCol());

	return stepsToSolve;    // Maze solved.
}

// 'goFront', 'goBack', 'goLeft', 'goRight' are in the reference frame of the MAZE, NOT the "person".
// The functions require the maze to be walled around, otherwise integer overflow.
bool MazeSolver::goRight(const SquareMaze_TwoBits& maze, size_t& row, size_t& col) {
	if (maze.getPixel(row, col + 1) == 1)
		return false;
	else {
		++col;
		return true;
	}
}
bool MazeSolver::goFront(const SquareMaze_TwoBits& maze, size_t& row, size_t& col) {
	if (maze.getPixel(row - 1, col) == 1)
		return false;    // There is a wall. Can't go there.
	else {
		--row;  // There is no wall. Go there.
		return true;
	}
}
bool MazeSolver::goLeft(const SquareMaze_TwoBits& maze, size_t& row, size_t& col) {
	if (maze.getPixel(row, col - 1) == 1)
		return false;
	else {
		--col;
		return true;
	}
}
bool MazeSolver::goBack(const SquareMaze_TwoBits& maze, size_t& row, size_t& col) {
	if (maze.getPixel(row + 1, col) == 1)    // Is this if statement necessary ? ? ?
		return false;
	else {
		++row;
		return true;
	}
}