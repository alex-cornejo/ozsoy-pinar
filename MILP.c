#include "def.h"
#include <ilcplex/cplex.h>
#include <string.h>
#include <stdlib.h>

extern int              n;              /* numero de nodos */
extern int              p;              /* numero de medianas */
extern int              *b;             /* capacidad */
extern int              *capacities;
extern int              **c;            /* matriz de costes */
extern int              *d;             /* vector de demandas */
extern ASSVAR           *assignvar;
extern int              varassigncount;
extern int              STATUS;

extern double           cputimeMAX;
extern size_t           cpumemoryMAX;

int                     agregatedDemand;
double                  Bestobjval,Bestfeasibleval;
int                     number;


int NUMCOLS,NUMROWS,NUMNZ;

static int CPXPUBLIC
   mycallback (CPXCENVptr env, void *cbdata, int wherefrom, void *cbhandle);

static int
   setproblemdata (char **probname_p, int *numcols_p, int *numrows_p, 
                   int *objsen_p, double **obj_p, double **rhs_p, 
                   char **sense_p, int **matbeg_p, int **matcnt_p, 
                   int **matind_p, double **matval_p, 
                   double **lb_p, double **ub_p, char **ctype_p, int type);

static void
    free_and_null (char **ptr);

double solve (int rad, int type)
{
   char     *probname= NULL;  
   int      numcols;
   int      numrows;
   int      objsen;
   double   *obj     = NULL;
   double   *rhs     = NULL;
   char     *sense   = NULL;
   int      *matbeg  = NULL;
   int      *matcnt  = NULL;
   int      *matind  = NULL;
   double   *matval  = NULL;
   double   *lb      = NULL;
   double   *ub      = NULL;
   char     *ctype   = NULL;
   int      solstat;
   double   objval;
   double   *x;
   double   *slack;
   int      *ass;
   int      *fac;
   int      demanda;
   int      *index;
   int      *priority;
   int      *direction;
   int      status=0;
    
   CPXENVptr     env = NULL;
   CPXLPptr      lp  = NULL;

   index=(int *)     calloc (varassigncount+n,sizeof(int));
   priority=(int *)  calloc (varassigncount+n,sizeof(int));
   direction=(int *) calloc (varassigncount+n,sizeof(int));

   ass = (int *) calloc (n,sizeof(int));
   fac = (int *) calloc (n,sizeof(int));

   NUMCOLS = varassigncount+n;
   NUMROWS = 2*n+varassigncount;
   NUMNZ   = 2*varassigncount+2*varassigncount;


   x = (double *) calloc(NUMCOLS, sizeof(double));
   slack = (double *) calloc(NUMROWS, sizeof(double));
   
    if ( x == NULL || slack==NULL ) {
        STATUS = CPXERR_NO_MEMORY;
        fprintf (stderr, "Could not allocate memory for solution.\n");
        goto TERMINATE;
    }
    
   env = CPXopenCPLEX (&STATUS);
   if ( env == NULL ) {
      char  errmsg[1024];
      fprintf (stderr, "Could not open CPLEX environment.\n");
      CPXgeterrorstring (env, STATUS, errmsg);
      fprintf (stderr, "%s", errmsg);
      goto TERMINATE;
   }
    
   STATUS = CPXsetmipcallbackfunc (env, mycallback, NULL);
    if ( STATUS ) {
        fprintf (stderr, "Failed to set callback function.\n");
        goto TERMINATE;
    }
    
   STATUS = CPXsetintparam (env, CPX_PARAM_SCRIND, CPX_OFF);   
   if ( STATUS ) {
      fprintf (stderr, 
               "Failure to turn on screen indicator, error %d.\n", STATUS);
      goto TERMINATE;
   }
   STATUS = setproblemdata (&probname, &numcols, &numrows, &objsen, &obj, 
                            &rhs, &sense, &matbeg, &matcnt, &matind, &matval, 
                            &lb, &ub, &ctype, type);
   if ( STATUS ) {
      fprintf (stderr, "Failed to build problem data arrays.\n");
      goto TERMINATE;
   }
   lp = CPXcreateprob (env, &STATUS, probname);
   if ( lp == NULL ) {
      fprintf (stderr, "Failed to create LP.\n");
      goto TERMINATE;
   }
   STATUS = CPXcopylp (env, lp, numcols, numrows, objsen, obj, rhs, 
                       sense, matbeg, matcnt, matind, matval,
                       lb, ub, NULL);

   if ( STATUS ) {
      fprintf (stderr, "Failed to copy problem data.\n");
      goto TERMINATE;
   }
   STATUS = CPXcopyctype (env, lp, ctype);
   if ( STATUS ) {
      fprintf (stderr, "Failed to copy ctype\n");
      goto TERMINATE;
   }
   STATUS = CPXmipopt (env, lp);
   if ( STATUS ) {
	   fprintf (stderr, "Failed to optimize MIP.\n");
	   goto TERMINATE;
   }
   solstat = CPXgetstat (env, lp);
   STATUS = CPXgetmipobjval (env, lp, &objval);
   if ( STATUS ) {
	   fprintf (stderr,"No MIP objective value available.  Exiting...\n");
	   goto TERMINATE;
   }
   
TERMINATE:
    if ( lp != NULL ) {
        status = CPXfreeprob (env, &lp);
        if ( status ) {
            fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
        }
    }

    if ( env != NULL ) {
        status = CPXcloseCPLEX (&env);
        if ( status ) {
            char  errmsg[1024];
            fprintf (stderr, "Could not close CPLEX environment.\n");
            CPXgeterrorstring (env, status, errmsg);
            fprintf (stderr, "%s", errmsg);
        }
   }
   
   free_and_null ((char **) &probname);
   free_and_null ((char **) &obj);
   free_and_null ((char **) &rhs);
   free_and_null ((char **) &sense);
   free_and_null ((char **) &matbeg);
   free_and_null ((char **) &matcnt);
   free_and_null ((char **) &matind);
   free_and_null ((char **) &matval);
   free_and_null ((char **) &lb);
   free_and_null ((char **) &ub);
   free_and_null ((char **) &ctype);
   free_and_null ((char **) &x);
   free_and_null ((char **) &slack);
   free_and_null ((char **) &index);
   free_and_null ((char **) &priority);
   free_and_null ((char **) &direction);
   free_and_null ((char **) &ass);
   free_and_null ((char **) &fac);

   return objval;
}  



