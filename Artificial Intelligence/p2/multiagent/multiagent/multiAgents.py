# multiAgents.py
# --------------
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


from util import manhattanDistance
from game import Directions
import random, util

from game import Agent

class ReflexAgent(Agent):
    """
    A reflex agent chooses an action at each choice point by examining
    its alternatives via a state evaluation function.

    The code below is provided as a guide.  You are welcome to change
    it in any way you see fit, so long as you don't touch our method
    headers.
    """


    def getAction(self, gameState):
        """
        You do not need to change this method, but you're welcome to.

        getAction chooses among the best options according to the evaluation function.

        Just like in the previous project, getAction takes a GameState and returns
        some Directions.X for some X in the set {NORTH, SOUTH, WEST, EAST, STOP}
        """
        # Collect legal moves and successor states
        legalMoves = gameState.getLegalActions()

        # Choose one of the best actions
        scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
        bestScore = max(scores)
        bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
        chosenIndex = random.choice(bestIndices) # Pick randomly among the best

        "Add more of your code here if you want to"

        return legalMoves[chosenIndex]

    def evaluationFunction(self, currentGameState, action):
        """
        Design a better evaluation function here.

        The evaluation function takes in the current and proposed successor
        GameStates (pacman.py) and returns a number, where higher numbers are better.

        The code below extracts some useful information from the state, like the
        remaining food (newFood) and Pacman position after moving (newPos).
        newScaredTimes holds the number of moves that each ghost will remain
        scared because of Pacman having eaten a power pellet.

        Print out these variables to see what you're getting, then combine them
        to create a masterful evaluation function.
        """
        # Useful information you can extract from a GameState (pacman.py)
        successorGameState = currentGameState.generatePacmanSuccessor(action)
        newPos = successorGameState.getPacmanPosition()
        newFood = successorGameState.getFood()
        newGhostStates = successorGameState.getGhostStates()
        newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

        "*** YOUR CODE HERE ***"
        score = 0

        if action == 'Stop':
            score -= 100

        foodDists = [manhattanDistance(newPos, food) for food in newFood.asList()]

        #if empty than pacman has eaten all food
        if foodDists: 
            score -= min(foodDists)

        #In this case there will always be at least one ghost
        minGhostDist = min([manhattanDistance(newPos, ghost.getPosition()) for ghost in newGhostStates])
        if (min(newScaredTimes) > 0):
            score -= minGhostDist #Reverse weight if ghosts are scared
        else:
            score += minGhostDist

        return successorGameState.getScore() + score

def scoreEvaluationFunction(currentGameState):
    """
    This default evaluation function just returns the score of the state.
    The score is the same one displayed in the Pacman GUI.

    This evaluation function is meant for use with adversarial search agents
    (not reflex agents).
    """
    return currentGameState.getScore()

class MultiAgentSearchAgent(Agent):
    """
    This class provides some common elements to all of your
    multi-agent searchers.  Any methods defined here will be available
    to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

    You *do not* need to make any changes here, but you can if you want to
    add functionality to all your adversarial search agents.  Please do not
    remove anything, however.

    Note: this is an abstract class: one that should not be instantiated.  It's
    only partially specified, and designed to be extended.  Agent (game.py)
    is another abstract class.
    """

    def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
        self.index = 0 # Pacman is always agent index 0
        self.evaluationFunction = util.lookup(evalFn, globals())
        self.depth = int(depth)

class MinimaxAgent(MultiAgentSearchAgent):
    """
    Your minimax agent (question 2)
    """

    def getAction(self, gameState):
        """
        Returns the minimax action from the current gameState using self.depth
        and self.evaluationFunction.

        Here are some method calls that might be useful when implementing minimax.

        gameState.getLegalActions(agentIndex):
        Returns a list of legal actions for an agent
        agentIndex=0 means Pacman, ghosts are >= 1

        gameState.generateSuccessor(agentIndex, action):
        Returns the successor game state after an agent takes an action

        gameState.getNumAgents():
        Returns the total number of agents in the game

        gameState.isWin():
        Returns whether or not the game state is a winning state

        gameState.isLose():
        Returns whether or not the game state is a losing state
        """
        "*** YOUR CODE HERE ***"
        def minimax(gameState, agentIndex, depth):
            # check if win/lose state
            if (gameState.isLose() or gameState.isWin() or depth == self.depth):
                return [self.evaluationFunction(gameState)]

            # Wrap agents around back to pacman
            if agentIndex == gameState.getNumAgents() - 1:
                depth += 1
                nextAgentIndex = self.index
            else:
                nextAgentIndex = agentIndex + 1

            # check if pacman: maximize points gained
            if agentIndex == 0:
                maxPoints = -float("inf")
                for action in gameState.getLegalActions(agentIndex):
                    newMaxPoints = minimax(gameState.generateSuccessor(agentIndex, action), nextAgentIndex, depth)[0]
                    if newMaxPoints > maxPoints:
                        maxPoints = newMaxPoints
                        maxAction = action
                return maxPoints, maxAction
            # else ghost: minimize points lost
            else:
                minPoints = float("inf")
                for action in gameState.getLegalActions(agentIndex):
                    newMinPoints = minimax(gameState.generateSuccessor(agentIndex, action), nextAgentIndex, depth)[0]
                    if newMinPoints < minPoints:
                        minPoints = newMinPoints
                        minAction = action
            return minPoints, minAction
        # end of function

        # Return best possible move for each case
        return minimax(gameState, self.index, 0)[1]

        util.raiseNotDefined()

class AlphaBetaAgent(MultiAgentSearchAgent):
    """
    Your minimax agent with alpha-beta pruning (question 3)
    """

    def getAction(self, gameState):
        """
        Returns the minimax action using self.depth and self.evaluationFunction
        """
        "*** YOUR CODE HERE ***"
        def alpha_beta(gameState, agentIndex, alpha, beta, depth):
            alphaAction = None
            betaAction = None

            # check if win/lose state
            if (gameState.isLose() or gameState.isWin() or depth == self.depth):
                return [self.evaluationFunction(gameState)]

            # Wrap agents around back to pacman
            if agentIndex == gameState.getNumAgents() - 1:
                depth += 1
                nextAgentIndex = self.index
            else:
                nextAgentIndex = agentIndex + 1

            # check if pacman: calculate alpha
            if agentIndex == 0:
                maxPoints = -float("inf")
                for action in gameState.getLegalActions(agentIndex):
                    newMaxPoints = alpha_beta(gameState.generateSuccessor(agentIndex, action), nextAgentIndex, alpha, beta, depth)[0]
                    if newMaxPoints > maxPoints:
                        maxPoints = newMaxPoints
                        maxAction = action

                    if maxPoints > beta:
                        return maxPoints, maxAction
                    alpha = max(alpha, maxPoints)

                return maxPoints, maxAction
            # else ghost: calculate beta
            else:
                minPoints = float("inf")
                for action in gameState.getLegalActions(agentIndex):
                    newMinPoints = alpha_beta(gameState.generateSuccessor(agentIndex, action), nextAgentIndex, alpha, beta, depth)[0]
                    if newMinPoints < minPoints:
                        minPoints = newMinPoints
                        minAction = action

                    if minPoints < alpha:
                        return minPoints, minAction
                    beta = min(beta, minPoints)

            return minPoints, minAction
        # end of function

        # Return best possible move for each case
        return alpha_beta(gameState, self.index, -float("inf"), float("inf"), 0)[1]

        util.raiseNotDefined()

class ExpectimaxAgent(MultiAgentSearchAgent):
    """
      Your expectimax agent (question 4)
    """

    def getAction(self, gameState):
        """
        Returns the expectimax action using self.depth and self.evaluationFunction

        All ghosts should be modeled as choosing uniformly at random from their
        legal moves.
        """
        "*** YOUR CODE HERE ***"
        def expectimax(gameState, agentIndex, depth):
            numActions = len(gameState.getLegalActions(agentIndex))

            # check if win/lose state
            if (gameState.isLose() or gameState.isWin() or depth == self.depth):
                return [self.evaluationFunction(gameState)]

            # Wrap agents around back to pacman
            if agentIndex == gameState.getNumAgents() - 1:
                depth += 1
                nextAgentIndex = self.index
            else:
                nextAgentIndex = agentIndex + 1

            # check if pacman: maximize points gained
            if agentIndex == 0:
                maxPoints = -float("inf")
                for action in gameState.getLegalActions(agentIndex):
                    newMaxPoints = expectimax(gameState.generateSuccessor(agentIndex, action), nextAgentIndex, depth)[0]
                    if newMaxPoints > maxPoints:
                        maxPoints = newMaxPoints
                        maxAction = action
                return maxPoints, maxAction
            # else ghost: calculate expectedMax
            else:
                expectedMax = 0
                for action in gameState.getLegalActions(agentIndex):
                    newExpectedMax = expectimax(gameState.generateSuccessor(agentIndex, action), nextAgentIndex, depth)[0]

                    expectedMax += newExpectedMax
            return (expectedMax/numActions, gameState.getLegalActions(agentIndex)[numActions-1])
        # end of function

        # Return best possible move for each case
        return expectimax(gameState, self.index, 0)[1]

        util.raiseNotDefined()

def betterEvaluationFunction(currentGameState):
    """
    Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
    evaluation function (question 5).

    DESCRIPTION: <write something here so we know what you did>
    """
    "*** YOUR CODE HERE ***"
    currPos = currentGameState.getPacmanPosition()
    currFood = currentGameState.getFood()
    currGhostStates = currentGameState.getGhostStates()
    score = 0

    foodDists = [manhattanDistance(currPos, food) for food in currFood.asList()]

    #if empty than pacman has eaten all food
    if foodDists: 
        score += 1 / min(foodDists)

    #In this case there will always be at least one ghost
    for ghost in currGhostStates:
        ghostDist = manhattanDistance(currPos, ghost.getPosition())
        if ghostDist > 0:
            if ghost.scaredTimer > 0:
                score += 1 / ghostDist #Reverse weight if ghosts are scared
            else:
                score -= 1 / ghostDist
    return currentGameState.getScore() + score
    util.raiseNotDefined()

# Abbreviation
better = betterEvaluationFunction
