#ifndef PROBLEM_H
#define PROBLEM_H

#include "Clause.h"

/** 
 * We define our structure for our Problem.
 * a Problem is defined as an array of Clause (so itself an array of Variable)
 */
struct _Problem {

	unsigned int nbVariables;

	unsigned int nbClauses;

	Clause* arrayOfClauses;

};

/** We rename our structure to avoid to tape struct _Problem everywhere... */
typedef struct _Problem Problem;

#endif
