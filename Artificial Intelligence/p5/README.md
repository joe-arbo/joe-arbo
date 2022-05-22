# Project 5

Project 5 was different from the rest of the projects in this course. It no longer pulls from the UC Berkeley course and now focuses on Boolean Satisfiability using DPLL and WalkSAT (two algorithms used to evaluate Boolean statements). DPLL is recursive and implemented in `dpll.py` while WalkSAT is iterative and is housed in `walkSAT.py`. 

`satsolve.py` holds the `main` function for the program and first sends a file to be parsed in `fileParser.py`. The files contain numerical representations of multi-variable Boolean statements. After `fileParser.py` returns the parsed information, it is fed through both the DPLL and WalkSAT implementations. That data (whether the statement is satisfiable or not) is then printed for the user and modeled graphically.

There is also a `sattest.py` which was used to run many iterations of each algorithm on randomized input files. This facilitated testing and allowed for answer verification.
