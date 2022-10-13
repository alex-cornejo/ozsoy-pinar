#Detecting OS and Architecture
UNAME_S := $(shell uname -s)

CC  = gcc -O3

ifeq ($(UNAME_S), Darwin)
    SYSTEM        = x86-64_darwin
    LIBFORMAT     = static_pic
    CPLEXDIR      = /Users/dago/Applications/IBM/ILOG/CPLEX_Studio125/cplex
    CONCERTDIR    = /Users/dago/Applications/IBM/ILOG/CPLEX_Studio125/concert
endif

ifeq ($(UNAME_S), Linux)
    CC+= -lrt
    
    SYSTEM        = x86-64_sles10_4.1
    LIBFORMAT     = static_pic
    CPLEXDIR      = /usr/local/CPLEX_Studio_125/cplex
    CONCERTDIR    = /usr/local/CPLEX_Studio_125/concert
endif


CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)    
CLNFLAGS      = -L$(CPLEXLIBDIR) -lcplex -m64 -lm -lpthread
COPT          = -m64 -fPIC -fno-strict-aliasing


all:
	make CVPCP
    

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CFLAGS        = $(COPT) -I$(CPLEXINCDIR)

clean :
	/bin/rm -rf *.o *~ *.class
	/bin/rm -rf CVPCP
	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp
    

CVPCP: CVPCP.o
	$(CC) $(CFLAGS) CVPCP.o ReadInstance.c LP.c MILP.c -o CVPCP $(CLNFLAGS) -w

CVPCP.o: main.c ReadInstance.c LP.c MILP.c def.h
	$(CC) -c $(CFLAGS) main.c -o CVPCP.o
