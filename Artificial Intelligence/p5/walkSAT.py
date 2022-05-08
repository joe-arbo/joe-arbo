'''
Title:    Project 5
Author:   Joseph Arbolino
Date:     5/1/2022
'''
import random
from dpll import clauseEval

def walkSAT(clauses, p=0.5, maxFlips=10000):
    # Generate symbols and current model
    symbols = {abs(sym) for clause in clauses for sym in clause}
    model = {s: random.choice([True, False]) for s in symbols}
 
    # Itererate through allowed flips
    for i in range(maxFlips):
        satisfied, unsatisfied = [], []
        for clause in clauses:
            (satisfied if clauseEval(clause, model) else unsatisfied).append(clause)
        if not unsatisfied:
            return model
        clause = random.choice(unsatisfied)
        if p > random.uniform(0.0, 1.0):
            sym = abs(random.choice(clause))
        else:
            # key function
            def sat_count(sym):
                sym = abs(sym)
                model[sym] = not model[sym]
                count = len([clause for clause in clauses if clauseEval(clause, model)])
                model[sym] = not model[sym]
                return count
            sym = max(clause, key=sat_count)
            sym = abs(sym)
        model[sym] = not model[sym]
    
    return None