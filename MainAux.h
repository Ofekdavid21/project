#ifndef MAINAUX_H
#define MAINAUX_H
#include "Game.h"
#include "Gurobi.h"
#include "BoardSolver.h"

/*	MainAux module summary:
 * 	The auxiliary functions are placed inside this module. Those are functions that
 *  do not belong to any of the other modules, and helping other modules.
 */


struct Game;




/* The function tries to load a sudoku board from a file saved in the supplied address,
 * if the all process succeed( the file is exits,the foramt is according the instructions, there is no
 * invalid values in the file and etc..),the old game module change to a new one with
 * new sudoku board, otherwise we stay with the old one.
 *  @param  game - Current game instance
 *			address - The path to the requested file
 *  @return 1 if the loading process succeeded , -1 otherwise.
 */
Game* load(char * address,Game* game,mode prevMode);

/* The function loads from the file that locate in address the size of
 * sudoku board into our game.cols and game.rows.That means the first numbers
 * on the file going to be saved to our current game (if the file is in the legal format).
 *  @param  game - Current game instance
 *  @return 1 if the calling succeeded , -1 otherwise.
 */
int loadTheSizeOfaBlock(char* address,Game* game);


/* The function loads the sudoko board saved in the address,into our game.board.
 * (if the file is in the legal format).
 *  @param  game - Current game instance
 *			address - the path to the requested file
 *  @return 1 if the loading succeeded , -1 otherwise.
 */
int loadTheValueIntoTheBoard(char* address,Game* game);

/* The function frees our  sudoku board (double dimension array of cells);
 *   @param  game - Current game instance
 */
void freeBoard(Game* game);


/* The function free memory that has been allocate for our current sudoku game module.
 *   @param  game - Current game instance
 */
void freeGame(Game* game);

/* The function frees part/all of our linked list of moves,
 * if  the parameter "clean" equal 1 , we free the all elements from game.current pointer
 * to the end of the list(not include game.current element).
 * if the parameter equal 0 we frees the entire list.
 *
 * @param  game - Current game instance
 * 		   clean- 0 for cleaning the entire list,1 for some of the list.
 *
 *  */
void freeLinkedList(Game* game,int clean);

/* The function updates our linked list of moves, by adding an new element at the end
 * of the list that represent our last move.
 *   @param  game - Current game instance
 *   @char  command - the first letter on the name of the command
 */
void updateLinkList(Game *game,char command);

/* The printBoard function prints our current sudoku board
 * In accordance with the instructions.
 *
 * @param  game - Current game instance
 *  */
void printBoard(Game* game);

/* Exit function In case a memory allocation has failed.
 *  */
void mallocFree();

/* The function counts how many full cells there are in the board.
 * @param  game - Current game instance
 * @return the number od the full cells
 *  */
int numOfFullyCells(Game *game);

/* The function checks  if the sudoku puzzle was solved successfully
 * @param  game - Current game instance
*/
void solvedSuccessfully(Game *game);

#endif
