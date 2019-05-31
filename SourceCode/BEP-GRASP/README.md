# Greedy Randomized Adaptive Search Procedure (GRASP) metaheuristic for the resolution of the Bus Evacuation Problem
### *Author: Javiera Loyola Vitali*
[Versión en Español](./README.es-ES.md)

### 1. Compile source code

To compile the source code, open the console on the parent folder and run `make` or `make all`. To remove object files *.o, run `make clean`.


### 2. Execute algorithm

To obtain a detail on the parameters of the algorithm, use `./bep-grasp -h`

To execute the algorithm, use the following format: 

```bash
./bep-grasp -i {directory}/{instanceName} -s {seed} -ref {lrcSizeProportion} -ithc {maxIterationsHC} -t {timeLimit} -o {outputFilename}
```

An example would be `./bep-grasp -i instances/real/BEP-5-52-12-50.txt -s 5 -ref 0.3 -ithc 10 -t 10 -o result.txt`


### 3. Source files

**main:** Contains the management of the input parameters, and call other functions to execute the algorithm

**dataStructures:** Contains the definition of the main objects to represent the problem (Bus, Depot, Shelter y Assembly Point) 

**fileHandler:** Contains methods to read instance file and write the output file

**problemInstance:** Mapping of the problem objects. They exist on the memory, and pointers are contained on arrays on the ProblemInstance object.

**solution:** Contains a list of the buses, each one containing the scheduled trips to perform. Contains the elements for the best solution found.

**tripMapping:** Object that maps a trip between two nodes.

**solver:** Contains the GRASP structure, methods that generate the initial solution, and the methods used by the local search algorithm (Hill Climbing) for post-processing.


### 4. Bash files for experiments

To execute experiments, three bash files are available.

**bash-bep-Random.sh:** test on randomly generated instances
**bash-bep-Real.sh:** test on real case based instances
**bash-bep-Real_bus-var.sh:** test on one real-based instance, changing the number of buses.
