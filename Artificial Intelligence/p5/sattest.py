'''
Title:    Project 5
Author:   Joseph Arbolino
Date:     5/1/2022

WARNING: Tests take a long time to run.
'''
from satsolve import satsolve
from random import randint
import matplotlib.pyplot as plt

# Tests satsolve with a random cnf file
def test_sat():
    ratios, depths, times = [], [], []
    avg = 5
    for m in range(1, 401):
        ratio, depth, time = 0, 0, 0
        for i in range(avg):
            ratio += genFile(m)
            dpll_depth, walkSAT_time = satsolve("randomcnf.txt")
            depth += dpll_depth
            time += walkSAT_time
        ratios.append(ratio / avg)
        depths.append(depth / avg)
        times.append(time / avg)

    plt.plot(ratios, depths)
    plt.xlabel('Clause/symbol ratio (m/n)')
    plt.ylabel('Runtime (Recursive Calls)')
    plt.savefig('graph_dpll.png')
    plt.close()
    
    plt.plot(ratios, times)
    plt.xlabel('Clause/symbol ratio (m/n)')
    plt.ylabel('Runtime (Execution Time)')
    plt.savefig('graph_walkSAT.png')
    print('Results in graph_dpll.png and graph_walkSAT.png')


# Generates a random cnf file with m clauses
def genFile(m: int) -> float:
    n = 50
    ratio = m/n
    file = open('randomcnf.txt', 'w')
    string = 'p cnf ' + str(n) + ' ' + str(m) + '\n'
    file.write(string)
    for i in range(m):
        string = ''
        for j in range(3):
            r = randint(1, 2)
            if r == 1:
                string = string + str(-randint(1, n)) + ' '
            else:
                string = string + str(randint(1, n)) + ' '
        string = string + str(0) + ' \n'
        file.write(string)
    file.close()
    return ratio


if __name__ == "__main__":
    test_sat()
