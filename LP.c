#include "def.h"
#include <ilcplex/cplex.h>
#include <string.h>
#include <stdlib.h>

extern int             n;            /* número de nodos */
extern int             p;            /* número de medianas */
extern int             *b;           /* capacidad */
extern int             *capacities;
extern int             **c;          /* matriz de costes */
extern int             *d;           /* vector de demandas */
extern int             *ordrad;
extern ASSVAR          *assignvar;
extern int             varassigncount;
int                    agregatedDemand;


int NUMCOLS,NUMROWS,NUMNZ;

static int CPXPUBLIC
   mycallback (CPXCENVptr env, void *cbdata, int wherefrom, void *cbhandle);

static int
   setproblemdatalp (char **probname_p, int *numcols_p, int *numrows_p, 
                   int *objsen_p, double **obj_p, double **rhs_p, 
                   char **sense_p, int **matbeg_p, int **matcnt_p, 
                   int **matind_p, double **matval_p, 
                   double **lb_p, double **ub_p);

static void
    free_and_null (char **ptr);

double solvelp (int rad)
{
   char     *probname = NULL;  
   int      numcols;
   int      numrows;
   int      objsen;
   double   *obj = NULL;
   double   *rhs = NULL;
   char     *sense = NULL;
   int      *matbeg = NULL;
   int      *matcnt = NULL;
   int      *matind = NULL;
   double   *matval = NULL;
   double   *lb = NULL;
   double   *ub = NULL;
   char     *ctype = NULL;
   int      solstat;
   double   objval;
   CPXENVptr     env = NULL;
   CPXLPptr      lp = NULL;
   int           status;

   NUMCOLS = varassigncount+n;
   NUMROWS = 2*n+varassigncount;
   NUMNZ   = 2*varassigncount+2*varassigncount;
   env = CPXopenCPLEX (&status);

   if ( env == NULL ) {
      char  errmsg[1024];
      fprintf (stderr, "Could not open CPLEX environment.\n");
      CPXgeterrorstring (env, status, errmsg);
      fprintf (stderr, "%s", errmsg);
      goto TERMINATE;
   }
   status = CPXsetintparam (env, CPX_PARAM_SCRIND, CPX_OFF);
   if ( status ) {
      fprintf (stderr, 
               "Failure to turn on screen indicator, error %d.\n", status);
      goto TERMINATE;
   }
   status = setproblemdatalp (&probname, &numcols, &numrows, &objsen, &obj, 
                            &rhs, &sense, &matbeg, &matcnt, &matind, &matval, 
                            &lb, &ub);
   if ( status ) {
      fprintf (stderr, "Failed to build problem data arrays.\n");
      goto TERMINATE;
   }
   lp = CPXcreateprob (env, &status, probname);
   if ( lp == NULL ) {
      fprintf (stderr, "Failed to create LP.\n");
      goto TERMINATE;
   }
   status = CPXcopylp (env, lp, numcols, numrows, objsen, obj, rhs, 
                       sense, matbeg, matcnt, matind, matval,
                       lb, ub, NULL);

   if ( status ) {
      fprintf (stderr, "Failed to copy problem data.\n");
      goto TERMINATE;
   }
   status = CPXlpopt (env, lp);
   if ( status ) {
	   fprintf (stderr, "Failed to optimize MIP.\n");
	   goto TERMINATE;
   }
   solstat = CPXgetstat (env, lp);
   status = CPXgetobjval (env, lp, &objval);
   if ( status ) {
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
   return objval;
}

static int
setproblemdatalp (char **probname_p, int *numcols_p, int *numrows_p, 
                int *objsen_p, double **obj_p, double **rhs_p, 
                char **sense_p, int **matbeg_p, int **matcnt_p, 
                int **matind_p, double **matval_p, 
                double **lb_p, double **ub_p)
{
   int      i,j,r,t;
   int      cuenta,pos;
   char     *zprobname = NULL;     /* Problem name <= 16 characters */        
   double   *zobj = NULL;
   double   *zrhs = NULL;
   char     *zsense = NULL;
   int      *zmatbeg = NULL;
   int      *zmatcnt = NULL;
   int      *zmatind = NULL;
   double   *zmatval = NULL;
   double   *zlb = NULL;
   double   *zub = NULL;
   int      status = 0;
 

   zprobname = (char *) malloc (16 * sizeof(char)); 
   zobj      = (double *) malloc (NUMCOLS * sizeof(double));
   zrhs      = (double *) malloc (NUMROWS * sizeof(double));
   zsense    = (char *) malloc (NUMROWS * sizeof(char)); 
   zmatbeg   = (int *) malloc (NUMCOLS * sizeof(int));   
   zmatcnt   = (int *) malloc (NUMCOLS * sizeof(int));   
   zmatind   = (int *) malloc (NUMNZ * sizeof(int));   
   zmatval   = (double *) malloc (NUMNZ * sizeof(double));
   zlb       = (double *) malloc (NUMCOLS * sizeof(double));
   zub       = (double *) malloc (NUMCOLS * sizeof(double));
 
   if ( zprobname == NULL || zobj    == NULL ||
        zrhs      == NULL || zsense  == NULL ||
        zmatbeg   == NULL || zmatcnt == NULL ||
        zmatind   == NULL || zmatval == NULL ||
        zlb       == NULL || zub     == NULL )  {
      status = 1;
      goto TERMINATE;
   }

   strcpy (zprobname, "example");
   r = 0;
   for (i=0; i<varassigncount; i++) {
	   zobj[i]     = 0;
	   zlb[r]      = 0;
	   zub[r++]    = 1;
   }
   for (i=0; i<n; i++) {
	   zobj[r]    = 1;
	   zlb[r]     = 0;
	   zub[r++]   = 1;
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
	double objval,feasibleval;

	if (wherefrom == CPX_CALLBACK_MIP) {
		status = CPXgetcallbackinfo(env, cbdata, 101, CPX_CALLBACK_INFO_BEST_REMAINING, &objval);
		status = CPXgetcallbackinfo(env, cbdata, 101, CPX_CALLBACK_INFO_BEST_INTEGER, &feasibleval);
	}
	if (objval >= p  || feasibleval <= p) return 1;
	return 0;
}