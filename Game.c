#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Game.h"
#include "BoardSolver.h"

int seed=0;


void Set(int x, int y, int z ,Game *game){
	int tmp;
	tmp=y;
	y=x;
	x=tmp;
	/* The if condition checks whether the coordinates are legal according to the
	 * current board's size.
	 * */
	if((x>game->N||x<1)||(y>game->N||y<1)){
		printf("You gave an invalid indices\n");
		return;
	}
	if((z>game->N||z<0)){
		printf("You gave invalid value to put\n");
		return;
	}
	if(currentMode==SOLVE && game->board[x-1][y-1].fixed==1){
		printf("You can not change a FIXED cell in SOLVE mode\n");
		return;
	}
	if(z!=0 && iLLegalSet(game,x-1,y-1,z)){
		game->board[x-1][y-1].erroneous=1;
	}
	if(z==0){
		game->board[x-1][y-1].erroneous=0;
		if(game->board[x-1][y-1].value!=0){
			game->numOfFullCells--;
		}
	}
	else if(z!=0&&game->board[x-1][y-1].value==0){
		game->numOfFullCells++;
	}
	game->board[x-1][y-1].value=z;
	updateerroneous(game);
	freeLinkedList(game,1);
	updateLinkList(game,'s');
	print_board(game);
	/* If the Board is now full we call an function that operates according the situation*/
	if(currentMode==SOLVE&&game->numOfFullCells==game->N*game->N){
		solvedSuccessfully(game);
	}
	return;
}



Game* Solve(Game *game,char* address){
	mode prevMode;
	prevMode = currentMode;
	currentMode=SOLVE;
	return load(address,game,prevMode);
}



Game* Edit(Game *game,char* address,int i){
	/* In case an address hasn't been supplied*/
	mode prevMode;
	prevMode = currentMode;
	if (i==0){
		freeBoard(game);
		freeLinkedList(game,0);
		game->N=9;
		game->cols=3;
		game->rows=3;
		game->numOfFullCells=0;
		game->head=NULL;
		game->curr=NULL;
		game->board=calloc(1,sizeof(cell*)*game->N);
		if (game->board==NULL){mallocFree();}
		for(i=0;i<9;i++){
			game->board[i]=calloc(1,sizeof(cell)*game->N);
			if (game->board[i]==NULL){mallocFree();}
		}
		currentMode=EDIT;
		updateLinkList(game,'b');
		mark_errors(game,0);
		print_board(game);
		return game;
		}
	/* In case an address has been supplied */
	if (i==1){
		currentMode=EDIT;
		return load(address,game,prevMode);
	}
	return game;
}



void save(char * address,Game* game){
	FILE* fptr_out;
	int i,j;
	/*Checking if there is an erroneous cell, if so we can not save the current game board */
	for(i=0;i<(game->N);i++){
		for(j=0;j<game->N;j++){
			if(game->board[i][j].erroneous==1&&currentMode==EDIT){
				printf("You can't save an erroneous board in EDIT mode\n");
				return;
			}
		}
	}
	/*Checking if the board is validate */
	if( currentMode==EDIT && mainGurobi(game,'v',1,1,0.0)==-2){
		printf("enter edit\n");
		printf("Error: board validation failed so you c'ant save the board\n");
		return;

	}
	fptr_out=fopen(address,"w");
	if(fptr_out==NULL){
		printf("Error occurred while trying to get file named %s\n",address);
		return;
	}
	fprintf(fptr_out,"%d ",game->rows);
	fprintf(fptr_out,"%d\n",game->cols);
	/*Passing over the board and saving it to the file */
	for(i=0;i<(game->N);i++){
		for(j=0;j<game->N;j++){
			if((currentMode==SOLVE&&game->board[i][j].fixed)||(currentMode==EDIT&&game->board[i][j].value!=0)){
				fprintf(fptr_out,"%d.",game->board[i][j].value);/*for fixed cells*/

			}
			else{
				fprintf(fptr_out,"%d",game->board[i][j].value);/*for unfixed cells*/
			}
			if(j!=(game->N-1)){
				fprintf(fptr_out," ");
			}
		}
		fprintf(fptr_out,"\n");
	}
	fclose(fptr_out);
}



