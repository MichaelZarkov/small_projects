
#include <iostream>
#include "MazeSolver.h"
#include "SquareMazeGenerator.h"


// Generates and solves a bunch of mazes and saves to file
// the one that the 'MazeSolver' took the most steps to solve.
void findInterestingMaze() {
	size_t sidelengthOfMaze = 500;
	SquareMaze_TwoBits maze{ sidelengthOfMaze };
	
	size_t mostSteps = 0;
	size_t howMany = 2000;    // How many random mazes to generate and solve.
	for (size_t i = 0; i < howMany; ++i) {
		// Reset the maze.
		maze.fillWith(0);
		maze.border();

		SquareMazeGenerator::randomMaze(maze);

		size_t currSteps = MazeSolver::solveMaze(maze);
		if (currSteps > mostSteps) {
			maze.saveAsPPM("testMaze.ppm");    // Save the interesting maze.
			mostSteps = currSteps;
			std::cout << "New maze found with number of steps to solve " << currSteps << std::endl;
		}
	}
	
	std::cout << "Most steps: " << mostSteps << std::endl;
}

int main()
{
	findInterestingMaze();

	return 0;
}