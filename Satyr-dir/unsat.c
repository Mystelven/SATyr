/*##############################################################################
# 
# Makefile for SATyr - Valentin Montmirail - Polytech Tours, France
# Copyright (c) 2015.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
# associated documentation files (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute,
# sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or
# substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
# OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
##############################################################################*/

#include "unsat.h"
#include "utils.h"
#include <stdlib.h>

/************************************************************************************************/
/*																								*/
/* getWhichVariableToPerformResolution : Get the most probable variable for a resolution 		*/
/* @param population the population where that we give us the good clauses						*/
/* @return the value of the litteral where we will perform a resolution 						*/
/*																								*/
/************************************************************************************************/
inline int getWhichVariableToPerformResolution(Individual* population) {

	/* We will go through every individual inside the population */
	Individual* tmp = population;

	int i,j;

	int min = BIG;

	int whichVariable = (random() % numatom)+1;

	while(tmp != NULL) {

		/* For every clauses still false for this individual */
		for(i = 0 ; i < tmp->numfalse; ++i) {
				
			int c = tmp->clausesFalse[i];

			if( size[tmp->clausesFalse[i]] < min) {
				
				min = size[tmp->clausesFalse[i]];
				
				j = (random() % size[tmp->clausesFalse[i]]);

				whichVariable = clause[c][j];

				for(i = 0;  i < sizeTabuVar; ++i) {
			
					if(ABS(whichVariable) == tabuVariables[i]) {
						
						whichVariable = (random() % numatom)+1;
					}
				}
			}
		} 

		tmp = tmp->next;
	}

	/*
	if(random() % 100 < 80) {
		whichVariable = (random() % numatom)+1;
	}
	*/

	/* ------------------------------------------------------------------------ */


	return ABS(whichVariable);
}


/************************************************************************************************/
/*																								*/
/* contains : This function will test if the variable is include inside the clause 				*/
/* @param cl the clause who maybe contains the variable 										*/
/* @param variable the variable that we need to check 											*/
/* @return TRUE if the clause contains the variable, FALSE otherwise. 							*/
/*																								*/
/************************************************************************************************/
inline char contains(int cl, int variable) {

	int i = 0;

	for(i = 0; i < size[cl]; ++i) {

		if(clause[cl][i] == variable) return TRUE;
	}

	return FALSE;
}

/************************************************************************************************/
/*																								*/
/* getWhichClausesToPerformResolution : We get the 2 clauses for the resolution 				*/
/* @param population the population where that we give us the good clauses						*/
/* @param whichVariable on which variable we will perform the resolution..						*/
/* @return a 2-cases array for the clauses where we will perform the resolution 				*/
/*																								*/
/************************************************************************************************/
inline int* getWhichClausesToPerformResolution(Individual* population,int whichVariable) {

	/* We allocate the good size for our 2 clauses. */
	int* clausesForResolution = (int*)malloc(sizeof(int)*2);

	int i = 0;
	int j = 0;
	int l = 0;

	int c1 = -1;
	int sizeC1;

	int c2 = -1;
	int sizeC2;

	int k = 0;
	
	char toAdd = 1;

	for(i = 0; i < numclause+numresolution; ++i) { 

		clausesResolutions[i] = 0;
	}

	Individual * tmp = population;

	/* For every individual inside the population */
	
	while(tmp != NULL) {
		
		/* For every clauses still false for this individual */

		for(i = 0 ; i < tmp->numfalse; ++i) {
	
			for(j = 0; j < size[tmp->clausesFalse[i]] ; ++j) {

				int c = tmp->clausesFalse[i];

				if( ABS(clause[c][j]) == whichVariable) {

					/* We check if the clause is not already inside the array. */
					for( l = 0; l < k ; ++l) {
						if(clausesResolutions[l] == c) toAdd = 0;
					}

					if(toAdd == 1) {
						clausesResolutions[k] = c;
						++k;
					}
				

					toAdd = 1;
				}
			}
		} 

		/* We go to the next individual of the population */
		tmp = tmp->next;
	}	

	toAdd = 1;

	for(i = 0; i < numresolution; ++i) {

		for(j = 0; j < size[numclause+i]; ++j) {

			if( ABS(clause[numclause+i][j]) == whichVariable) {

				/* We check if the clause is not already inside the array. */
				for( l = 0; l < k ; ++l) {
					if(clausesResolutions[l] == (numclause+i)) toAdd = 0;
				}

				if(toAdd == 1) {

					clausesResolutions[k] = (numclause+i);
					++k;
				}
				

				toAdd = 1;
			}
		}
	}

	sizeC1 = BIG;
	sizeC2 = BIG;

	
	for(i = 0; i < k ; ++i) {

		if( (contains(clausesResolutions[i],whichVariable) == 1) && size[clausesResolutions[i]] < sizeC1) {

			c1 = clausesResolutions[i];
			sizeC1 = size[clausesResolutions[i]];

		}

		if( (contains(clausesResolutions[i],-1*whichVariable) == 1) && size[clausesResolutions[i]] < sizeC2 && clausesResolutions[i] != c1) {

			c2 = clausesResolutions[i];
			sizeC2 = size[clausesResolutions[i]];
		}
	}
	

	/* We store theses computed values inside the array to return it. */
	clausesForResolution[0] = c1;
	clausesForResolution[1] = c2;

	/* We return the result to perform the resolution after. */
	return clausesForResolution;
}

