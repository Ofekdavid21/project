CC = gcc
OBJS = main.o MainAux.o Game.o BoardSolver.o Parser.o Gurobi.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors -g
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
all: $(EXEC)
main.o: main.c MainAux.h Parser.h BoardSolver.h SPBufferset.h Game.h 
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
MainAux.o: MainAux.c MainAux.h 
	$(CC) $(COMP_FLAG) -c $*.c
Game.o: Game.c Game.h 
	$(CC) $(COMP_FLAG) -c $*.c
BoardSolver.o: BoardSolver.c BoardSolver.h 
	$(CC) $(COMP_FLAG) -c $*.c
Parser.o: Parser.c Parser.h 
	$(CC) $(COMP_FLAG) -c $*.c
Gurobi.o: Gurobi.c Gurobi.h 
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c

clean:
	rm -f $(OBJS) $(EXEC)
