#ifndef LINEAR_PROG_INT_H
#define LINEAR_PROG_INT_H

typedef struct {
/* A linear program data structure that includes:
   the number of variables (decisions + slacks), 
   the number of constraints, 
   the program written in standard table form, 
   and the solutions 
*/
    int minmax ;
    int n_var ;
    int n_const ;
    int n_dec ;
    int *z ;
    int **prog ;
    int *w ;  
    double *x ;
    double optim ;
    int *piv ;

} linP_int ;

void free_linP(linP_int*) ;


linP_int* createProgByShape(int,int,int,int) ;
linP_int* createProgByTab(int,int,int,int*,int**,int) ;
linP_int* createProgByFile(char*,int) ;

int checkBases(linP_int*) ;
void setPivot(linP_int*,int*,int*,int*,int*) ;

int solveVirtual(linP_int*,int) ;
void solveFinal(linP_int*) ;
double* solve(linP_int*) ;

void factor(int*,int*,int*,int*) ;
int compare(linP_int*,int*,int*,int*) ;
void reduce(int*,int) ;
int gcd(int*,int*) ;

void display(linP_int*) ;

#endif