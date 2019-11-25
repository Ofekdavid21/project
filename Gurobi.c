#include "Gurobi.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


void hintGuessedValues(Game *game,int*** boardOfLists,double* sol,int x,int y){
	int v;
	printf("There are the legal values of cell <%d,%d> and their scores: \n",y+1,x+1);
	for(v=1;v<game->N+1;v++){
		if(boardOfLists[x][y][v]!=-1&&sol[boardOfLists[x][y][v]]>0.0){
			printf("value:%d with score:%f, ",v,sol[boardOfLists[x][y][v]]);
		}
	}
	printf("\n");
}



void assingningGuessedValues(Game *game,int*** boardOfLists,double* sol,double threshold){
	int i,j,v,sum,randNum,seed;
	double* possibleRandNums;
	seed=7;
	srand(seed);
	/*We create a randomly shuffled array of indices*/
	possibleRandNums = (double*)calloc(sizeof(double),game->N+1);
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
			for(v=1;v<game->N+1;v++){/*empty possibleRandNums*/
				possibleRandNums[v] = -1;
			}
			sum = 0;
			/* the v th index in the array gets the probability of value v from the LP solution*/
			for(v=1;v<game->N+1;v++){
				if(boardOfLists[i][j][v]!=-1&&sol[boardOfLists[i][j][v]]>threshold&&!iLLegalSet(game,i,j,v)){
					possibleRandNums[v]=sol[boardOfLists[i][j][v]];
				}
			}
			for(v=1;v<game->N+1;v++){/*0.3 become 3 and aggregate*/
				if(possibleRandNums[v]!=-1){
					sum = (int)(possibleRandNums[v]*10)+sum;
					possibleRandNums[v] = sum;
				}
			}
			randNum=rand()%(sum+1);
			/*Choose number to fill the cell with proportion to his weight*/
			for(v=1;v<game->N+1;v++){
				if(possibleRandNums[v]>=randNum){
					game->board[i][j].value = v;
					break;
				}
			}
		}
	}
	free(possibleRandNums);
}
	


void free3DMatrix(Game *game,int*** matrix){
	int i;
	int j;
	i=0;
	for(i=0;i<game->N;i++){
		for(j=0;j<game->N;j++){
			free(matrix[i][j]);
		}
		free(matrix[i]);
	}
	free(matrix);
}



void initGurobiMat(Game *game, int*** boardOfLists){
	int i,j,k;
	for (i = 0; i < game->N; ++i)
		boardOfLists[i] = (int**)malloc(sizeof (int *) * game->N);
	for(i = 0; i < game->N; ++i){
		for (j = 0; j < game->N; ++j){
			boardOfLists[i][j] = (int*)malloc(sizeof (int)*(game->N+1));
			for(k = 0; k < game->N+1; ++k){
				boardOfLists[i][j][k]=-1;/*fill the <i,j> array with -1*/
			}
		}
	}
}



int updateGurobiMat(Game *game, int*** boardOfLists){
	int i,j,v,cnt;
	cnt=0;
	for(i = 0; i < game->N; ++i){
		for (j = 0; j < game->N; ++j){
			/* if the <i,j> on the sudoku board is already full we skip to the next one*/
			if(game->board[i][j].value!=0){
				continue;
			}
			for (v = 1; v < game->N+1; ++v){
				if(!iLLegalSet(game,i,j,v)){
					/* if v is a legal assignment for the <i,j> cell in the puzzle we save the current counter
					 * in the <i,j> cell of our 3 dimension matrix*/
					boardOfLists[i][j][v]=cnt;
					cnt++;
				}
			}
		}
	}
	return cnt;
}




