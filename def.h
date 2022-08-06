#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#define getrandom( min, max ) ((rand() % (int) (((max)+1)-(min)))+(min))

#define TRUE 1
#define FALSE 0
#define NOT_AVAIL -1
#define NONE -1
#define EPSILON 0.0000001

typedef struct ASSVAR {
	int i;
	int j;
} ASSVAR;

typedef struct DIST {
  int node;
  int demand;
} DIST;

typedef struct ASSRAD {
  int    node;
  int    count;
  DIST   *Nodes;
} ASSRAD;

void error(int);
void ReadInstance(char *);

double solve(int, int);
double solvelp(int);

void print();
double runTime();