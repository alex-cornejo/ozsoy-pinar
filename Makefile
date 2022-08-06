#Notas
#Compilacion de toda la libreria def.h
#ar -rcs libdef.a ReadInstance.o LP.o MILP.o 

SYSTEM     = x86_darwin
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------

CPLEXDIR      = /Users/dago/Applications/IBM/ILOG/CPLEX_Studio125/cplex
CONCERTDIR    = /Users/dago/Applications/IBM/ILOG/CPLEX_Studio125/concert
# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CC  = gcc -O0


# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

COPT  = -m32 -fPIC -fno-strict-aliasing

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXJARDIR   = $(CPLEXDIR)/lib/cplex.jar
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)


CLNFLAGS  = -L$(CPLEXLIBDIR) -lcplex -m32 -lm -lpthread -framework CoreFoundation -framework IOKit -fno-strict-aliasing


all:
	make CVPCP
    

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CFLAGS  = $(COPT)  -I$(CPLEXINCDIR)

# ------------------------------------------------------------

clean :
	/bin/rm -rf *.o *~ *.class
	/bin/rm -rf CVPCP
	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp
    

# ------------------------------------------------------------
#
# The examples

CVPCP.o: CVPCP.c LP.c MILP.c ReadInstance.c
	$(CC) -c $(CFLAGS) CVPCP.c -o CVPCP.o
CVPCP: CVPCP.o
	$(CC) -c $(CFLAGS) ReadInstance.c -o lib/ReadInstance.o
	$(CC) -c $(CFLAGS) LP.c -o lib/LP.o
	$(CC) -c $(CFLAGS) MILP.c -o lib/MILP.o
	ar rcs lib/libdef.a lib/ReadInstance.o lib/LP.o lib/MILP.o
	$(CC) $(CFLAGS) CVPCP.o -o CVPCP $(CLNFLAGS) -L./lib -ldef

# Local Variables:
# mode: makefile
# End:
