#include "Parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>




int digit_only(const char *s){
	int flag,count;
	count=0;
	flag=0;
	while(*s){
		/*In case the parameter is 01 we return 0*/
		if(flag==1){
			return 0;
		}
		if(*s=='0'&&count==0){
			flag=1;
		}
		if(isdigit(*s++)==0){
			return 0;
		}
		count++;

	}
	return 1;
}





int floating_only(const char *s){
	while(*s){
		if(isdigit(*s++)==0){
			return 0;
		}

	}
	return 1;
}



int twoFirstDigitis(char *s){
	int i;
	i=0;
	while(i<2){
		if(i==0&&(*s!='0'&&*s!='1')){
			return 0;
		}
		if(i==1&&*s!='.'){
			return 0;
		}
		s=s+1;
		i++;
	}
	return 1;
}


int parametersAreCorrect(int copyI,char* arg1,char* arg2,char* arg3){
		int digit;
		digit=0;
		while(copyI!=0){
			if(copyI==3){
				digit = digit_only(arg3);
			}
			if(copyI==2){
				digit = digit_only(arg2);
			}
			if(copyI==1){
				digit = digit_only(arg1);
			}
			if(digit==0){
				printf("Error: You gave invalid parameter\n");
				return -1;
			}
			copyI--;
		}
	return 1;

}