inline int isTautology() {

	int i,j;
	int var;

	/* We will go through all the variable inside the result of our resolution. */
	for(i = 0; i < size[numclause+numresolution-1]; ++i) {

		/* We take the i-th variable */
		var = clause[numclause+numresolution-1][i];
		
		/* And we will check for every other variable inside the clause, if there is var and ~var */
		for(j = 0; j < size[numclause+numresolution-1]; ++j) {
			
			/* If (... var or ~var ... ), then the clause is a tautology, and we don't need it. */
			if(-1*var == clause[numclause+numresolution-1][j] ) 
				return TRUE;
		}
	}

	/* None of the variable inside the clause creates a tautology, the clause is useful. */
	return FALSE;
}


/************************************************************************************************/
/*																								*/
/* performResolutionProof : We will try to prove that the problem has no solution 				*/
/* @param population the population where that we give us the good clauses						*/
/*																								*/
/************************************************************************************************/
inline void performResolutionProof(Individual* population) {

	/* If the previous resolution is a tautology, we will not need this clause... */
	if( isTautology() == TRUE ) {

		 numresolution--;
	}

	/* If have gone to far, we restart everything, we will not find anything in this branch... */
	if(numresolution >= numclause) { 
		
		restart(); 
	}

	/* We will perform the resolution on the most probable variable. */
	int whichVariable 		  = getWhichVariableToPerformResolution(population);
	

	/* We get the 2 causes where we will perform the resolution inside an array */
	int* clausesForResolution = getWhichClausesToPerformResolution(population,whichVariable);


	tabuVariables[nbTabu] = ABS(whichVariable);

	nbTabu = nbTabu+1;

	if(nbTabu >= sizeTabuVar-1) {

		nbTabu = 0;
	}

	/* If we find something interesting, we will perform the resolution on it. */
	if(clausesForResolution[0] != -1 && clausesForResolution[1] != -1) {

		resolution(clausesForResolution[0],clausesForResolution[1],whichVariable);
	}	

	/* We clean the memory space for this 2-spaces array. */
	free(clausesForResolution);
}