static int setproblemdata (char **probname_p, int *numcols_p, int *numrows_p, 
                int *objsen_p, double **obj_p, double **rhs_p, 
                char **sense_p, int **matbeg_p, int **matcnt_p, 
                int **matind_p, double **matval_p, 
                double **lb_p, double **ub_p, char **ctype_p, int type)
{
   int      i,j,r,t;
   int      cuenta,pos;
   char     *zprobname  = NULL;     /* Problem name <= 16 characters */        
   double   *zobj       = NULL;
   double   *zrhs       = NULL;
   char     *zsense     = NULL;
   int      *zmatbeg    = NULL;
   int      *zmatcnt    = NULL;
   int      *zmatind    = NULL;
   double   *zmatval    = NULL;
   double   *zlb        = NULL;
   double   *zub        = NULL;
   char     *zctype     = NULL;
   int      status = 0;
 

   zprobname = (char *)     malloc (16      * sizeof(char)); 
   zobj      = (double *)   malloc (NUMCOLS * sizeof(double));
   zrhs      = (double *)   malloc (NUMROWS * sizeof(double));
   zsense    = (char *)     malloc (NUMROWS * sizeof(char)); 
   zmatbeg   = (int *)      malloc (NUMCOLS * sizeof(int));   
   zmatcnt   = (int *)      malloc (NUMCOLS * sizeof(int));   
   zmatind   = (int *)      malloc (NUMNZ   * sizeof(int));   
   zmatval   = (double *)   malloc (NUMNZ   * sizeof(double));
   zlb       = (double *)   malloc (NUMCOLS * sizeof(double));
   zub       = (double *)   malloc (NUMCOLS * sizeof(double));
   zctype    = (char *)     malloc (NUMCOLS * sizeof(char)); 
 
   if ( zprobname == NULL || zobj    == NULL ||
        zrhs      == NULL || zsense  == NULL ||
        zmatbeg   == NULL || zmatcnt == NULL ||
        zmatind   == NULL || zmatval == NULL ||
        zlb       == NULL || zub     == NULL ||
        zctype    == NULL                       )  {
      status = CPXERR_NO_MEMORY;
      goto TERMINATE;
   }

   strcpy (zprobname, "example");

   r = 0;
   for (i=0; i<varassigncount; i++) {
	   zobj[i]     = 0;
	   zlb[r]      = 0;
	   zub[r]      = 1;
	   if (type==0)
		   zctype[r++] = 'C';
	   else 
		   zctype[r++] = 'B';
   }
   for (i=0; i<n; i++) {
	   zobj[r]    = 1;
	   zlb[r]     = 0;
	   zub[r]     = 1;
	   if (type==0) 
		   zctype[r++] = 'B';
	   else
		   zctype[r++] = 'B';
   }

   t=0;
   for(i=0;i<n;i++) {
	   zsense[t]   = 'E';
	   zrhs[t++]   = 1;
   }
   for (i=0; i<n; i++) {
	   zsense[t]   = 'L';
	   zrhs[t++]   = b[i];
   }
 
   for (i=0; i<varassigncount; i++) {
	   zsense[t]   = 'L';
	   zrhs[t++]   = 0;
   }

   t=0;
   r=0;
   cuenta=0;
   for (i=0; i<varassigncount; i++) {
	   zmatval[t]       = 1;
	   zmatind[t++]     = assignvar[i].j;
	   zmatval[t]       = d[assignvar[i].j];
	   zmatind[t++]     = n+assignvar[i].i;
	   zmatval[t]       = 1;
	   zmatind[t++]     = 2*n+i;
	   zmatbeg[r]       = cuenta;
	   zmatcnt[r++]     = 3;
	   cuenta+=3;
   }

   pos=cuenta;
   for (i=0; i<n; i++) {
       cuenta=0;
	   for (j=0; j<varassigncount; j++) {
		   if (assignvar[j].i == i) {
			   zmatval[t]   = -1;
			   zmatind[t++] = 2*n+j;
			   cuenta++;
		   }
	   }
	   zmatbeg[r]     = pos;
	   zmatcnt[r++]   = cuenta;
	   pos+=cuenta;
   }

TERMINATE:

   if ( status ) {
      free_and_null ((char **) &zprobname);
      free_and_null ((char **) &zobj);
      free_and_null ((char **) &zrhs);
      free_and_null ((char **) &zsense);
      free_and_null ((char **) &zmatbeg);
      free_and_null ((char **) &zmatcnt);
      free_and_null ((char **) &zmatind);
      free_and_null ((char **) &zmatval);
      free_and_null ((char **) &zlb);
      free_and_null ((char **) &zub);
      free_and_null ((char **) &zctype);
       
   }
   else {
      *numcols_p   = NUMCOLS;
      *numrows_p   = NUMROWS;
      *objsen_p    = CPX_MIN;   /* The problem is minimization */
   
      *probname_p  = zprobname;
      *obj_p       = zobj;
      *rhs_p       = zrhs;
      *sense_p     = zsense;
      *matbeg_p    = zmatbeg;
      *matcnt_p    = zmatcnt;
      *matind_p    = zmatind;
      *matval_p    = zmatval;
      *lb_p        = zlb;
      *ub_p        = zub;
      *ctype_p     = zctype;
   }
   return (status);

}