void validate(Game *game){
	/* Checking if there is an erroneous cell before using ILP*/
	if(checkErroneous(game)==1){
		printf("There is a cell with erroneous value,the board is not valid\n");
		return;
	}
	if(mainGurobi(game,'v',1,1,0.0)==-2){
		printf("The board is not solvable\n");
		return;
	}
	else{
		printf("The board is solvable\n");
	}

}




int checkErroneous(Game* game){
	int i,j;
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
			if(game->board[i][j].erroneous==1){
				return 1;/*there is a erroneous value*/
			}
		}
	}
	return 0;
}



void hint(Game* game,int x, int y){
	/* Note x is the colmun y is the row (minus 1) */
	int i;
	if((x>game->N||x<1)||(y>game->N||y<1)){
			printf("You gave an invalid indices\n");
			return;
		}
	if(checkErroneous(game)==1){
		printf("There a cell with erroneous value, the board is not valid so we cannot give you an hint\n");
		return;
	}
	if(game->board[y-1][x-1].fixed==1){
		printf("error : the cell is fixed\n");
		return;
	}
	if(game->board[y-1][x-1].value!=0){
		printf("the cell is already has an value\n");
		return;
	}

	i=mainGurobi(game,'h',y-1,x-1,0.0);/*Calls Gurobi for ILP */
	if(i==-2){
		printf("we cannot give you an hint, the board is not solvable\n");

	}

}




int generate(Game* game,int x,int y){
	int i,j,k;
	int** backUpBoard;
	int numOfCellsInTheBoard;
	int numOfVacantCells;
	numOfCellsInTheBoard= game->N*game->N;
	numOfVacantCells=numOfCellsInTheBoard - game->numOfFullCells;
	if(y<1 || y>numOfCellsInTheBoard){
		printf("The Y you gave is not good, it's too big or too small\n");
		return -1;
	}


	if(numOfVacantCells<x){
		printf("There is Not enough empty cells\n");
		return -1;
	}

	if(checkErroneous(game)==1){
		printf("There a cell with erroneous value, the board is not valid sos cannot be generate\n");
		return -1;
	}
	if(mainGurobi(game,'v',1,1,0.0)==-1){
		printf("The board is not solvable so cannot be generate\n");
		return -1;
	}
	/* We keep an copy of the board in case the generate process will fail*/
	backUpBoard=calloc(1,sizeof(int*)*game->N);
	if (backUpBoard==NULL){mallocFree();}
	for(i=0;i<game->N;i++){
		backUpBoard[i]=calloc(1,sizeof(int)*game->N);
		if (backUpBoard[i]==NULL){mallocFree();}
	}
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
			backUpBoard[i][j]=game->board[i][j].value;/* saving the values on the backup board*/
			}

	}
	/* Trying to generate the board for 1000 iterations, if after 1000 iterations
	 * we still did'nt make it we cease.
	 */
	for(i=0;i<10000;i++){
		/* If we pass the first part (filling X cells) we moves to the second part (to clean the board but the Y cell) */
		if(generatePartOne(game,x)==1){
			generatePartTwo(game,y);
			for(i=0;i<game->N;i++){
				free(backUpBoard[i]);
			}
			free(backUpBoard);
			game->numOfFullCells = numOfFullyCells(game);
			freeLinkedList(game,1);
			updateLinkList(game,'G');
			print_board(game);
			updateerroneous(game);
			if(currentMode==SOLVE&&game->numOfFullCells==game->N*game->N){
				solvedSuccessfully(game);
			}
			return 1;
		}
		/* In case the first part has failed we restore the original board*/
		for(k=0;k<game->N;k++){
			for(j=0;j<game->N;j++){
				game->board[k][j].value=backUpBoard[k][j];
				}

		}
	}
	for(i=0;i<game->N;i++){
		free(backUpBoard[i]);
	}
	free(backUpBoard);
	return 0;
}




