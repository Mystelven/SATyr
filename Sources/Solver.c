#include "../Headers/Solver.h"

unsigned int isSolution(Solver* solver, Problem* problem) {

	for(unsigned int i = 0; i < problem->nbClauses; i++) {

		if(isOpen(solver,&problem->arrayOfClauses[i]) == 0) {

			return 0;
		}
	}

	return 1;
}


void solveProblem(Solver* solver, Problem* problem) {

	unsigned int indiceRandom = 0;

	while(isSolution(solver,problem) != 1) {

		indiceRandom = (unsigned int) rand() % (solver->nbVariables);

		solver->arrayOfSolutions[indiceRandom] *= -1;

	}
	
}

Solver* initSolver(Problem* problem) {

	Solver* solver = (Solver*)malloc(sizeof(Solver)*1);
	solver->nbVariables = problem->nbVariables;

	solver->arrayOfSolutions = (int*)malloc(sizeof(int)*solver->nbVariables);
	if(solver->arrayOfSolutions == NULL) {

		perror("Allocation problem in Solver* initSolver(Problem* problem)");
		exit(-2);
	}

	for(unsigned int i = 1; i <= solver->nbVariables; i++) {
		solver->arrayOfSolutions[i-1] = (int)i;
	}

	return solver;
}


unsigned int isOpen(Solver* solver,Clause* clause) {

	for(unsigned int i = 0; i < clause->nbVariables; i++) {


		unsigned int id = clause->arrayOfVariables[i].id;
		unsigned int value = clause->arrayOfVariables[i].value;

		if(solver->arrayOfSolutions[id] >= 0 && value == true) return 1;

		else if(solver->arrayOfSolutions[id] < 0 && value == false) return 1;

	}

	return 0;
}

void displaySolution(Solver* solver) {

	for(unsigned int i = 0 ; i < solver->nbVariables; i++) {

		printf("%d ",solver->arrayOfSolutions[i]);
	}
	printf("\n");
}