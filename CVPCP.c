
//@article{ozsoy2006,
//        Author  = {F. Aykut Ozsoy and M. C. Pınar},
//        Journal = {Computers & Operations Research},
//        Number  = {5},
//        Pages   = {1420-1436},
//        Title   = {An exact algorithm for the capacitated vertex p-center problem},
//        Volume  = {33},
//        Year    = {2006},
//    }


#include "def.h"
#include <sys/resource.h>
#include <errno.h>

int             tipo;           /* Categoria de Instancia */
int             id;             /* Id de Instancia */
int             n;              /* n */
int             p;              /* p */
int             *b;             /* capacidad */
int             **c;            /* matriz de costes */
int             *d;             /* vector de demandas */
int             *capacities;
int             *ordrad;
int             agregatedDemand;
int             rad;
FILE            *fp;
ASSRAD          *Cover;
ASSVAR          *assignvar;
int             varassigncount;
float           radius,LBound,UBound;
int             index_inf,index_sup;
int             mini,maxi;
float           lowerBound;
float           optsubgradiente(float, int);
float           HeuristicaPrimal(float, int*, int**, int**, float**);
clock_t         start;
double          cputimeMAX;
int             STATUS;


int comparerad (const void *a, const void *b) {
	int *x, *y;
	x = (int *) a;
	y = (int *) b;
	if ( *x > *y )
		return 1;
	if ( *x < *y )
		return -1;
	return 0;
}

int main(int argc, char *argv[]) {
    
    //---Suppress IBM ILOG License Manager output----//
    int backup, redirection;
    fflush(stderr);                             // Flush stderr
    backup = dup(2);                            // Duplicate FD 2
    redirection = open("/dev/null", O_WRONLY);  // Open file
    dup2(redirection, 2);                       // Duplicate it to FD 2
    close(redirection);                         // Close original
    //-----------------------------------------------//
    
    
	register int i, j, k;
	int      flag;
	int      InitialLowerBound;
	int      LBrad;
	int      Radiost;
	int      l,u,diff;
	int      *ordenados;
	int      countiter;
	int      index;
	double   plantas;
	double   pu,pl;
	ReadInstance(argv[1]);
    cputimeMAX=atof(argv[2]);
    
	ordrad     = (int *) calloc (n*n, sizeof(int));
	ordenados  = (int *) calloc (n, sizeof(int));
	capacities = (int *) calloc(n, sizeof(int));
	agregatedDemand=0;
    
    start=clock();
    
	for (j=0; j<n; j++) agregatedDemand+=d[j];
	k=0;
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			ordrad[k++]=c[i][j];
		}
	}
	qsort((int *)ordrad, n*n, sizeof(int), comparerad);
	j=k=0;
	flag=0;
	do {
		radius=(float) ordrad[j];
		while (j<n*n && (float) ordrad[j]==radius)j++;
		ordrad[k++]=(int) radius;
		if (j==n*n) flag=1;
	} while (!flag);
	Radiost=k;
    
    //Phase I
	l = ordrad[0];
	u = ordrad[k-1];
	countiter = 0;
	while (u-l > 1) {
		diff    = (int) floor((double)(u-l)/2);
		rad     = l+diff;
		varassigncount=0;
		for (i=0; i<n; i++) {
			for (j=0; j<n; j++) {
				if (c[i][j] <= rad) {
					varassigncount++;
				}
			}
		}
		assignvar = (ASSVAR *) calloc(varassigncount, sizeof(ASSVAR));
		k = 0;
		for (i=0; i<n; i++) {
			for (j=0; j<n; j++) {
				if (c[i][j] <= rad) {
					assignvar[k].i=i;
					assignvar[k].j=j;
					k++;
				}
			}
		}
		plantas=solvelp(rad);
		if (plantas - p > EPSILON) {
			l=rad;
			pl=plantas;
		}
		else {
			u=rad;
			pu=plantas;
		}
		free(assignvar);
		countiter++;
	}
	if (plantas - p > EPSILON) {
		rad=u;
	}
	else {
		rad=l;
	}
    
    //printf("Phase I: %d\n",rad);
    
    //Phase II
    
	InitialLowerBound=rad;
	index=0;
	for (k=0; k<Radiost; k++) {
		if (ordrad[index] >= rad) {
			break;
		}
		index++;
	}
	do {
		varassigncount=0;
		for (i=0; i<n; i++) {
			for (j=0; j<n; j++) {
				if (c[i][j] <= rad) {
					varassigncount++;
				}
			}
		}
		assignvar = (ASSVAR *) calloc(varassigncount, sizeof(ASSVAR));
		l = 0;
		for (i=0; i<n; i++) {
			for (j=0; j<n; j++) {
				if (c[i][j] <= rad) {
					assignvar[l].i=i;
					assignvar[l].j=j;
					l++;
				}
			}
		}
		plantas=solve(rad,0);
		if (plantas - p > EPSILON) {
			LBrad=ordrad[index];
			index+=1;
			rad=ordrad[index];
			free(assignvar);
		}
		else {
			break;
		}
	} while(1);
	do {
		varassigncount=0;
		for (i=0; i<n; i++) {
			for (j=0; j<n; j++) {
				if (c[i][j] <= rad) {
					varassigncount++;
				}
			}
		}
		assignvar = (ASSVAR *) calloc(varassigncount, sizeof(ASSVAR));
		l = 0;
		for (i=0; i<n; i++) {
			for (j=0; j<n; j++) {
				if (c[i][j] <= rad) {
					assignvar[l].i=i;
					assignvar[l].j=j;
					l++;
				}
			}
		}
		plantas=solve(rad,1);
		if (plantas - p > EPSILON) {
			index+=1;
			rad=ordrad[index];
			free(assignvar);
		}
		else {
			break;
		}
	} while(1);
    
    print();
   
    //---Suppress IBM ILOG License Manager output----//
    
    fflush(stderr);     /* Flush redirection */
    dup2(backup, 2);    /* Duplicate backup to FD 2 */
    close(backup);      /* Close backup */
    //----------------------------------------------//


    return 0;
}

void print()
{
    struct rusage* memory = malloc(sizeof(struct rusage));
    unsigned long _ru_maxrss;
    if (getrusage(RUSAGE_SELF, memory) == -1)
        _ru_maxrss=-1;
    else
        _ru_maxrss=memory->ru_maxrss;
    
    printf("%d\t%d\t%d\t%d\t%d\t%.4f\t%ld\t%d\t%.4f\n",
           tipo,id,n,p,rad,runTime(),_ru_maxrss,STATUS,cputimeMAX);
    
    if (memory != NULL) {
        free (memory);
        memory = NULL;
    }
}

double runTime()
{
    return (double) (clock()-start)/CLOCKS_PER_SEC;
}