int translate (Game** game){
	int i,copyI;char line[258];
	char *tokens;char *arg0;char *arg1;char *arg2;char *arg3;
	arg0=NULL;
	arg1=NULL;
	arg2=NULL;
	arg3=NULL;
	i=0;
	line[256]='\0';
	printf("Enter a command please\n");
	fgets(line,1024,stdin);
	/* We treat any input line beyond 256 characters as an invalid command*/
	if(line[256]!='\0'){
		printf("Error: Too much parms for command (more than one line)\n");
		return -1;
	}
	tokens = strtok(line," \t\r\n");
	/* Separating the command into arguments (4 at most) and counts them*/
	while(tokens){
		if(i==4){
			i++;
			break;
		}
	      if(i==0){
	    	 arg0 = tokens;
	      }
	      if(i==1){
	        arg1 = tokens;
	      }
	      if(i==2){
	    	arg2 = tokens;
	      }
	      if(i==3){
	    	arg3 = tokens;
	      }
	      i++;
	      tokens = strtok(0," \t\r\n");
	}
	copyI = i-1;

	if(i==0){
		printf("Error: You didn't gave any input\n");
		return 0;
	}

	if(!strcmp(arg0,"solve")){
		if (i>2){
			printf("Error: You gave too many args to an solve commend\n");
			return -1;
		}
		if (i<2){
			printf("Error: You have to add an address to an solve commend\n");
			return -1;
		}
		*game=Solve(*game,arg1);
		return 1;
	}
	if(!strcmp(arg0,"edit")){
		if (i>2){
			printf("Error: You gave too much params\n");
			return -1;
		}
		if (i==1){
			Edit(*game,"Project",0);
		}
		if(i==2){
			*game=Edit(*game,arg1,1);
		}
		return 1;
	}
	if(!strcmp(arg0,"set")){
		if(i<4){
			printf("Error: Too few arguments\n");
			return -1;
		}
		if(i>4){
			printf("Error: Too much arguments\n");
			return -1;
		}
		if(parametersAreCorrect(copyI,arg1,arg2,arg3)==-1){/*If the parameters are not valid numbers*/
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not use set while you are in INIT mode\n");
			return -1;
		}
		if(atoi(arg1)==0||atoi(arg2)==0){
			printf("Error: The indices should be integers\n");
			return -1;
		}
		if(!strcmp(arg3,"0")){

		}
		else if (atoi(arg3)==0){
			printf("Error: The indices should be integers\n");
			return -1;
		}
		Set(atoi(arg1),atoi(arg2),atoi(arg3),*game);
		return 1;
	}
	if(!strcmp(arg0,"save")){
		if(i>2){
			printf("Error: Too much arguments\n");
			return -1;
		}
		if(i<2){
			printf("Error: Too few arguments\n");
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not use save while you are in INIT mode\n");
			return -1;
		}
		save(arg1,*game);
		return 1;
	}
	if(!strcmp(arg0,"validate")){
		if(i>1){
			printf("Error: You gave too much arguments\n");
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not use validate while you are in INIT mode\n");
			return -1;
		}
		validate(*game);
		return 1;
	}
	if(!strcmp(arg0,"hint")){
		if(i>3){
			printf("Error: You gave too much arguments\n");
			return -1;
		}
		if(i<3){
			printf("Error: You gave too few arguments\n");
			return -1;
		}
		if(parametersAreCorrect(copyI,arg1,arg2,arg3)==-1){/*If the parameters are not valid numbers*/
			return -1;
		}
		if(atoi(arg1)==0||atoi(arg2)==0){
			printf("Error: The indices should be integers\n");
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not use hint while you are in INIT\n");
			return -1;
		}
		if(currentMode==EDIT){
			printf("Error: You can not use hint while you are in EDIT mode\n");
			return -1;
		}

		hint(*game,atoi(arg1),atoi(arg2));
		return 1;
	}

	if(!strcmp(arg0,"generate")){
		if(i>3){
			printf("Error: You gave too much arguments\n");
			return -1;
		}
		if(i<3){
			printf("Error: You gave too few arguments\n");
			return -1;
		}
		if(parametersAreCorrect(copyI,arg1,arg2,arg3)==-1){/*If the parameters are not valid numbers*/
			return -1;
		}
		if(!strcmp(arg1,"0")){
			printf("Error: You gave too few empty cells to fill\n");
			return -1;
		}
		if(!strcmp(arg2,"0")){
			printf("Error: Too few cells were chosen to remain full\n");
			return -1;
		}
		if(atoi(arg1)==0||atoi(arg2)==0){
			printf("Error: The indices should be a integers\n");
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not use generate while you are in INIT\n");
			return -1;
		}
		if(currentMode==SOLVE){
			printf("Error: You can not use generate while you are in SOLVE mode\n");
			return -1;
		}
		if(generate(*game,atoi(arg1),atoi(arg2))==0){
			printf("Error: Generate has failed\n");
		}
		return 1;
	}

	if(!strcmp(arg0,"guess")){
		if(i>2){
			printf("Error: You gave too much arguments\n");
			return -1;
		}
		if(i<2){
			printf("Error: You gave too few arguments\n");
			return -1;
		}

		if(twoFirstDigitis(arg1)==0||floating_only(arg1+2)==0){
			printf("Error: The indices should be a doubles between 0.0 to 1.0\n");
			return -1;

		}
		if(!strcmp(arg1,"0")||!strcmp(arg1,"0.0")){

		}
		else if (atof(arg1)==0.0||atof(arg1)>1.0||atof(arg1)<0.0){
			printf("Error: The indices should be a doubles between 0.0 to 1.0\n");
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not use guess while you are in INIT\n");
			return -1;
		}
		if(currentMode==EDIT){
			printf("Error: You can not use guess while you are in EDIT mode\n");
			return -1;
		}
		guess(*game,atof(arg1));
		return 1;

	}
	if(!strcmp(arg0,"guess_hint")){
			if(i>3){
				printf("Error: You gave too much arguments\n");
				return -1;
			}
			if(i<3){
				printf("Error: You gave too few arguments\n");
				return -1;
			}
			if(parametersAreCorrect(copyI,arg1,arg2,arg3)==-1){/*If the parameters are not valid numbers*/
				return -1;
			}

			if(atoi(arg1)==0||atoi(arg2)==0){
				printf("Error: The indices should be integers\n");
				return -1;
			}
			if(currentMode==INIT){
				printf("Error: You can not use guess_hint while you are in INIT mode\n");
				return -1;
			}
			if(currentMode==EDIT){
				printf("Error: You can not use guess_hint while you are in EDIT mode\n");
				return -1;
			}
			guess_hint(*game,atoi(arg1),atoi(arg2));
			return 1;
		}
	if(!strcmp(arg0,"autofill")){
		if(i>1){
			printf("Error: You gave too much arguments\n");
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not use autofill while you are in INIT mode\n");
			return -1;
		}
		if(currentMode==EDIT){
			printf("Error: you can not use autofill while you are in EDIT mode\n");
			return -1;
		}
		autofill(*game);
		return 1;
	}

	if(!strcmp(arg0,"num_solutions")){
		if(i>1){
			printf("Error: You gave too much arguments\n");
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not use num_solutions while you are in INIT mode\n");
			return -1;
		}
		num_solutions(*game);
		return 1;
	}
	if(!strcmp(arg0,"undo")){
		if(i>1){
			printf("Error: You gave too much arguments\n");
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not use undo while you are in INIT mode\n");
			return -1;
		}
		undo(*game);
		return 1;
	}
	if(!strcmp(arg0,"redo")){
		if(i>1){
			printf("Error: You gave too much arguments\n");
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not use redo while you are in INIT mode\n");
			return -1;
		}
		redo(*game);
		return 1;
	}
	if(!strcmp(arg0,"reset")){
		if(i>1){
			printf("Error: You gave too much arguments\n");
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not use reset while you are in INIT mode\n");
			return -1;
		}
		reset(*game);
		return 1;
	}
	if(!strcmp(arg0,"exit")){
		if(i>1){
			printf("Error: You gave too much arguments\n");
			return -1;
		}
		Exiti(*game);
		return 1;
	}
	if(!strcmp(arg0,"mark_errors")){
		if(i>2){
			printf("Error: You gave too much arguments\n");
			return -1;
		}
		if(i<2){
			printf("Error: You gave too few arguments\n");
			return -1;
		}
		if(parametersAreCorrect(copyI,arg1,arg2,arg3)==-1){/*If the parameters are not valid numbers*/
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not use mark_erros while you are in INIT mode\n");
			return -1;
		}
		if(currentMode==EDIT){
			printf("Error: You can not use mark_erros while you are in EDIT mode\n");
			return -1;
		}

		if(!strcmp(arg1,"0")){

		}
		else if (atoi(arg1)==0){
			printf("Error: The argument should be 0 or 1\n");
			return -1;
		}
		mark_errors(*game,atoi(arg1));
		return 1;
	}

	if(!strcmp(arg0,"print_board")){
		if(i>1){
			printf("Error: You gave too much arguments\n");
			return -1;
		}
		if(currentMode==INIT){
			printf("Error: You can not print the board in INIT mode\n");
			return -1;
		}
		printBoard(*game);
		return 1;
	}

	printf("Error: There is no such a command\n");


	return 1;
}
