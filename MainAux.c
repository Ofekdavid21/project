#include "MainAux.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>



int loadTheSizeOfaBlock(char* address,Game* game){
	FILE* fptr_in;
	int c,a,words,num,m,n;
	a=0;num=0;words=0;
	fptr_in=fopen(address,"r");
	if(fptr_in==NULL){
		printf("Error occurred while trying to get file: %s, probably the file is not exists\n",address);
		return -3;
	}
	while(!((c=fgetc(fptr_in))==EOF))
	{
		if(isspace(c)){
			if(a==0){
				continue;
			}
			/* The first number on the file is the number of the rows*/
			if(words==1){
				a=0;
				m=num;
				num=0;
				game->rows=m;
				continue;
			}
			/* The first number on the file is the number of the cols*/
			if(words==2){
				n=num;
				game->cols=n;
				fclose(fptr_in);
				return 1;
			}
		}
		if(c<48||c>57){
			printf("not legal a char\n");
			fclose(fptr_in);
			return -1;
		}
		c=c-48;
		/* if we did not encounter an space char yet we add the digit to the num variable*/
		if(a==0){
			num+=c;
			a++;
			words++;
		}
		else
		num=(num*10)+c;

	}
	printf("Error: the file is not on the right format\n");
	fclose(fptr_in);
	return -1;
}



int loadTheValueIntoTheBoard(char* address,Game* game){
	FILE* fptr_in;
	int c,cprev,a,words,num,d,counter,i,j,falgForFixed;
	a=0;words=0;num=0;i=0;j=0;falgForFixed=0,counter=0;
	cprev = 32;
	fptr_in=fopen(address,"r");
	if(fptr_in==NULL){
		printf("Error occurred while trying to get file: %s\n",address);
		return -3;
	}
	/*Skipping the two first numbers, those are the size of the boards*/
	fscanf(fptr_in,"%d",&d);
	fscanf(fptr_in,"%d",&d);
	while(!((c=fgetc(fptr_in))==EOF))
	{
		/* If there too many numbers on the file*/
		if(!isspace(c)&&counter==(game->N*game->N)){
			printf("Too much values On the File\n");
			fclose(fptr_in);
			return -1;
		}
		/* If we load a space char after a space char*/
		if(isspace(c)&&a==0){
			continue;
		}
		if(isspace(c)){
				a=0;
				/* If there is a Number with value bigger than the size of the grid=game.N*/
				if(num<0||num>game->N){
					printf("Error:Invalid value on the file, not in the legal range\n");
					fclose(fptr_in);
					return -1;
				}
				game->board[i][j].value=num;
				/*if there is a '.' after the number we fixed the cell				 */
				if(falgForFixed==1){
					if(num==0){
						printf("Invalid value on the file: .0\n");
						fclose(fptr_in);
						return -1;
					}
					game->board[i][j].fixed=1;
					falgForFixed=0;
				}
				counter++;
				j=j+1;
					/* We filled the all row so we jump to the next one*/
				if(j==game->N){
					i=i+1;
					j=0;
				}
				num=0;
				continue;
		}
		if(a==0&&(c<48||c>57)){
			printf("Invalid char in the file\n");
			fclose(fptr_in);
			return -1;
		}
		if(c!=46&&a>0&&(c<48||c>57)){
			printf("Invalid char in the file\n");
			fclose(fptr_in);
			return -1;
		}
		if(c==46){/* char 46= '.'*/
			falgForFixed=1;
			continue;

		}
		c=c-48;
		if(a==0){
			num+=c;
			a++;
			words++;
		}
		else{
			num=(num*10)+c;
			if(falgForFixed==1){
				printf("Invalid value on the file such as 7.7\n");
				fclose(fptr_in);
				return -1;
			}
		}
		if(counter==game->N*game->N-1){
			cprev=c;
		}
	}
	if(isspace(cprev)!=32&&counter==game->N*game->N-1){
		if(num<0||num>game->N){
				printf("invalid value on the file\n");
				fclose(fptr_in);
				return -1;
			}
			game->board[i][j].value=num;
			/*if there is a '.' after the number we fixed the cell*/
			if(falgForFixed==1){
				if(num==0){
					printf("Invalid value on the file: .0\n");
					fclose(fptr_in);
					return -1;
				}
			game->board[i][j].fixed=1;
			}
			counter++;
	}



	if(counter<(game->N*game->N)){
		printf("not enough values on the file\n");
		return -1;
	}
	/*If we are in SOLVE mode we check if there is not erroneous fixed cell*/
	if(currentMode==SOLVE){
		for(i=0;i<game->N;i++){
			for(j=0;j<game->N;j++){
				if (game->board[i][j].fixed){
					if(checkErroneousForFixedCell(game,i,j,game->board[i][j].value)){
						printf("You can't load a file with a erroneous fixed cells\n");
						return -1;
					}
				}
			}
		}
	}

	return 1;
}






