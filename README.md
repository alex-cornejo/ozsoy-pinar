# An exact algorithm for the capacitated vertex <em>p</em>-center problem

This repository contains the computational implementations of the exact method proposed in ["An exact algorithm for the capacitated vertex 
<em>p</em>-center problem"](https://doi.org/10.1016/j.cor.2004.09.035) by Özsoy & Pınar, you can find there the formal definition.

## Prerequisites

This implementation was developed in C++ and run under a Unix-like OS and use CPLEX solver. You must verify that you have install the following tools:

* GNU C++
* Make
* Git
* CPLEX

If you don't have this tools, then install the packages `build-essential` and `git`.

## Compilation

Use git to download the latest version of the package to your home `git` folder (or elsewhere), and then make it:

```
$ cd ~/git
$ git clone https://github.com/dagoquevedo/ozsoy-pinar
$ cd ozsoy-pinar
$ make
```

The `make` command will compile the code into an executable named `CVPCP`. Before to use the `make` command, you need to update the paths in the `Makefile`, i.e. CPLEX installation path.

## Dataset format

This implementation use the same dataset format defined in [Heuristic for the Capacitated Vertex p-Center Problem](https://github.com/dagoquevedo/cvpcp).

## Execution

For execute the heuristic method, run the following:
```
$ ./CVPCP {file} {max_time}
```
Where,

|  Parameter |                                          Description                                          |
|----------|---------------------------------------------------------------------------------------------|
| `{file}` | Instance file path with a valid format, defined here                                    |
| `{max_time}`    | Maximum time execution, in seconds                                                                  |

### Output information

The execution report a output with the following relevant information:

`$ [set] [instance] [n] [p] [best_lb] [found_lb] [gap] [time] [memory] [status] [max_time]`

Where,

|  Output  |                                Description                               |
|-----------|------------------------------------------------------------------------|
| `[set]`       | Set number                                                               |
| `[instance]`  | Instance number                                                          |
| `[n]`         | Number of nodes                                                          |
| `[p]`         | Number of centers                                                        |
| `[best_lb]`   | Best known lower bound                                                    |
| `[found_lb]`   | Best lower bound found                                                   |
| `[gap]`   | Gap between `[best_lb]` and `[found_lb]`                                             |
| `[time]`      | Execution time, in seconds                                                |
| `[memory]`    | Maximum memory used                                                      |
| `[status]`  | CPLEX status                     |
| `[max_time]`  | Maximum time execution, in seconds                     |


## Citation

F. Aykut Özsoy, Mustafa Ç. Pınar, An exact algorithm for the capacitated vertex <em>p</em>-center problem, <em>Computers & Operations Research</em>,
33(5):1420–1436, 2006. [doi.org/10.1016/j.cor.2004.09.035](https://doi.org/10.1016/j.cor.2004.09.035).