int generatePartOne(Game* game,int x){
	int i,j,numOfEmptyCells,counter,randNum,n,temp_x,temp_y,top,row,col,temp;
	tuple* listOfTuples;/*List of vacant cells each cell represent by an tuple*/
	int* range;
	numOfEmptyCells=0;counter=0;n=0;
	/* Counting the number of empty cells*/
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
			if(game->board[i][j].value==0){
				numOfEmptyCells++;
			}
		}
	}
	if(numOfEmptyCells<x){
		printf("There is Not enough empty cells\n");
		return -1;
	}
	listOfTuples=calloc(sizeof(tuple),numOfEmptyCells);
	if (listOfTuples==NULL){mallocFree();}
	/* Creating a array of tuples ,the tuples includes the indices of the empty cells*/
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
			if(game->board[i][j].value==0){
				listOfTuples[counter].x=i;
				listOfTuples[counter].y=j;
				counter++;
			}
		}
	}

	srand(time(NULL));
	/*Now we randomly shifting the array of tuples*/
	while(n<x){
			randNum=rand()%numOfEmptyCells;
			temp_x=listOfTuples[randNum].x;
			temp_y=listOfTuples[randNum].y;
			listOfTuples[randNum].x=listOfTuples[numOfEmptyCells-1].x;
			listOfTuples[randNum].y=listOfTuples[numOfEmptyCells-1].y;
			listOfTuples[numOfEmptyCells-1].x=temp_x;
			listOfTuples[numOfEmptyCells-1].y=temp_y;
			numOfEmptyCells--;
			n++;
	}
	range=calloc(sizeof(int),game->N);
	if (range==NULL){mallocFree();}
	/*Now we filling the 'x' randomly selected cells with random legal values*/
	for(i=0;i<x;i++){
		for(j=0;j<game->N;j++){
			range[j]=j;
		}
		row=listOfTuples[numOfEmptyCells].x;
		col=listOfTuples[numOfEmptyCells].y;
		top=game->N;
		while(top!=0){
				srand(seed++);
				randNum=rand()%top;
				if(!iLLegalSet(game,row,col,range[randNum])){
					game->board[row][col].value=range[randNum];
					break;
					}
				temp=range[randNum];
				range[randNum]=range[top-1];
				range[top-1]=temp;
				top--;
		}
		if(top==0){ /* The generate process has fail because we cannot put an legal value in the chosen random cell*/
			free(listOfTuples);
			free(range);
			return -1;
		}
		numOfEmptyCells++;
	}
	i=mainGurobi(game,'g',-1,-1,0.0);
	free(listOfTuples);
	free(range);
	return i;
}

int generatePartTwo(Game* game,int y){
	int counter,i,j,maxi,n,randNum,temp_x,temp_y,row,col;
	tuple* listOfTuples;
	counter=0;n=0;
	maxi=game->N*game->N;
	listOfTuples=calloc(sizeof(tuple),game->N*game->N);
	if (listOfTuples==NULL){mallocFree();}
	/* creating a array of tuples ,the tuples includes the indices of the empty cells*/
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
				listOfTuples[counter].x=i;
				listOfTuples[counter].y=j;
				counter++;
		}
	}
	srand(time(NULL));
	/*Now we randomly shifting the array of tuples*/
	while(n<y){
			randNum=rand()%maxi;
			temp_x=listOfTuples[randNum].x;
			temp_y=listOfTuples[randNum].y;
			listOfTuples[randNum].x=listOfTuples[maxi-1].x;
			listOfTuples[randNum].y=listOfTuples[maxi-1].y;
			listOfTuples[maxi-1].x=temp_x;
			listOfTuples[maxi-1].y=temp_y;
			maxi--;
			n++;
	}
	for(i=0;i<maxi;i++){
		row=listOfTuples[i].x;
		col=listOfTuples[i].y;
		game->board[row][col].value=0;
	}

	free(listOfTuples);
	return 1;
}



