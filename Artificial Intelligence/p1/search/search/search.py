# search.py
# ---------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem.
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state.
        """
        util.raiseNotDefined()

    def getSuccessors(self, state):
        """
          state: Search state

        For a given state, this should return a list of triples, (successor,
        action, stepCost), where 'successor' is a successor to the current
        state, 'action' is the action required to get there, and 'stepCost' is
        the incremental cost of expanding to that successor.
        """
        util.raiseNotDefined()

    def getCostOfActions(self, actions):
        """
         actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.
        The sequence must be composed of legal moves.
        """
        util.raiseNotDefined()


def tinyMazeSearch(problem):
    """
    Returns a sequence of moves that solves tinyMaze.  For any other maze, the
    sequence of moves will be incorrect, so only use this for tinyMaze.
    """
    from game import Directions
    s = Directions.SOUTH
    w = Directions.WEST
    return  [s, s, w, s, w, w, s, w]

def depthFirstSearch(problem):
    """
    Search the deepest nodes in the search tree first.

    Your search algorithm needs to return a list of actions that reaches the
    goal. Make sure to implement a graph search algorithm.

    To get started, you might want to try some of these simple commands to
    understand the search problem that is being passed in:

    print("Start:", problem.getStartState())
    print("Is the start a goal?", problem.isGoalState(problem.getStartState()))
    print("Start's successors:", problem.getSuccessors(problem.getStartState()))
    """
    stack = util.Stack()
    visited = []

    #push first state to stack
    stack.push((problem.getStartState(),[])) 
    while not stack.isEmpty():
        parent,plan = stack.pop()
        visited.append(parent)

        #check if goal state
        if problem.isGoalState(parent):
            return plan
        #find successors and check if they have been visited
        for i in problem.getSuccessors(parent):
            if i[0] not in visited:        
                stack.push((i[0], plan + [i[1]]))

    util.raiseNotDefined()

def breadthFirstSearch(problem):
    """Search the shallowest nodes in the search tree first."""

    queue = util.Queue()
    visited = []

    #push first state to queue
    queue.push((problem.getStartState(),[]))
    while not queue.isEmpty():
        parent,plan = queue.pop()
        #we check if visited here due to the differences in dfs and bfs
        #in dfs we could check when the node is found but for bfs we
        #need to check before it explores that nodes successors
        if parent not in visited:
            visited.append(parent)

            #check if goal state
            if problem.isGoalState(parent):
                return plan

            #find successors and push them to queue
            for i in problem.getSuccessors(parent):
                queue.push((i[0], plan + [i[1]]))

    util.raiseNotDefined()

def uniformCostSearch(problem):
    """Search the node of least total cost first."""
    
    pqueue = util.PriorityQueue()
    visited = []

    #push first state to queue
    pqueue.push((problem.getStartState(),[]), 0)
    while not pqueue.isEmpty():
        parent,plan = pqueue.pop()
        #again we check here since this is esentially bfs with a uniform cost heuristic
        if parent not in visited:
            visited.append(parent)

            #check if goal state
            if problem.isGoalState(parent):
                return plan

            #find successors and push them to queue
            for i in problem.getSuccessors(parent):
                priority = problem.getCostOfActions(plan + [i[1]])
                pqueue.push((i[0], plan + [i[1]]), priority)

    util.raiseNotDefined()

def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the nearest
    goal in the provided SearchProblem.  This heuristic is trivial.
    """
    return 0

#my priority function adding the priority and the heuristic cost
def priFunc(item):
    return item[2].getCostOfActions(item[1]) + item[3](item[0], item[2])

def aStarSearch(problem, heuristic=nullHeuristic):
    """Search the node that has the lowest combined cost and heuristic first."""
    pqueue = util.PriorityQueueWithFunction(priFunc)
    visited = []

    #push first state to queue
    #"item" for this queue consists of (state, plan, problem, heuristic)
    pqueue.push((problem.getStartState(), [], problem, heuristic))
    while not pqueue.isEmpty():
        item = pqueue.pop()
        #check if visited
        if item[0] not in visited:
            visited.append(item[0])

            #check if goal state
            if problem.isGoalState(item[0]):
                return item[1]

            #find successors and push them to queue along with item
            for i in problem.getSuccessors(item[0]):
                pqueue.push((i[0], item[1] + [i[1]], problem, heuristic))

    util.raiseNotDefined()


# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch
