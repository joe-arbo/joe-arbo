'''
Title:    Project 5
Author:   Joseph Arbolino
Date:     5/1/2022
'''
def parseFile(fileName):
    file = open(fileName, 'r')
    lines = file.readlines()
    file.close()

    try:
        index = [i for i, line in enumerate(lines) if 'p cnf' in line][0]
    except:
        print("No Problem Line")
        exit()

    problemLine = lines[index].split()
    numVars = int(problemLine[2])

    lines = lines[index + 1:]
    lines = [line.split() for line in lines]

    # Creates a list of lists for clauses
    clauses = [[int(i) for i in line] for line in lines]
    for clause in clauses:
        clause.remove(0)
    
    return numVars, clauses