#include "def.h"
#include <stdbool.h>

double  euclidean(double, double, double, double);
void    error(int);
void    resize(bool);

extern int      tipo;   /* Categoria de Instancia */
extern int      id;     /* ID de Instancia */
extern int      n;      /* numero de nodos */
extern int      p;      /* numero de medianas */
extern int      *b;     /* capacidad */
extern int      **c;    /* matriz de costes */
extern int      *d;     /* vector de demandas */
extern FILE     *fp;
int             **xy;   /* coordenadas*/


void ReadInstance(char *archivo) {
    int     register    i,j;
    int     valor;
    double  value;
    
    fp = fopen(archivo, "r");
    if ( fp == NULL) {
        error(2);
    }
    
    fscanf(fp, "%d",  &valor);
    tipo=valor;
    fscanf(fp, "%d",  &valor);
    id=valor;
    fscanf(fp, "%d",  &valor);
    n=valor;
    fscanf(fp, "%d",  &valor);
    p=valor;
    fscanf(fp, "%lf", &value);
    
    
    //Beasley
    if (tipo==1) {
        //Dimension
        resize(true);
        
        //Capacidad
        fscanf(fp, "%d", &valor);
        for (i=0; i<n; i++) {
            b[i]=valor;
        }
        
        //Demanda y Coordenadas
        for (i=0; i<n; i++) {
            fscanf(fp, "%d", &valor);
            fscanf(fp, "%d", &xy[i][0]);
            fscanf(fp, "%d", &xy[i][1]);
            fscanf(fp, "%d", &d[i]);
        }
        
        //Costos
        for (i=0; i<n; i++)
            for (j=i; j<n; j++) {
                c[i][j]=i!=j?(int)euclidean(xy[i][0],xy[j][0],xy[i][1],xy[j][1]):0;
                c[j][i]=c[i][j];
            }
        
        if (xy != NULL) {
            free (xy);
            xy = NULL;
        }
    }
    
    if (tipo==2) {
        //Dimension
        resize(false);
        
        //Capacidad
        for (i=0; i<n; i++){
            fscanf(fp, "%lf", &value);
            b[i]=(int)value;
        }
        
        //Demanda
        for (i=0; i<n; i++){
            fscanf(fp, "%lf", &value);
            d[i]=(int)value;
        }
        
        //Costos
        for (i=0; i<n; i++)
            for (j=0; j<n; j++){
                fscanf(fp, "%lf", &value);
                c[i][j]=(int)value;
            }
    }
    
    if (tipo==3 || tipo==8) {
        
        //Dimension
        resize(true);
        
        //Demanda y Coordenadas
        for (i=0; i<n; i++) {
            fscanf(fp, "%d", &xy[i][0]);
            fscanf(fp, "%d", &xy[i][1]);
            fscanf(fp, "%d", &b[i]);
            fscanf(fp, "%d", &d[i]);
        }
        
        //Costos
        for (i=0; i<n; i++)
            for (j=i; j<n; j++) {
                c[i][j]=i!=j?(int)euclidean(xy[i][0],xy[j][0],xy[i][1],xy[j][1]):0;
                c[j][i]=c[i][j];
            }
        
        if (xy != NULL) {
            free (xy);
            xy = NULL;
        }
    }
    
    if (tipo>=4 && tipo<=7) {
        
        //Dimension
        resize(false);
        
        //Costos
        for (i=0; i<n; i++)
            for (j=0; j<n; j++){
                fscanf(fp, "%d", &valor);
                c[i][j]=valor;
            }
        
        //Demanda
        for (i=0; i<n; i++){
            fscanf(fp, "%d", &valor);
            d[i]=valor;
        }
        
        //Capacidad
        for (i=0; i<n; i++){
            fscanf(fp, "%d", &valor);
            b[i]=valor;
        }
    }
}


void resize(bool coor)
{
    register    i,j;
    if ( (b = (int *) calloc (n, sizeof(int))) == NULL )
        error(3);
    if ( (d = (int *) calloc (n, sizeof(int))) == NULL )
        error(3);
    
    if(coor){
        if ( (xy = (int **) calloc (n, sizeof(int *))) == NULL )
            error(3);
        for (i=0; i<n; i++)
            if ( (xy[i] = (int *) calloc (2, sizeof(int))) == NULL )
                error(3);
    }
    
    if ( (c = (int **) calloc (n, sizeof(int *))) == NULL )
        error(3);
    
    for (i=0; i<n; i++)
        if ( (c[i] = (int *) calloc (n, sizeof(int))) == NULL )
            error(3);
}

void error(int ErrorNo) {
    if (ErrorNo == 1) {
        printf("\nError: No se especifica archivo de datos\n");
        exit(8);
    }
    if (ErrorNo == 2) {
        printf("\nError: No se pudo abrir el archivo de datos\n");
        exit(8);
    }
    if (ErrorNo == 3) {
        printf("\nError: Memoria insuficiente\n");
        exit(8);
    }
}

double euclidean(double x1, double x2, double y1, double y2)
{
    return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}
