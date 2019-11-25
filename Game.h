#ifndef GAME_H
#define GAME_H


/*	Game module summary:
 * 	The module is responsible for the functions that the user enters directly to the console,
 * 	and contains the main structs of the game such as "Game","cell" and Element.
 */



/* An enum named mode that indicate the current game status */

typedef enum {INIT,SOLVE,EDIT} mode;

/* Initialize a global variable from type mode that store the current game status */

mode currentMode;


/*	cell Struct summary:
 *
 * 	A container that holds :
 *	value - the value of a specific cell on a sudoku board
 *	fixed - An indicator for an fixed cell, turn on (aka equals 1) when a cell is fixed
 *	erroneous - An indicator for an erroneous cell, turn on (aka equals 1) when a cell is erroneous
 * */


typedef struct {
	int value;
	int fixed;
	int erroneous;
}cell;

/*	Element Struct summary:
 * 	A node on the linked list of the player's moves ,useful for 'do','undo' and 'reset'
 *	backUpBoard - In case we need it ,we keep a copy of the board, for example for 'guess' on an empty board
 *	numOfFullCells - The number of cells with value on an specific move
 *	prev - An pointer to previous element on the doubly-linked list of the user's moves
 *	next - An pointer to next element on the doubly-linked list of the user's moves
 * */


typedef struct Element{
     char c;
     cell** backUpBoard;
     int numOfFullCells;
     struct Element *prev;
     struct Element *next;
} Element;

/*	Game Struct summary:
 *
 * 	A container that holds :
 *	rows - the number of rows of current Sudoku game.
 *	cols - the number of cols of current Sudoku game.
 *	N - rows*cols
 *	numOfFully - the number of the cells in the game which have already been filled
 * 	mark_errors - the "mark errors" parameter
 *	board - the matrix of "cells" that represents the player's Sudoku board.
 *	head - the "head" of the doubly-linked list of the user's moves
 *	curr - the current move Element on the doubly-linked list of the user's moves
 * */


typedef struct {
	int N;
	int rows;
	int cols;
	int mark_errors;
	int numOfFullCells;
	cell** board;
	Element* head;
	Element* curr;
}Game;

typedef struct {
	int x;
	int y;
}tuple;

/* The function Starts a puzzle in EDIT mode loaded from a file with the name "address", where address
 * includes a full or relative path to the file with help of "load" function.
 * address is optional, if address is not supplied we enter EDIT with an empty 9x9 board.
 *
 *  @param  game - Current game instance
 *	 		address - the address of the file to be loaded
 *			i - An integer equals 1 if an address has been supplied, otherwise equals 0
 *	@return a pointer to the current game instance
 *
 *
 */
Game* Edit(Game *game,char* address,int i);
/* The function gets a coordinates and value from the user,
 * and puts the value on the 'Game.Board' according to the coordinates if it is legal.
 *  @param  x - number of colum
 *	 		y - number of row
 *	 		z - the value to put
 *	 		game - Current game instance
 *
 */
void Set(int x, int y, int z ,Game *game);
/* The function Saves the current game board to the specified file, where X includes a full or
 * relative path to the file, according to the format that been given in the instructions file.
 *  @param  game - Current game instance
 *	 		address - the address of the file to be loaded
 */
void save(char * address,Game* game);
/* The function passes over the current game board and checks if there is
 * an erroneous cell.
 *  @param  game - Current game instance
 */
void hint(Game* game,int x, int y);
/* The function Validates the current board using ILP via "Gurobi" module, ensuring it is solvable,
 * and prints the result to the user.
 *  @param  game - Current game instance
 */
void validate(Game *game);
/* The function terminates the program but before it freeing all memory resources.
 *  @param  game - Current game instance
 */
void Exiti(Game *game);
/* The function Starts a puzzle in Solve mode loaded from a file with the name "address", where address
 * includes a full or relative path to the file with help of "load" function.
 *  @param  game - Current game instance
 *	 		address - the address of the file to be loaded
 *
 *	@return a pointer to the current game instance
 *
 *
 */
Game* Solve(Game *game,char* address);
/* The function passes over the current game board and checks if there is
 * an erroneous cell.
 *  @param  game - Current game instance
 *  @return 1 if there is an erroneous cell, 0 otherwise.
 */
int checkErroneous(Game* game);
/* The function generates a sudoku board by randomly filling X empty cells with legal values,
 * running ILP to solve the board, and then clearing all but Y random cells.
 *  @param  game - Current game instance
 *  		x - the number empty cells to be filled
 *  		y - the number of cell to not clear
 *  @return 1 if the process succeeded, -1 otherwise.
 */
int generate(Game* game,int x,int y);
/* The function randomly choose 'x' empty cells and after it filling them with
 * randomly legal values.
 *  @param  game - Current game instance
 *  		x - the number empty cells to be filled
 *  @return 1 if the process succeeded, -1 otherwise.
 */
int generatePartOne(Game* game,int x);
/* The function randomly choose (game.N*game.N)-'Y' cells and
 * clearing them.
 *  @param  game - Current game instance
 *  		x - the number empty cells to be filled
 *  @return 1 if the process succeeded, -1 otherwise.
 */
int generatePartTwo(Game* game,int y);
/* The function guesses a solution to the current board using LP with threshold X.
 *  @param  game - Current game instance
 *  		x - the number empty cells to be filled
 *  @return 1 if the process succeeded, -1 otherwise.
 */
int guess(Game* game,double threshold);
/* The function print a guess to the user for a cell [X,Y].
 *  @param  game - Current game instance
 *  		x - the x coordinate
 *  		y- the y coordinate
 */
void guess_hint(Game* game,int x,int y);
/* The function fill cell cells which contain a single legal value.
 *  @param  game - Current game instance
 */
void autofill(Game *game);
/* The function Print the number of solutions for the current board.
 *  @param  game - Current game instance
 */
void num_solutions(Game* game);
/* The function redo a move previously undone by the user.
 *  @param  game - Current game instance
 */
void redo(Game *game);
/* The function undo the previous move that the user made.
 *  @param  game - Current game instance
 */
void undo(Game *game);
/* The function restoring the board to its original loaded state.
 *  @param  game - Current game instance
 */
void reset(Game *game);
/* The function Sets the "mark errors" setting to i, where i is either 0 or 1.
 *  @param  game - Current game instance
 *  		i	 - The parameter from the user, '1' is for turning on mark_errors.
 *  @return 1 if the process succeeded, 0 otherwise.
 *
 */
int mark_errors(Game *game,int i);
/* The function calls "print_board" in order to prints the board to the user.
 *  @param  game - Current game instance
 */
void print_board(Game* game);

#endif