int guess(Game* game,double threshold){
	if(checkErroneous(game)==1){
		printf("There a cell with erroneous value, the command has not executed\n");
		return -1;
	}
	if(mainGurobi(game,'L',1,1,threshold)==-2){
		printf("The board isn't solvable\n");
		return -1;
	}
	freeLinkedList(game,1);
	game->numOfFullCells = numOfFullyCells(game);
	updateLinkList(game,'g');
	print_board(game);
	updateerroneous(game);
	if(currentMode==SOLVE&&game->numOfFullCells==game->N*game->N){
		solvedSuccessfully(game);
	}
	return 1;
}





void guess_hint(Game* game,int x,int y){
	if((x>game->N||x<1)||(y>game->N||y<1)){
		printf("Invalid indices, try again\n");
		return;
	}
	if(checkErroneous(game)==1){
		printf("Error : there is a cell with erroneous value\n");
		return;
	}
	if(game->board[y-1][x-1].fixed==1){
		printf("Error : the cell is fixed\n");
		return;
	}
	if(game->board[y-1][x-1].value!=0){
		printf("The cell is already has an value\n");
		return;
	}
	/*Calling for the LP solution in the Gourbi module*/
	if(mainGurobi(game,'l',y-1,x-1,0.0)==-2){
		printf("Error : The board isn't solvable\n");
	}
}





void autofill(Game *game){
	int i,j,v,c,o;
	int **board;
	if(checkErroneous(game)==1){
		printf("There a cell with erroneous value, the board can not be autofilled\n");
		return;
	}
	board=calloc(1,sizeof(int*)*game->N);
	if (board==NULL){mallocFree();}
	/*Creating an empty  board that will store the "obvious" values */
	for(i=0;i<game->N;i++){
		board[i]=calloc(1,sizeof(int)*game->N);
		if (board[i]==NULL){mallocFree();}
	}
	/* Passing over the board and fill empty cells*/
	for(i = 0; i < game->N; ++i){
		for (j = 0; j < game->N; ++j){
			c=0;
			o=0;
			if(game->board[i][j].value!=0){
				continue;
			}
			for (v = 1; v < game->N+1; ++v){
				if(!iLLegalSet(game,i,j,v)){
					c++;
					o=v;
				}
				/* If there is more than 1 legal value we jump to next empty one*/
				if(c>1){
					break;
				}
			}
			if(c==1){
				board[i][j]=o;
				printf("Cell<%d,%d> changed to %d\n",j+1,i+1,o);

			}
		}
	}
	/*Coping the "obvious" values from the new Board that we made*/
	for(i = 0; i < game->N; ++i){
		for (j = 0; j < game->N; ++j){
			if(board[i][j]!=0){
				game->board[i][j].value=board[i][j];
			}
		}
	}
	for(i=0;i<game->N;i++){
		free(board[i]);
	}
	free(board);
	updateerroneous(game);
	freeLinkedList(game,1);
	game->numOfFullCells = numOfFullyCells(game);
	updateLinkList(game,'a');
	print_board(game);
	if(currentMode==SOLVE&&game->numOfFullCells==game->N*game->N){
		solvedSuccessfully(game);
	}
}





