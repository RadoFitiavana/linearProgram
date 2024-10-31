#include "linear_prog_int.h"
#include <stdlib.h>
#include <stdio.h>

void display(linP_int *Lp) {
    printf("z ") ;
    for (int j=0; j<Lp->n_var+2; j++) {
        printf("%d ",Lp->z[j]) ;
    }
    printf("\n") ;
    for (int i=0; i<Lp->n_const; i++) {
        printf("  ") ;
        for (int j=0; j<=Lp->n_var; j++) {
            printf("%d ",Lp->prog[i][j]) ;
        }
        printf("\n") ;
    }
    if (Lp->w!=NULL) {
        printf("w ") ;
        for (int j=0; j<=Lp->n_var; j++) {
            printf("%d ",Lp->w[j]) ;
        }
    }
    printf("\n") ;    
}

void free_linP(linP_int *Lp) {
    if (Lp != NULL) {
        if (Lp->prog != NULL) {
            free(Lp->prog);  // Free the program memory
            Lp->prog = NULL; // Set it to NULL to avoid double free
        }
        if (Lp->x != NULL) {
            free(Lp->x);    // Free the decision variables memory
            Lp->x = NULL;   // Avoid double free
        }
        if (Lp->z!=NULL){
            free(Lp->z) ;
            Lp->z = NULL ;
        }
        free(Lp);  // Free the structure itself
        Lp = NULL;
    }
}

int gcd(int *a, int *b) {
    if (*a==0 || *b==0) {
        if (*a==0) {
            return *b ;
        }
        else {
            return *a ;
        }
    }
    else {
        int r = *a % *b ;
        while (r>0) {
            *a = *b ;
            *b = r ;
            r = *a % *b ;
        }
        return *b ;
    }
}

void factor(int *x, int *y, int *a, int *b) {
    *a = (*x >= 0)? *x : -*x ;
    *b = (*y >= 0)? *y : -*y ;
    int r = *a % *b ;
    *b = gcd(a,b) ;
    *a = *y / *b ;
    *b = *x / *b ;
}

void reduce(int *tab, int n) {
    int a = (tab[0]>=0)? tab[0] : -tab[0] ;
    int b = (tab[1]>=0)? tab[1] : -tab[1] ;
    int g = gcd(&a,&b) ;
    for (int j=2; j<n; j++) {
        a = g ;
        b = (tab[j]>=0)? tab[j] : -tab[j] ;
        g = gcd(&a,&b) ;
    }
    if (g>1) {
        # pragma omp parallel shared(tab)
        {
            # pragma omp for
            for (int j=0; j<n; j++) {
                tab[j] /= g ;
            }
            #pragma barrier
        }
    }
}

linP_int* createProgByTab(int n_var,int n_const,int n_dec,int *z,int **prog, int minmax) {
    linP_int *Lp = (linP_int*)malloc(sizeof(linP_int)) ;
    Lp->minmax = minmax ;
    Lp->n_var = n_var ;
    Lp->n_const = n_const ;
    Lp->z = z ;
    Lp->prog = prog ;
    Lp->w = NULL ; 
    Lp->x = NULL ;
    return Lp ;
}

linP_int* createProgByFile(char *fileName, int minmax) {
    FILE *f = fopen(fileName,"r") ;
    if (!f) {
        printf("An error occured while opening %s",fileName) ;
        return NULL ;
    }
    else {
        linP_int *Lp = (linP_int*)malloc(sizeof(linP_int)) ;
        Lp->minmax = minmax ;
        fscanf(f,"%d %d %d",&(Lp->n_var),&(Lp->n_const),&(Lp->n_dec)) ;
        Lp->z = (int*)calloc(Lp->n_var+2,sizeof(int)) ;
        if (Lp->minmax==1){
            for (int j=0; j<Lp->n_var+1; j++) {
                fscanf(f,"%d",&(Lp->z[j])) ;
            }    
        }
        else {
            for (int j=0; j<Lp->n_var+1; j++) {
                fscanf(f,"%d",&(Lp->z[j])) ;
                Lp->z[j] = -Lp->z[j] ;
            }
        }
        fscanf(f,"%d",&(Lp->z[Lp->n_var+1])) ;
        Lp->prog = (int**)calloc(Lp->n_const,sizeof(int*)) ;
        for (int i=0; i<(Lp->n_const); i++) {
            Lp->prog[i] = (int*)calloc(Lp->n_var+1,sizeof(int)) ;
            for (int j=0; j<=(Lp->n_var); j++) {
                fscanf(f,"%d",&(Lp->prog[i][j])) ;
            }
        }
        fclose(f) ;
        Lp->w = NULL ; 
        Lp->x = NULL ;
        Lp->piv = NULL ;
        display(Lp) ;
        printf("\n") ;
        return Lp ;
    }
}

