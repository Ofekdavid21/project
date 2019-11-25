#ifndef BoardSolver
#define BoardSolver
#include "MainAux.h"
#include "Gurobi.h"

/*	Game module summary:
 * 	The module is responsible for the functions that involves directly on the sudoku board
 * 	and contains the "Node" struct.
 */


/*	Node Struct summary:
 *
 * 	An item in an stack that holds :
 *	x - the x coordinate of the Node
 *	y - the y coordinate of the Node
 *	prev - an pointer to the previous stack in the stuck
 * */

typedef struct Node{
     int x;
     int y;
     struct Node *prev;
} Node;

/* The function checks if it is illegal according to sudoku's rules
 *  to assign the value z on the <y,x> cell;
 *  @param  game - Current game instance
 *			x - the x coordinate of a specific cell
 *			y - the y coordinate of a specific cell
 *  		z - the value for the testing
 *  @return 1 if the assignment illegal , 0 otherwise ( the assignment legal).
 */
int iLLegalSet(Game* game,int x,int y,int z);

/* For each cell in the game board the function check if
 * the cell is erroneous or not, and updating his 'erroneous' field.
 *  @param  game - Current game instance
 */

void updateerroneous(Game* game);

/* The function checks if it is illegal according to sudoku's rules
 *  to assign the value z on the <y,x> fixed cell, but we only consider
 *  fixed cells for the testing.
 *  @param  game - Current game instance
 *			x - the x coordinate of a specific cell
 *			y - the y coordinate of a specific cell
 *  		z - the value for the testing
 *  @return 1 if the assignment illegal , 0 otherwise ( the assignment legal).
 */

int checkErroneousForFixedCell(Game* game,int x,int y,int z);

/* The function crates a new Node element and pushes it to a exiting stack as the new head.
 *		x - the x coordinate of the Node
 *		y - the y coordinate of the Node
 *  	head - an pointer to pointer to head of the stack
 */

void push(int x,int y,Node** head);

/* The function pops the head of the stack
 *  	head - an pointer to pointer to head of the stack
 */

void pop(Node** head);

/* The function searches for the first cell with no value(means 0),
 * and push to the stack a Node with the coordinates found.
 *  @param  game - Current game instance
 *  		head - an pointer to pointer to head of the stack
 *  @return 1 we found an empty cell, 0 otherwise.

 *
 */

int getTheIndicesOfTheNextEmptyCell(Game *game ,Node** head);

/* The function  exhausts all possible values for all empty cells of the board,
 * counting all solutions found in this process.
 * Once the algorithm backtracks from the 1st empty cell,
 * the function is finished, and the counter contains the number of different solutions available
 * for the current board.
 * @param  game - Current game instance
 *  @return counter-the number of solutions for the current board.

 */

int exhaustiveBackTracking(Game* game);
#endif