void num_solutions(Game* game){
	int i,j;
	int** backUpBoard;
	if(checkErroneous(game)==1){
		printf("we can't start num_solutions the borad is erroneous\n");
		return;
	}
	backUpBoard=calloc(1,sizeof(int*)*game->N);
	if (backUpBoard==NULL){mallocFree();}
	for(i=0;i<game->N;i++){
		backUpBoard[i]=calloc(1,sizeof(int)*game->N);
		if (backUpBoard[i]==NULL){mallocFree();}
	}
	/*Crating a copy of the current sudoku board.*/
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
			backUpBoard[i][j]=game->board[i][j].value;
			}

	}
	/*Edge case when the board is already fulled with values.*/
	if(game->numOfFullCells==game->N*game->N&&currentMode==EDIT){
		if(checkErroneous(game)==0){
			printf("The number of Solutions is %d\n",1);
		}
		else{
			printf("The number of Solutions is %d\n",0);
		}
	}
	/* Calling for exhaustiveBackTracking the function that count the number of possible solutions.*/
	else{
		printf("The number of Solutions is %d\n",exhaustiveBackTracking(game));
	}
	/* Restoring the current sudoku board */
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
			game->board[i][j].value=backUpBoard[i][j];
			}

	}
	for(i=0;i<game->N;i++){
		free(backUpBoard[i]);
	}
	free(backUpBoard);
	return ;
}





void undo(Game *game){
	int i,j,a,b;
	/*In case the player didn't do any move so far*/
	if(game->curr->c=='b'){
		printf("You can't undo because there are no moves to undo\n");
		return;
	}
	/* Restoring the previous status of the game*/
	else{
		game->numOfFullCells = game->curr->prev->numOfFullCells;
		for(i=0;i<game->N;i++){
			for(j=0;j<game->N;j++){
				/* Set the current move pointer to the previous move and updates the game.board*/
				a=game->curr->prev->backUpBoard[i][j].value;
				b=game->board[i][j].value;
				game->board[i][j].erroneous=game->curr->prev->backUpBoard[i][j].erroneous;
				game->board[i][j].fixed=game->curr->prev->backUpBoard[i][j].fixed;
				if(a!=b){
					printf("Cell<%d,%d> changed from %d to %d\n",j+1,i+1,b,a);
					game->board[i][j].value=game->curr->prev->backUpBoard[i][j].value;
				}
			}
		}
		game->curr = game->curr->prev;
		print_board(game);
	}
}




void redo(Game *game){
	int i,j,a,b;
	/*In case the there is not a move to be redo*/
	if(game->curr->next==NULL){
		printf("You can't redo because there are no moves to redo\n");
		return;
	}
	/* Restoring the next situation of the game*/
	else{
		game->numOfFullCells = game->curr->next->numOfFullCells;
		for(i=0;i<game->N;i++){
			for(j=0;j<game->N;j++){
				a=game->curr->next->backUpBoard[i][j].value;
				b=game->board[i][j].value;
				game->board[i][j].erroneous=game->curr->next->backUpBoard[i][j].erroneous;
				game->board[i][j].fixed=game->curr->next->backUpBoard[i][j].fixed;
				if(a!=b){
					printf("Cell<%d,%d> changed from %d to %d\n",j+1,i+1,b,a);
					game->board[i][j].value=game->curr->next->backUpBoard[i][j].value;
				}
			}
		}
		game->curr = game->curr->next;
		print_board(game);
	}
}






void reset(Game *game){
	int i,j;
	game->curr=game->head;
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
			game->board[i][j].erroneous=game->curr->backUpBoard[i][j].erroneous;
			game->board[i][j].fixed=game->curr->backUpBoard[i][j].fixed;
			game->board[i][j].value=game->curr->backUpBoard[i][j].value;
			}
		}
	print_board(game);
	}



void Exiti(Game *game){
	freeLinkedList(game,0);
	freeBoard(game);
	free(game);
	printf("Exit executed, Endgame.\n");
	exit(0);
	}





int mark_errors(Game *game,int i){
	if(i!=0 && i!=1){
		printf("The parameter for mark_errors should be 0 or 1 only\n");
		return 0;}
	if(i==1){
		game->mark_errors=1;
		return 1;
	}
	else
		game->mark_errors=0;
		return 1;
	}





void print_board(Game* game){
	printBoard(game);
}