static void free_and_null (char **ptr)
{
    if ( *ptr != NULL ) {
        free (*ptr);
        *ptr = NULL;
    }
}


static int CPXPUBLIC mycallback (CPXCENVptr env, void *cbdata, int wherefrom, void *cbhandle) {
    int status=0;

	if (wherefrom == CPX_CALLBACK_MIP) {
		status = CPXgetcallbackinfo(env, cbdata, 101, 
                                    CPX_CALLBACK_INFO_BEST_REMAINING, &Bestobjval);
		status = CPXgetcallbackinfo(env, cbdata, 101, 
                                    CPX_CALLBACK_INFO_BEST_INTEGER, &Bestfeasibleval);
		status = CPXgetcallbackinfo(env, cbdata, 101, 
                                    CPX_CALLBACK_INFO_NODE_COUNT, &number);
	}
   
    if (runTime() > cputimeMAX)
    {
        STATUS = CPX_STAT_ABORT_TIME_LIM;
        print();
	    exit(8);
	}
    
    if (runMemory() > cpumemoryMAX) {
        STATUS = CPXMIP_MEM_LIM_FEAS;
        print();
	    exit(8);
    }
    
	if (Bestobjval - p > EPSILON  || Bestfeasibleval - p <= EPSILON)
        return 1;
    
	return 0;
}