Game* load(char* address,Game* game,mode prevMode){
	int a,i;
	/* Initialize an new game instance in case we will need it for the new board*/
	Game* newGame=malloc(sizeof(Game));
	if (newGame==NULL){mallocFree();}
	newGame->head=NULL;
	newGame->curr=NULL;
	a=loadTheSizeOfaBlock(address,newGame);
	if(a==-3){/*In case there is no such an address we stops*/
		free(newGame);
		currentMode = prevMode;
		return game;
	}
	/*Edge case : board's dimensions are not legal*/
	if(a==-1||(newGame->cols*newGame->rows>99)||(newGame->cols*newGame->rows==0)){
		free(newGame);
		printf("Not a legal board to be loaded\n");
		currentMode = prevMode;
		return game;
	}
	newGame->N=(newGame->cols*newGame->rows);
	newGame->board=calloc(1,sizeof(cell*)*newGame->N);
	if (newGame->board==NULL){mallocFree();}
	for(i=0;i<newGame->N;i++){
		newGame->board[i]=calloc(1,sizeof(cell)*newGame->N);
		if (newGame->board[i]==NULL){mallocFree();}
	}
	i=loadTheValueIntoTheBoard(address,newGame);
	/*If we successfully loaded the new board, we return an pointer the new Game module */
	if(i==1){
		updateerroneous(newGame);
		freeLinkedList(game,0);
		freeGame(game);
		newGame->numOfFullCells = numOfFullyCells(newGame);
		updateLinkList(newGame,'b');
		mark_errors(newGame,1);
		print_board(newGame);
		if(currentMode==SOLVE&&newGame->numOfFullCells==newGame->N*newGame->N){
			solvedSuccessfully(newGame);
		}
		return newGame;
	}
	/*If we had fail loaded the new board, we return an pointer the original Game module */
	currentMode = prevMode;
	freeGame(newGame);
	return game;
}


void freeGame(Game* game){
	freeBoard(game);
	free(game);
}

void freeBoard(Game* game){
	int i;
	i=0;
	for(i=0;i<game->N;i++){
		free(game->board[i]);
	}
	free(game->board);
}



void updateLinkList(Game *game,char command){
	int i,j;
	Element *newElem;
	newElem = (Element*)calloc(1,sizeof(Element));
	if (newElem==NULL){mallocFree();}
	newElem->c = command;/*Inserting the name of the new move to an Element object*/
	newElem->backUpBoard=calloc(1,sizeof(cell*)*game->N);
	if (newElem->backUpBoard==NULL){mallocFree();}
	for(i=0;i<game->N;i++){
		newElem->backUpBoard[i]=calloc(1,sizeof(cell)*game->N);
		if (newElem->backUpBoard[i]==NULL){mallocFree();}
	}
	newElem->numOfFullCells = game->numOfFullCells;
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
			newElem->backUpBoard[i][j].value=game->board[i][j].value;
			newElem->backUpBoard[i][j].fixed=game->board[i][j].fixed;
			newElem->backUpBoard[i][j].erroneous=game->board[i][j].erroneous;
		}

	}
	/*If the the list is empty we insert the element as the head of the list*/
	if(game->head==NULL){
		game->head = newElem;
		game->curr = newElem;
		newElem->next=NULL;
		newElem->prev=NULL;
	}
	/*Else we insert the element in the end and changing our current move pinter
	 * to point to the element*/
	else{
		newElem->prev=game->curr;
		newElem->next=NULL;
		game->curr->next=newElem;
		game->curr = newElem;
	}
}



void freeLinkedList(Game* game,int clean){/*clean=0 SOLVE,EDIT, clean=1 the rest of commands*/
	int i;
	Element *cleaner,*tmp;
	if(clean==0){
		cleaner = game->head;
	}
	else{
		cleaner = game->curr->next;
	}

	while(cleaner!=NULL){
		for(i=0;i<game->N;i++){
			free(cleaner->backUpBoard[i]);
		}
		free(cleaner->backUpBoard);
		tmp = cleaner;
		cleaner = cleaner->next;
		free(tmp);
	}
}




void printBoard(Game* game){
	cell cellToPrint;
	int i,j,celli;
	celli = 4*game->N+game->rows+1;
	for(i=0;i<celli;i++){
		printf("-");
	}
	printf("\n");
	for(j=0;j<game->N;j++){
			for(i=0;i<game->N;i++){
				cellToPrint=game->board[j][i];
				if(i==0){
					printf("|");
				}
				if(cellToPrint.value==0){
					printf("   ");}
				else{
					printf(" %2d", cellToPrint.value);}
				if(currentMode==EDIT||game->mark_errors==1){
					if(currentMode==SOLVE&&cellToPrint.fixed==1){
						printf(".");}
					else if(cellToPrint.erroneous==1){
						printf("*");
					}
					else{
						printf(" ");
					}

				}
				else{
					if(cellToPrint.fixed==1){
						printf(".");}
					else{
						printf(" ");
					}
				}
				if((i+1)%(game->cols)==0){
					printf("|");
				}
			}
			printf("\n");
			if((j+1)%game->rows==0){
				for(i=0;i<celli;i++){
					printf("-");
				}
				printf("\n");

			}
	}

}




void mallocFree(){
	printf("Error occurred while trying to allocate memory\n");
	exit(0);
}


int numOfFullyCells(Game *game){
	int i,j,cnt;
	cnt=0;
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
			if(game->board[i][j].value!=0){
				cnt++;
			}
		}
	}
	return cnt;
}

void solvedSuccessfully(Game *game){
	if(checkErroneous(game)==0){
		printf("The puzzle was solved successfully\n");
		currentMode=INIT;
	}
}