int checkBases(linP_int *Lp) {
    int n_virtual = 0 ;
    int n_slack = Lp->n_var - Lp->n_dec ;
    if (n_slack == 0){
        n_virtual = Lp->n_const ;
    }
    else {
        int *tmp = NULL ;
        int pos ;
        if (n_slack == Lp->n_const){
            pos = Lp->n_const-1 ;
            for (int i=pos; i>=0; i--){
                if (Lp->prog[i][Lp->n_dec+i]<0){
                    tmp = Lp->prog[i] ;
                    Lp->prog[i] = Lp->prog[pos] ;
                    Lp->prog[pos] = tmp ;
                    tmp = NULL ;
                    pos -= 1 ;
                    n_virtual += 1 ;
                }
                else {
                    Lp->piv = (int*)calloc(Lp->n_const,sizeof(int)) ;
                    Lp->piv[i] = Lp->n_dec+i ;
                }
            }
        }
        else {
            pos = Lp->n_const-1 ;
            // Place pure virtuals first
            int n_zeros ;
            for (int i=Lp->n_const-1; i>=0; i--){
                n_zeros = 0 ;
                for (int j=Lp->n_dec; j<Lp->n_var; j++){
                    if (Lp->prog[i][j]==0){
                        n_zeros+=1 ;
                    }
                }
                if (n_zeros==n_slack){
                    tmp = Lp->prog[i] ;
                    Lp->prog[i] = Lp->prog[pos] ;
                    Lp->prog[pos] = tmp ;
                    tmp = NULL ;
                    pos -= 1 ;
                    n_virtual += 1 ;

                }
            }
            // Place virtuals 
            for (int i=pos; i>=0; i--){
                if (Lp->prog[i][Lp->n_dec+i]<0){
                    tmp = Lp->prog[i] ;
                    Lp->prog[i] = Lp->prog[pos] ;
                    Lp->prog[pos] = tmp ;
                    tmp = NULL ;
                    pos -= 1 ;
                    n_virtual += 1 ;
                }
                else {
                    Lp->piv = (int*)calloc(Lp->n_const,sizeof(int)) ;
                    Lp->piv[i] = Lp->n_dec+i ;
                }
            }            
        }
    }

    if (n_virtual > 0) {
        Lp->w = (int*)calloc(Lp->n_var+1,sizeof(int)) ;
        for (int j=0; j<=Lp->n_var; j++) {
            Lp->w[j] = Lp->prog[Lp->n_const-1][j] ;
        }
        for (int i=Lp->n_const-2; i>=Lp->n_const-n_virtual; i--) {
            for (int j=0; j<=Lp->n_var; j++) {
                Lp->w[j] += Lp->prog[i][j] ;
            }
        }
    }
    return n_virtual ;
}

int compare(linP_int *Lp, int *i, int *u, int *j) {
    // int n1 = Lp->prog[*i][Lp->n_var]*Lp->prog[*u][*j] ;
    // int n2 = Lp->prog[*u][Lp->n_var]*Lp->prog[*i][*j] ;
    // return ((n1<n2)? 1 : 0 ) ;

    return (((Lp->prog[*i][Lp->n_var]*Lp->prog[*u][*j])<(Lp->prog[*u][Lp->n_var]*Lp->prog[*i][*j]))? 1 : 0) ;
}

void setPivot(linP_int *Lp, int *pi, int *pj, int *n_cost, int *bounded) {
    *n_cost = 0 ;
    *bounded=1 ;
    for (int j=0; j<Lp->n_var; j++){
        if (Lp->z[j]>0){
            *n_cost = 1 ;
            *pj = j ;
            int cpt = Lp->n_const ;
            for (int i=0; i<Lp->n_const; i++){
                if (Lp->prog[i][*pj]>0){
                    *pi = i ;
                    *bounded = 1 ;
                    cpt -= 1 ;
                    break ;
                }
            }
            if (cpt==Lp->n_const){
                *bounded = 0 ;
                break ;
            }
        }
    }
    if (*n_cost && *bounded) {
        for (int i=*pi+1; i<Lp->n_const; i++){
            if (Lp->prog[i][*pj]>0 && compare(Lp,&i,pi,pj)) {
                *pi = i ;
            }
        }
    }
}

