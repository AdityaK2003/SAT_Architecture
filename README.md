# SAT_Architecture
Exploration and implementation of a specific architecture to implement SAT problems, inspired by PLA folding.


## Usage
The `experiments.cpp` file is used to run experiments on the SAT Architectures.
To create the executable:
```
make
```

To run the executable:
```
./experiments
```

## Introduction and Background
This research focuses on Circuit Satisfiability (SAT), which is a fundamental problem in complexity theory. Essentially, the Circuit SAT problem asks whether a valid input assignment exists for a circuit representation of a boolean formula such that the circuit’s output is “True”.

Specifically, this work focuses on reducing the complexity of Circuit SAT representations. Currently, SAT circuits are represented in a matrix (2D array) format, with each variable having two columns associated with it (one for the regular literal, one for the negated literal). Additionally, the rows correspond to clauses, leading to an array size of `2nm`, where `n` is the number of variables and `m` is the number of clauses. Although this circuit representation allows for the creation of any sets of clauses, the major drawback is the sparse nature of this matrix and the large size of the circuit. In turn, the large size of the circuit increases power consumption as well as the delay of circuit SAT. 

## Approach
Leveraging previous research experience in [SAT solving](https://github.com/AdityaK2003/SAT_Solver) (such as designing a from-scratch solver and numerous functions to test runtimes for different heuristics),  this code implements a backtracking technique to implement a SAT formula on a certain architecture. 

Specifically, an architecture is designed as some SAT circuit which could have multiple lines in a single column; this restricts both variable and clause placements.



### Specific Research Notes
[link](https://docs.google.com/document/d/14Z1hpFxtM1TlqqpX4Z06geHvYqIu-eOSUZ0HzGPkjGk/edit)
