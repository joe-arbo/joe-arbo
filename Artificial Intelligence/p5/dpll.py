'''
Title:    Project 5
Author:   Joseph Arbolino
Date:     5/1/2022
'''
def dpll(clauses, symbols, model, depth=0):
    unknown_clauses = []
    for clause in clauses:
        # Evaluate each value in clause
        value = clauseEval(clause, model)

        # Decide if clause is known
        if value is False:
            return False, depth
        if value is None:
            unknown_clauses.append(clause)
    
    if not unknown_clauses:
        return model, depth
    
    # Eval pure symbols
    P, value = find_pure_symbol(symbols, unknown_clauses)
    if P:
        return dpll(clauses, remove_all(P, symbols), extend(model, P, value), depth + 1)
    
    # Eval unit clauses
    P, value = find_unit_clause(clauses, model)
    if P:
        return dpll(clauses, remove_all(P, symbols), extend(model, P, value), depth + 1)

    # Recursive call with chosen input
    P = first(symbols)
    value = True
    return (dpll(clauses, remove_all(P, symbols), extend(model, P, value), depth + 1) or
            dpll(clauses, remove_all(P, symbols), extend(model, P, not value), depth + 1))


def find_pure_symbol(symbols, clauses):
    for sym in symbols:
        found_pos, found_neg = False, False
        for clause in clauses:
            if not found_pos and sym in clause:
                found_pos = True
            elif not found_neg and -sym in clause:
                found_neg = True
        
        if found_pos != found_neg:
            return sym, found_pos
    return None, None


def find_unit_clause(clauses, model):
    for clause in clauses:
        P, value = unit_clause_assign(clause, model)
        if P:
            return P, value
    return None, None


def unit_clause_assign(clause, model):
    P, value = None, None
    for literal in clause:
        sym, positive = inspect_literal(literal)

        if sym in model:
            if model[sym] == positive:
                return None, None
        elif P:
            return None, None
        else:
            P, value = sym, positive
    return P, value


def inspect_literal(literal):
    if literal > 0:
        return literal, True
    else:
        return abs(literal), False


def clauseEval(clause, model):
    value = False
    for val in clause: 
        if abs(val) in model:      
            model_val = model[abs(val)]
        else:
            model_val = None
        
        if (model_val and val > 0) or (model_val is False and val < 0):
            return True
        if model_val is None:
            value = None

    return value


'''
Functions below this point were taken directly
from the textbook references.
'''
def extend(s, var, val):
    """Copy dict s and extend it by setting var to val; return copy."""
    return {**s, var: val}


def remove_all(item, seq):
    """Return a copy of seq (or string) with all occurrences of item removed."""
    if isinstance(seq, str):
        return seq.replace(item, '')
    elif isinstance(seq, set):
        rest = seq.copy()
        rest.remove(item)
        return rest
    else:
        return [x for x in seq if x != item]


def first(iterable, default=None):
    """Return the first element of an iterable; or default."""
    return next(iter(iterable), default)