int solveVirtual(linP_int* Lp, int n_virtual) {
    int n_cost = 0 ;
    int pi=0, pj=0 ;
    int a,b ;
    for (pj=0; pj<Lp->n_var; pj++) {
        if (Lp->w[pj] > 0) {
            n_cost = 1 ;
            break ;
        }
    }    
    while (n_cost) {
        for (int i=Lp->n_const-1; i>=0; i--) {
            if (Lp->prog[i][pj]>0) {
                pi = i ;
                break ;
            }
        }
        for (int i=pi-1; i>=0; i--) {
            if (Lp->prog[i][pj]>0 && compare(Lp,&i,&pi,&pj)) {
                pi = i ;
            }
        }
        printf("(%d,%d)\n",pi+1,pj+1) ;
        Lp->piv[pi] = pj ;
        factor(&Lp->z[pj],&Lp->prog[pi][pj],&a,&b) ;

        //#pragma omp parallel
        //{
            #pragma omp for
            for (int j=0; j<=Lp->n_var; j++) {
                Lp->z[j] *= a ;
                Lp->z[j] -= b*Lp->prog[pi][j] ;
            }
            #pragma omp barrier
        //}
        Lp->z[Lp->n_var+1] *= a ;
        reduce(Lp->z,Lp->n_var+2) ;

        //#pragma omp parallel
        //{
            //#pragma omp for
            for (int i=0; i<pi; i++) {
                //#pragma omp critical
                //{
                    factor(&Lp->prog[i][pj],&Lp->prog[pi][pj],&a,&b) ;
                    for (int j=0; j<=Lp->n_var; j++) {
                        Lp->prog[i][j] *= a ;
                        Lp->prog[i][j] -= b*Lp->prog[pi][j] ;
                    }
                    reduce(Lp->prog[i],Lp->n_var+1) ;
                //}
            }
            for (int i=pi+1; i<Lp->n_const; i++) {
                //#pragma omp critical
                //{
                    factor(&Lp->prog[i][pj],&Lp->prog[pi][pj],&a,&b) ;
                    for (int j=0; j<=Lp->n_var; j++) {
                        Lp->prog[i][j] *= a ;
                        Lp->prog[i][j] -= b*Lp->prog[pi][j] ;
                    }
                    reduce(Lp->prog[i],Lp->n_var+1) ;
                //}
            }
            //#pragma barrier
        //}

        factor(&Lp->w[pj],&Lp->prog[pi][pj],&a,&b) ;
        for (int j=0; j<=Lp->n_var; j++) {
            Lp->w[j] *= a ;
            Lp->w[j] -= b*Lp->prog[pi][j] ;
        }
        reduce(Lp->w,Lp->n_var+1) ;
        display(Lp) ;
        printf("\n") ;
        n_cost = 0 ;
        for (pj=0; pj<Lp->n_var; pj++) {
            if (Lp->w[pj] > 0) {
                n_cost = 1 ;
                break ;
            }
        }
    }

    display(Lp) ;
    printf("\n") ;
     
    if (Lp->w[Lp->n_var] == 0) {
        free(Lp->w) ;
        Lp->w = NULL ;
        return 1 ;
    }
    else {
        free(Lp->w) ;
        Lp->w = NULL ;
        return 0 ;
    }
}