/************************************************************************************************/
/*																								*/
/* resoution : This function will perform the resolution rule on 2 clauses 'c1' and 'c2'		*/
/* @param c1 the left member of the resolution rule 											*/
/* @param c2 the right member of the resolution rule 											*/
/* @param whichVariable on which variable we will perform the resolution 						*/
/* @return 0 if the problem is still UNKNOWN and 1 if the problem is UNSAT 						*/
/* At the end, 1 clause will disappear and the 2nd one will be the result of the resolution		*/
/*																								*/
/************************************************************************************************/
inline int resolution(int c1, int c2,int whichVariable) {

	/* In the worst case, the resolution will be just an OR between c1 and c2 */
	long max = (long)(size[c1] + size[c2]);		

	/* We allocate the good size for this futur resolution's result. */
	clause[numclause+numresolution] = (int*)( malloc( sizeof(int)*(unsigned long)max));

	int i, j, k, l;

	/* boolean to know if we need to add or not the value in the result clause. */
	char toAdd = 1;

	/* we will insert the first element in the 0th place of result */
	k = 0;

	/* -------------------------------------------------- */

	/* We will, for every atom inside the C1 claude */
	for( i = 0 ; i < size[c1] ; ++i) {

		/* compare this value with every atom inside the C2 clause */
		for( j = 0 ; j < size[c2] ; ++j) {
			
			/* A v -A is equals to 1, so we don't need it for the resolution. */
			if( ( (clause[c1][i] == -1*clause[c2][j]) && (ABS(clause[c1][i]) == whichVariable) )) {

					toAdd = 0;
			}	
		}

		/* We go through every element inside C2, C1 is not present. */
		if(toAdd == 1) {

			/* We will so, insert it and go for the next element */
			clause[numclause+numresolution][k++] = (char)clause[c1][i];

		}

		/* We refresh the value and go for the next one. */
		toAdd = 1;
	}

	/* -------------------------------------------------- */

	/* We will, for every atom inside the C2 clause */
	for( i = 0 ; i < size[c2] ; ++i) {

		/* compare this value with every atom inside the C1 clause */
		for( j = 0 ; j < size[c1] ; ++j) {
			
			/* A v -A is equals to 1, so we don't need it for the resolution. */
			if( ( (clause[c2][i] == -1*clause[c1][j]) && (ABS(clause[c2][i]) == ABS(whichVariable)) ) ) {

					toAdd = 0;
				
			}
		}

		/* If we considere that we still need to insert, we should if he is not already there. */
		if(toAdd == 1) {

			for(l = 0; l < (int)max ; ++l) {

				/* This value is already inside the result, so don't need to insert it. */
				if(clause[numclause+numresolution][l] == clause[c2][i]) toAdd = 0;
			}
		}

		/* Ok this value passes all the tests, we need to insert it in the result. */
		if(toAdd == 1) {

			/* We insert it and we k++ for the next value. */
			clause[numclause+numresolution][k++] = (char)clause[c2][i];

		}

		/* We treat this value, let's check the next one. */
		toAdd = 1;
	}

	for(i = k; i < max; ++i) {
		clause[numclause+numresolution][i] = 0;
	}

	/* -------------------------------------------------- */

	size[numclause+numresolution] = k;

	++numresolution;

	/* The resolution gives us the empty clause */
	if( k == 0) {

		/* The problem is so UNSAT */
		FOUND = UNSAT;
		return  1;
	}

	if (subsumes(c1,(int)numclause+numresolution-1) == TRUE) {
			
		numresolution--;

	} else if (subsumes(c2,(int)numclause+numresolution-1) == TRUE) {
		
		numresolution--;
	}

	/* -------------------------------------------------- */

	return 0;
}

/************************************************************************************************/
/*																								*/
/* subsumes : Will check if the clause c1 subsumes the clause C2 								*/
/* @param c1 the index of the first clause 														*/
/* @param c2 the index of the second clause 													*/
/* @return TRUE if c1 subsumes c2, FALSE otherwise 												*/
/*																								*/
/************************************************************************************************/
inline int subsumes(int c1, int c2) {

	int i,j;

	if(size[c1] < size[c2]) {
		i = c1;
		c1 = c2;
		c2 = i;
	}

    for (i = 0; i < size[c1]; ++i) {
        
        for (j = 0; j < size[c2]; ++j) {
            
            if(clause[c1][i] != ABS(clause[c2][j])) return FALSE;
    	}
    }

    return TRUE;
}

/************************************************************************************************/
/*																								*/
/* restart : We did "too much" resolution and didn't find anything yet, we restart to find...   */
/*																								*/
/************************************************************************************************/
inline void restart() {

	int i = 0;

	for(i  = 0 ; i < numresolution; i++) {

		free(clause[numclause+i]);		
		size[numclause+i] = 0;	
	}
	
	numresolution = 0;
}
