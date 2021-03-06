/*
 * TimeTable - Valentin Montmirail - Polytech Tours, France
 *  Copyright (c) 2015.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 * OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "htmlSolutionGenerator.h"
#include <time.h>

 int main(int argc, char* argv[]) {

    char* filename;
    char*   output;
    char*   solver;
    char*     html;

    char cmd[255];

    Planning * planning;

    double generationTime;

    clock_t begin;
    clock_t end;

    /* We will log a lot of informations in this output. */
    FILE* log = stdout; 
          //log = fopen("display.log","w+");

    if(argc > 2) {
        
        filename = argv[1];

        solver   = argv[2];

    } else {
        
        fprintf(stderr,"You should give in input the plan file and the wanted solver.\n");
        fprintf(stderr,"The solver should also display the result exactly as it should : \n");
        fprintf(stderr,"s [UN]SATISFIABLE");
        fprintf(stderr,"v {{i,-i}}\n\n");
        
        fprintf(stderr,"The execution should be like this : ./timetable file.input path-to-solver\n");
        
        exit(0);
    }

    fprintf(log,"\n[INFO] - We will read the file %s to create our Planning problem.\n",filename);

    begin = clock();

    planning = readInputFile(filename);

    initializeAllTeachers(planning);

    fprintf(log,"[INFO] - We will now create our CNF file.");
    
    output = createCNF(planning);

    fprintf(log,"[INFO] - The output file is now : %s\n",output);
    fprintf(log,"[INFO] - We will now try to solve the CNF file\n");

    strcat(cmd,solver);
    strcat(cmd," ");
    strcat(cmd,output);
    strcat(cmd," > solution.out");

    fprintf(log,"[INFO] - commande : %s",cmd);

    system(cmd);

    unsigned int* solution = getSolutionSchedule(planning,"solution.out");
    
    end = clock();

    /* We display the solution on the standard output stream. */
    displaySolutionSchedule(stdout,planning,solution);

    generationTime = ((end-begin)/(CLOCKS_PER_SEC/1000));
    
    html = createHTMLplan(planning, solution,generationTime,filename);

    fprintf(log,"[INFO] - The HTML plan is now inside the file : %s\n",html);

    fprintf(log,"[INFO] - The program is now finished, you got the solution (or the proof that there is none).\n\n");
    
    fclose(log);
    
    return 0;
}