void solveFinal(linP_int *Lp) {
    int pi,pj,a,b, n_cost, bounded ;
    setPivot(Lp,&pi,&pj,&n_cost,&bounded) ;
    while (n_cost && bounded) {
        printf("(%d,%d)\n",pi+1,pj+1) ;
        Lp->piv[pi] = pj ;
        factor(&Lp->z[pj],&Lp->prog[pi][pj],&a,&b) ;
        for (int j=0; j<=Lp->n_var; j++) {
            Lp->z[j] *= a ;
            Lp->z[j] -= b*Lp->prog[pi][j] ;
        }
        Lp->z[Lp->n_var+1] *= a ;
        reduce(Lp->z,Lp->n_var+2) ; 

        for (int i=0; i<pi; i++) {
            factor(&Lp->prog[i][pj],&Lp->prog[pi][pj],&a,&b) ;
            for (int j=0; j<=Lp->n_var; j++) {
                Lp->prog[i][j] *= a ;
                Lp->prog[i][j] -= b*Lp->prog[pi][j] ;
            }
            reduce(Lp->prog[i],Lp->n_var+1) ;
        }

        for (int i=pi+1; i<Lp->n_const; i++) {
            factor(&Lp->prog[i][pj],&Lp->prog[pi][pj],&a,&b) ;
            for (int j=0; j<=Lp->n_var; j++) {
                Lp->prog[i][j] *= a ;
                Lp->prog[i][j] -= b*Lp->prog[pi][j] ;
            }
            reduce(Lp->prog[i],Lp->n_var+1) ;
        }

        display(Lp) ;
        printf("\n") ;

        setPivot(Lp,&pi,&pj,&n_cost,&bounded) ;
    }
    display(Lp) ;
    printf("\n") ;
    if (bounded){
        for (int i=0; i<Lp->n_const; i++){
            printf("(%d,%d)\n",i+1,Lp->piv[i]+1) ;
        }
        Lp->x = (double*)calloc(Lp->n_dec,sizeof(double)) ;
        printf("Solutions found\n") ;
        int tem ;
        for (int j=0; j<Lp->n_dec; j++){
            tem = 0 ;
            for (int i=0; i<Lp->n_const; i++){
                if (j==Lp->piv[i]){
                    //printf("(%d,%d)\n",i+1,j+1) ;
                    tem = 1 ;
                    Lp->x[j] = (double)Lp->prog[i][Lp->n_var]/(double)Lp->prog[i][j] ;
                    if ((Lp->prog[i][Lp->n_var])%(Lp->prog[i][j])==0){
                        printf("x%d = %d\n",j+1,(Lp->prog[i][Lp->n_var]/Lp->prog[i][j])) ;
                    }
                    else {
                        printf("x%d = %d/%d\n",j+1,Lp->prog[i][Lp->n_var],Lp->prog[i][j]) ;
                    }
                    break ;
                }
            }
            if (!tem){
                Lp->x[j] = 0.0 ;
                printf("x%d = 0\n",j+1) ;
            }
        }
        if (Lp->minmax==1){
            Lp->optim = (double)Lp->z[Lp->n_var]/(double)Lp->z[Lp->n_var+1] ;
            if (Lp->z[Lp->n_var+1]==1 || Lp->z[Lp->n_var+1]==-1){
                printf("z_max = %d\n",Lp->z[Lp->n_var]*Lp->z[Lp->n_var+1]) ;
            }
            else {
                int s = (Lp->z[Lp->n_var+1]>0)? 1 : -1 ;
                printf("z_max = %d/%d\n",s*Lp->z[Lp->n_var],s*Lp->z[Lp->n_var+1]) ;
            }
        }
        else {
            Lp->optim = - (double)Lp->z[Lp->n_var]/(double)Lp->z[Lp->n_var+1] ;
            if (Lp->z[Lp->n_var+1]==1 || Lp->z[Lp->n_var+1]==-1){
                printf("z_min = %d\n",-Lp->z[Lp->n_var]*Lp->z[Lp->n_var+1]) ;
            }
            else {
                int s = (Lp->z[Lp->n_var+1]>0)? 1 : -1 ;
                printf("z_min = %d/%d\n",-s*Lp->z[Lp->n_var],s*Lp->z[Lp->n_var+1]) ;
            }            
        }
    }
    free(Lp->z) ;
    Lp->z = NULL ;
}

double* solve(linP_int *Lp) {
    int n = checkBases(Lp) ;
    int solvable = 1 ;
    if (n > 0) {
        if (Lp->piv == NULL) {
            Lp->piv = (int*)calloc(Lp->n_const,sizeof(int)) ;
        }        
        display(Lp) ;
        printf("\n") ;
        solvable = solveVirtual(Lp,n) ;
    }
    if (solvable == 1) {
        if (Lp->piv == NULL) {
            Lp->piv = (int*)calloc(Lp->n_const,sizeof(int)) ;
        }
        solveFinal(Lp) ;
        if (Lp->x == NULL){
            printf("No bounded solutions\n") ;
        }
    }
    else {
        printf("No solutions\n") ;
    }
    if (Lp->piv != NULL){
        free(Lp->piv) ;
        Lp->piv = NULL ;
    }
    for (int i=0; i<Lp->n_const; i++){
        free(Lp->prog[i]) ;
        Lp->prog[i] = NULL ;
    }
    free(Lp->prog) ;
    Lp->prog = NULL ;
    return Lp->x ;
}

int main(){
    linP_int *Lp = createProgByFile("test.txt",1) ;
    solve(Lp) ;
    free_linP(Lp) ;
    return 0 ;
}
