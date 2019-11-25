#ifndef GUROBI_H
#define GUROBI_H
#include "BoardSolver.h"
#include "gurobi_c.h"


/*	Game module summary:
 * 	The module is responsible for all the functions that involves directly using on the guroby library, and
 * 	interacting with the  solution (if exists) supplied by the guroby optimizer.
 */

/* The function initialize a three dimension array of integers,
 *	i.e in the <x,y> coordinates there is a array in size game.N full with '-1'.
 *  *  @param  game - Current game instance
 *  		   boardOfLists - a double dimension array of integers
 */
void initGurobiMat(Game *game, int*** boardOfLists);


/* The function fills a three dimension array according to our game.board - our sudoku board.
 *	I.e the <x,y> coordinates on the three dimension matrix there is a array of size game.N.
 *	And if v is a legal set on the sudkou board at cell <x,y> so the v index on the array equals cnt which is
 *	always not zero.
 *  *  @param  game - Current game instance
 *  		   boardOfLists - a three dimension array of integers
 */
int updateGurobiMat(Game *game, int*** boardOfLists);


/* The function is responsible for the interface with the Gurobi library
 * we send a sudoku puzzle, and the function converts our problem
 * into linear constrains (in case of lp we also add target function)
 * and the function send the linear problem to the Gurobi to gain a solution.
 * and according the to supply command we use the solution.
 * @param  game - Current game instance
 *  	   command - the first char of the relevant command (guess/hint_guess/generate/hint/validate).
 *  	   x- the x coordinate of the hinted cell
 *  	   y- the y coordinate of the hinted cell
 *  	   threshold - if the command is guess we need the threshold given by the user.
 *  @return -2 if there is no solution for our cutrent sudoko board, -1 if we encounter a vital error during the Guorbi optimization
 *  process  *
 *
 *  */
int mainGurobi(Game* game,char command,int x,int y,double threshhold);

/* The function frees our  3D MATRIX (double dimension array of cells);
 *  *  @param  game - Current game instance
 *  		   matrix - a triple dimension array of ints
 */
void free3DMatrix(Game *game,int*** matrix);

/* The function getting a solution to sudoku puzzle from the gurobi LP.
 *  fills all cell values with a score of 'threshold' or greater. If several
 *  values hold for the same cell, we randomly choose one according to the score.
 *  *  @param  game - Current game instance
 *  		   boardOfLists - matrix of lists
 *  		   sol - the array of solution from the gurobi
 *  		   x- the x coordinate of the hinted cell
 *  		   y- the y coordinate of the hinted cell
 */
void assingningGuessedValues(Game *game,int*** boardOfLists,double* sol,double threshhold);


/* The function getting a the solution to specific puzzle from the LP gurobi
 * and prints the possible values for cell <x,y> with probabilities.
 *  @param  game - Current game instance
 *  		boardOfLists - matrix of lists
 *  		sol - the array of solution from the gurobi
 *  		x- the x coordinate of the hinted cell
 *  		y- the y coordinate of the hinted cell
 */
void hintGuessedValues(Game *game,int*** boardOfLists,double* sol,int x,int y);


#endif