int mainGurobi(Game* game,char command,int x,int y,double threshold){
	int*** boardOfLists;
	GRBenv   *env   = NULL;
	GRBmodel *model = NULL;
	int      *ind;
	double   *val;
	char     *vtype;
	double   *sol;
	double   *obj;
	double   *ub;
	double   objval;
	int       i, j, v, ig, jg, countVariebles,optimstatus,cnt,randNum;
	int       error = 0;
	
	

	boardOfLists = (int***)malloc(sizeof (int **) *game->N );
	initGurobiMat(game,boardOfLists);
	countVariebles = updateGurobiMat(game,boardOfLists);
	/*Edge case, The Board is not full but there is no even one legal value
	 * to fill on the cells,so the board is not solvable */
	if(countVariebles==0&&game->numOfFullCells!=game->N*game->N){
		free3DMatrix(game,boardOfLists);
		return -2;
	}

	ind = (int*)calloc(game->N,sizeof(int));
	val = (double*)calloc(game->N,sizeof(double));
	obj = (double*)calloc(countVariebles,sizeof(double));
	vtype = (char*)calloc(countVariebles,sizeof(char));
	sol = (double*)calloc(countVariebles,sizeof(double));
	ub = (double*)calloc(countVariebles,sizeof(double));/*freeee*/
	
	/*Loading Gurobi environment*/
	error = GRBloadenv(&env, "mip1.log");
	if (error) {
	  printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
	  return -1;
	}
	/*Setting the parameters for the gurobi*/
	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
	  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
	  return -1;
	}

	/* Create an empty model named "mip1" */
	error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) {
	  printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
	  return -1;
	}

	/* Create an empty model */

	srand(time(NULL));
	for(i=0;i<countVariebles;i++){
		if(command=='L'||command=='l'){/*In case we are using LP*/
			randNum=(rand()%10)+1;
			obj[i]=randNum;
			ub[i]=1;
			vtype[i] = GRB_CONTINUOUS;
		}
		else{/*In case we are using ILP*/
			obj[i]=0;
			vtype[i] = GRB_BINARY;
		}
	}

	/* add variables to model */
	if(command=='L'||command=='l'){/*In case we are using LP we gave a upper bound 1 for every variable*/
		error = GRBaddvars(model, countVariebles, 0, NULL, NULL, NULL, obj, NULL, ub, vtype, NULL);
	}
	else{
		error = GRBaddvars(model, countVariebles, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
	}

	if (error) {
	  printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
	  return -1;
	}
	/* Change objective sense to maximization */
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
	  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
	  return -1;
	}

	/* update the model - to integrate new variables */

	error = GRBupdatemodel(model);
	if (error) {
	  printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
	  GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
	  return -1;
	}


	/* Constrain one : Each cell gets a value */

	for (i = 0; i < game->N; i++) {
		for (j = 0; j < game->N; j++) {
			cnt=0;
			for (v = 1; v < game->N+1; v++) {
				if(boardOfLists[i][j][v]!=-1){/*passing all the legal value for cell <i,j> */
					ind[cnt] = boardOfLists[i][j][v];
					val[cnt++] = 1.0;
				}
			}
			if(cnt==0){
				continue;
			}
			error = GRBaddconstr(model, cnt, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d 2nd GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
				return -1;
			}
		}
	}



  /* Constrain two : Each value must appear once and only once in each column */

	for (v = 1; v < game->N+1; v++) {
		for (j = 0; j < game->N; j++) {
			cnt=0;
			for (i = 0; i < game->N; i++) {
				if(boardOfLists[i][j][v]!=-1){
					ind[cnt] = boardOfLists[i][j][v];
					val[cnt++] = 1.0;
				}
			}
			if(cnt==0){
							continue;
						}
			error = GRBaddconstr(model, cnt, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d 2nd GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
				return -1;
			}
		}
	}

  /* Constrain three : Each value must appear once and only once in each row */

	for (v = 1; v < game->N+1; v++) {
		for (i = 0; i < game->N; i++) {
			cnt=0;
			for (j = 0; j < game->N; j++) {
				if(boardOfLists[i][j][v]!=-1){
					ind[cnt] = boardOfLists[i][j][v];
					val[cnt++] = 1.0;
				}
			}
			if(cnt==0){
							continue;
						}
			error = GRBaddconstr(model, cnt, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d 2nd GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
				return -1;
			}
		}
	}



  /* Constrain foue : Each value must appear once and only in each subgrid */

	for (v = 1; v < game->N+1; v++) {/*Passing every value*/
		for (ig = 0; ig < game->cols; ig++) {/*Passing every little subgrid*/
			for (jg = 0; jg < game->rows; jg++) {
				cnt=0;
				for (i = ig*game->rows; i < ig*game->rows+game->rows; i++) {/*Passing every row and column in subgrid*/
					for (j = jg*game->cols; j < jg*game->cols+game->cols; j++) {
						if(boardOfLists[i][j][v]!=-1){
							ind[cnt] = boardOfLists[i][j][v];
							val[cnt++] = 1.0;
						}	
					}
				}
			if(cnt==0){
								continue;
						}
			error = GRBaddconstr(model, cnt, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d 2nd GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
				return -1;
			}
		}
	}
}




	/* Optimize model - need to call this before calculation */
	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
		return -1;
	}
	/* Write model to 'mip1.lp' */
	error = GRBwrite(model, "mip1.lp");
	if (error) {
		printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
		return -1;
	}

	/* Get solution information */

	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
		return -1;
	}
	  /* solution found */
	 if (optimstatus == GRB_OPTIMAL) {
		/* get the objective -- the optimal result of the function */
		error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
		if (error) {
			printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
			GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
			return -1;
		}
	
		/* get the solution - the assignment to each variable */
		error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, countVariebles, sol);
		if (error) {
			printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
			GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
			return -1;
		}
		/*In case the command is hint we extract the solution of  cell <i,j>
		 * by searching a variable of this cell that equals 1 */
		if(command=='h'){
				for (v = 1; v < game->N+1; v++){
				if(boardOfLists[x][y][v]!=-1){
					if(sol[boardOfLists[x][y][v]]==1){
						printf("The value of cell <%d,%d> is %d\n",y+1,x+1,v);
					}
				}

			}
		}
		/*In case the command is generate we fill the entire sudoku board with the solution supplied by the Guorbi*/
		if(command=='g'){
				for (j = 0; j < game->N; j++) {
				for (i = 0; i < game->N; i++) {
					if(game->board[i][j].value!=0){
						continue;
					}
					for (v = 1; v < game->N+1; v++){
						if(boardOfLists[i][j][v]!=-1){
							if(sol[boardOfLists[i][j][v]]==1){
								game->board[i][j].value=v;
							}
						}

					}
				}
				}
		}
		/*In case the command is guess we sent the solution to our "assingningGuessedValues" function*/
		if(command=='L'){
			assingningGuessedValues(game,boardOfLists,sol,threshold);
		}
		/*In case the command is hint_guess we sent the solution to our "hintGuessedValues" function*/
		if(command=='l'){
			hintGuessedValues(game,boardOfLists,sol,x,y);
		}
		GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
		return 1;
	 }
	 /* no solution found */
	else if (optimstatus == GRB_INF_OR_UNBD) {
		printf("The board isn't solvable\n");
		GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
		return -2;

	}
	/* error or calculation stopped */
	else {
		/*printf("Optimization was stopped early\n");*/
		GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
		return -2;

	}


	/* IMPORTANT !!! - Free model and environment */
	GRBfreemodel(model);GRBfreeenv(env);free3DMatrix(game,boardOfLists);free(ind);free(val);free(obj);free(vtype);free(sol);free(ub);
	return -1;

}





