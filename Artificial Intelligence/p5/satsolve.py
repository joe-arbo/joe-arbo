'''
Title:    Project 5
Author:   Joseph Arbolino
Date:     5/1/2022
'''
from fileparser import parseFile
from dpll import dpll
from walkSAT import walkSAT
import time

def satsolve(fileName):
    # Parse file for clauses
    numVars, clauses = parseFile(fileName)
    dpll_model, walkSAT_model = {}, {}
    symbols = list(range(1, numVars + 1))

    # Call recursive function
    dpll_model, dpll_depth = dpll(clauses, symbols, dpll_model)

    # Use time to measure instead of recursive calls
    start_time = time.time()
    walkSAT_model = walkSAT(clauses)
    finish_time = time.time()
    walkSAT_time = finish_time - start_time

    # Print Format
    print('Test case name:', fileName)
    if dpll_model:
        print('DPLL: Satisfied')
        print('Model:', end=' ')
        [print(f'{int(dpll_model[sym])}', end='') for sym in symbols if sym in dpll_model]
        print ('\nCalls:', dpll_depth)
    else:
        print('DPLL: Not Satisfied')
        print('Model: N/A')
        print('Calls:', dpll_depth)

    if walkSAT_model:
        print('WalkSAT: Satisfied')
        print('Model:', end=' ')
        [print(f'{int(walkSAT_model[sym])}', end='') for sym in symbols if sym in walkSAT_model]
        print ('\nFlips: 1\n')
    else:
        print('WalkSAT: Not Satisfied')
        print('Model: N/A')
        print('Flips: Exeeded Max Flips\n')
    
    return dpll_depth, walkSAT_time


def main():
    satsolve('c17.txt')
    satsolve('hole6.txt')
    satsolve('testcase1.txt')
    satsolve('testcase-aim-50-1_6-yes1-4.txt')
    satsolve('testcase-quinn.txt')
    satsolve('testcase-aim-100-1_6-no-1.txt')
    satsolve('dubois20.txt')
    satsolve('dubois21.txt')
    satsolve('dubois22.txt')


if __name__ == '__main__':
    main()
