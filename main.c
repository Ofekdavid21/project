#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "MainAux.h"
#include "BoardSolver.h"
#include "Game.h"
#include "Parser.h"
#include "SPBufferset.h"

int main(){
	int i;
	Game* game;
	currentMode = INIT;
	/* Initialize an element of "Game" type*/
	game=malloc(sizeof(Game));
	if (game==NULL){mallocFree();}
	game->cols=3;
	game->rows=3;
	game->N=9;
	game->board=calloc(1,sizeof(cell*)*game->N);
	if (game->board==NULL){mallocFree();}
    SP_BUFF_SET();
    game->head=NULL;
    game->curr=NULL;
	/* Allocating an space for the sudoku board*/
	for(i=0;i<game->N;i++){
		game->board[i]=calloc(1,sizeof(cell)*game->N);
		if (game->board[i]==NULL){mallocFree();}
	}
	printf("Welcome to our SUDOKO game! \n");
	while(1){
		translate(&game);
	}


}
