# An exact algorithm for the capacitated vertex <em>p</em>-center problem

This repository contains the computational implementations of the exact method proposed in ["An exact algorithm for the capacitated vertex 
em>p</em>-center problem"](https://doi.org/10.1016/j.cor.2004.09.035), you can find there a formal definition and a experimental approach about this.

## Prerequisites

This implementation was developed in C++ and run under a Unix-like OS. You must verify that you have install the following tools:

* GNU C++
* Make
* Git
* CPLEX

If you don't have this tools, then install the packages build-essential and git.

## Compilation

Use git to download the latest version of the package to your home `git` folder (or elsewhere), and then make it:

```
$ cd ~/git
$ git clone https://github.com/dagoquevedo/ozsoy-pinar
$ cd ozsoy-pinar
$ make
```

The `make` command will compile the code into an executable named `CVPCB`. Before to use the `make` command, you need to update the paths in the `Makefile`, i.n. CPLEX installation path.



