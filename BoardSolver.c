#include "BoardSolver.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>





int iLLegalSet(Game* game,int x,int y,int z){
	int i,j,leftcol,toprow;
	/*Passing over the row, to check if there is a z in the row already*/
	for(i=0;i<game->N;i++){
		if(game->board[x][i].value==z){
			if (i==y){
				continue;
			}
			return 1;
		}

	}
	/*Passing over the row, to check if there is a z in the column already*/
	for(i=0;i<game->N;i++){
		if(game->board[i][y].value==z){
			if (i==x){
				continue;
			}
			return 1;
		}
	}

	leftcol = (y/game->cols)*game->cols;
	toprow = (x/game->rows)*game->rows;
	/*Passing over the little grid, to check if there is a z in the grid already*/
	for(i=toprow;i<game->rows+toprow;i++){
		for(j=leftcol;j<game->cols+leftcol;j++){
			if(x==i&&y==j){
				continue;
			}
			if(game->board[i][j].value==z){
				return 1;
			}
		}
	}
	return 0;
}



void updateerroneous(Game* game){
	int i,j;
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
			if (game->board[i][j].value==0){
				game->board[i][j].erroneous=0;
				continue;
			}
			else{
				if(iLLegalSet(game,i,j,game->board[i][j].value)){
					game->board[i][j].erroneous=1;
				}
				else{
					game->board[i][j].erroneous=0;
				}
			}
		}
	}
}



int checkErroneousForFixedCell(Game* game,int x,int y,int z){
	int i,j,leftcol,toprow;
	/*Passing over the row, to check if there is a z in a fixed cell on the row already*/
	for(i=0;i<game->N;i++){
		if(game->board[x][i].value==z&&game->board[x][i].fixed==1){
			if (i==y){
				continue;
			}
			return 1;
		}
	}
	/*Passing over the column, to check if there is a z in a fixed cell on the colmun already*/
	for(i=0;i<game->N;i++){
		if(game->board[i][y].value==z&&game->board[i][y].fixed==1){
			if (i==x){
				continue;
			}
			return 1;
		}
	}

	leftcol = (y/game->cols)*game->cols;
	toprow = (x/game->rows)*game->rows;
	/*Passing over the little grid, to check if there is a z in a fixed cell on the grid already*/
	for(i=toprow;i<game->rows+toprow;i++){
		for(j=leftcol;j<game->cols+leftcol;j++){
			if(x==i&&y==j){
				continue;
			}
			if(game->board[i][j].value==z&&game->board[i][j].fixed==1){
				return 1;
			}
		}
	}
	return 0;
}




void push(int x,int y,Node** head){
	Node* tempNode;
	/*In case the stack is empty*/
	if(*head==NULL){
		tempNode=(struct Node*)malloc(1*sizeof(Node));
		tempNode->x=x;
		tempNode->y=y;
		*head=tempNode;
		(*head)->prev=NULL;
	}
	/*In case There are an elements in the stack*/
	else{
		tempNode=(struct Node*)malloc(1*sizeof(Node));
		tempNode->x=x;
		tempNode->y=y;
		tempNode->prev=*head;
		*head=tempNode;
	}
}


void pop(Node** head){
	Node* tempNode;
	tempNode=*head;
	*head=(*head)->prev;
	free(tempNode);

}




int getTheIndicesOfTheNextEmptyCell(Game *game ,Node** head){
	int i,j;
	i=0;
	if(*head!=NULL){
		i=(*head)->x;
	}
	for(;i<game->N;i++){
		for(j=0;j<game->N;j++){
			if(game->board[i][j].value==0){
				push(i,j,head);
				return 1;
			}
		}
	}
	return 0;
}



int exhaustiveBackTracking(Game* game){
	int counter,x_origin,y_origin,x,y;
	Node* head;
	x_origin=0;y_origin=0;
	counter = 0;
	head=NULL;
	/*In case the board is full we leaving*/
	if(getTheIndicesOfTheNextEmptyCell(game,&head)==0){
		return 0;
	}
	/*Retrieving the indices from the call to getTheIndicesOfTheNextEmptyCell*/
	x_origin=head->x;
	y_origin=head->y;
	x=x_origin;
	y=y_origin;
	game->board[x][y].value+=1;
	/* While we did not backtrack from the  <x_origitn,y_origin> cell*/
	while(game->board[x_origin][y_origin].value<(game->N)+1){
		/*In case we exhausts all the possible assignments for the cell we backtracks*/
		if(game->board[x][y].value==(game->N+1)){
			game->board[x][y].value=0;
			x=head->prev->x;
			y=head->prev->y;
			pop(&head);
			game->board[x][y].value+=1;
			continue;
		}
		/*Trying to put a new value on the cell and move forwards */
		if(iLLegalSet(game,x,y,game->board[x][y].value)){
			game->board[x][y].value+=1;
			continue;
		}
		/*In case we are in the last empty cell we increase the counter for every legal value */
		if(getTheIndicesOfTheNextEmptyCell(game,&head)==0){
			counter++;
			game->board[x][y].value+=1;
			continue;
		}
		/*Retrieving the coordinates for the next empty cell*/
		x=head->x;
		y=head->y;
		game->board[x][y].value+=1;
	}

	pop(&head);
	return counter;
}
