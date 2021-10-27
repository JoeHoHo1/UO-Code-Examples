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
        GameStates (pacman.py) and returns a number, where hig her numbersare better.

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

        # calculate distance to farthest food
        minDist = -1                            # init
        for food in newFood.asList():
            if minDist >= util.manhattanDistance(newPos, food) or minDist == -1:    # don't think I need to explain
                minDist = util.manhattanDistance(newPos, food)                      # every manhattan dist call


        # calculate distance from pacman to ghosts
        distGhost = 1                                               # init
        for ghostState in successorGameState.getGhostPositions():
            distGhost += util.manhattanDistance(newPos, ghostState)
            if util.manhattanDistance(newPos, ghostState) <= 1:     # RUN
                return -1
        # bigger food distance is less of an increase
        # bigger ghost distance is less of a decrease
        # score most important
        return successorGameState.getScore() + (1 / float(minDist)) - (1 / float(distGhost))

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

        # definitions inside action to avoid unnecessary self call
        # implementation of minimax
        def minimax(agent, depth, gameState):
            # if game ends or depth reached
            if gameState.isWin() or gameState.isLose() or depth == self.depth:
                return self.evaluationFunction(gameState)
            if agent == 0:
                return max(minimax(1, depth, gameState.generateSuccessor(agent, newState)) \
                           for newState in gameState.getLegalActions(agent))
            else:                               # min for ghosts
                nextAgent = agent + 1           # increasing depth
                if gameState.getNumAgents() == nextAgent:
                    nextAgent = 0
                if nextAgent == 0:
                    depth += 1
                return min(minimax(nextAgent, depth, gameState.generateSuccessor(agent, newState)) \
                           for newState in gameState.getLegalActions(agent))

        maximum = float("-inf")                 # init
        action = Directions.EAST                # init
        for agentState in gameState.getLegalActions(0):
            utility = minimax(1, 0, gameState.generateSuccessor(0, agentState))
            if utility > maximum or maximum == float("-inf"):       # store max
                maximum = utility
                action = agentState

        return action


class AlphaBetaAgent(MultiAgentSearchAgent):
    """
    Your minimax agent with alpha-beta pruning (question 3)
    """

    def getAction(self, gameState):
        """
        Returns the minimax action using self.depth and self.evaluationFunction
        """
        "*** YOUR CODE HERE ***"

        # definitions inside action to avoid unnecessary self call
        # maximizes based on provided pseudo
        def maximizer(agent, depth, gameState, a, b):
            v = float("-inf")                       # init

            for newState in gameState.getLegalActions(agent):
                v = max(v, prune(1, depth, gameState.generateSuccessor(agent, newState), a, b))
                if v > b:
                    return v
                a = max(a, v)
            return v

        # minimizes based on provided pseudo
        def minimizer(agent, depth, gameState, a, b):
            v = float("inf")                        # init
            nextAgent = agent + 1                   # iterate agent

            if gameState.getNumAgents() == nextAgent:       # end of agents
                nextAgent = 0
                depth += 1

            for newState in gameState.getLegalActions(agent):
                v = min(v, prune(nextAgent, depth, gameState.generateSuccessor(agent, newState), a, b))
                if v < a:
                    return v
                b = min(b, v)
            return v

        # prune
        def prune(agent, depth, gameState, a, b):
            # basically the same as in minimax
            # if game ends or depth reached
            if gameState.isWin() or gameState.isLose() or depth == self.depth:
                return self.evaluationFunction(gameState)

            if agent == 0:
                return maximizer(agent, depth, gameState, a, b)
            else:
                return minimizer(agent, depth, gameState, a, b)

        # maximizing to pacman
        utility = float("-inf")                 # init
        action = Directions.EAST                # init
        a = float("-inf")                       # init
        b = float("inf")                        # init
        for agentState in gameState.getLegalActions(0):
            ghostVal = prune(1, 0, gameState.generateSuccessor(0, agentState), a, b)
            if ghostVal > utility:              # store max in util
                utility = ghostVal
                action = agentState
            if utility > b:
                return utility
            a = max(a, utility)

        return action


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

        # definitions inside action to avoid unnecessary self call
        # expectimax logic
        def expectimax(agent, depth, gameState):
            if gameState.isLose() or gameState.isWin() or depth == self.depth:  # return the utility in case the defined
                return self.evaluationFunction(gameState)                       # depth is reached or game is won/lost.
            if agent == 0:                                                      # maximizing for pacman
                return max(expectimax(1, depth, gameState.generateSuccessor(agent, newState)) \
                           for newState in gameState.getLegalActions(agent))
            else:                                                               # expectimax for ghost nodes.
                nextAgent = agent + 1                                           # iterate agent
                if gameState.getNumAgents() == nextAgent:
                    nextAgent = 0
                    depth += 1                                                  # iterate depth
                return sum(expectimax(nextAgent, depth, gameState.generateSuccessor(agent, newState)) for newState \
                           in gameState.getLegalActions(agent)) / float(len(gameState.getLegalActions(agent)))

        """maximizing task to pacman"""
        maximum = float("-inf")                                                 # init
        action = Directions.EAST
        for agentState in gameState.getLegalActions(0):
            utility = expectimax(1, 0, gameState.generateSuccessor(0, agentState))
            if utility > maximum or maximum == float("-inf"):                   # store max
                maximum = utility
                action = agentState

        return action

def betterEvaluationFunction(currentGameState):
    """
    Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
    evaluation function (question 5).

    DESCRIPTION: <reflex with scared timer added and some weighting tweaks>
    """
    "*** YOUR CODE HERE ***"
    """distance to closest food"""
    newPos = currentGameState.getPacmanPosition()

    minDist = -1                                                # init
    for food in currentGameState.getFood().asList():
        if minDist >= util.manhattanDistance(newPos, food) or minDist == -1:
            minDist = util.manhattanDistance(newPos, food)

    """distance from pacman to ghosts"""
    distGhost = 1                                               # init
    for ghostState in currentGameState.getGhostPositions():
        distGhost += util.manhattanDistance(newPos, ghostState)
        if util.manhattanDistance(newPos, ghostState) <= 1:     # RUN
            return float("-inf")

    """simple calculation with weight tweaks"""
    return currentGameState.getScore() + (3 / float(minDist)) - (3 / float(distGhost)) + \
           sum(ghostState.scaredTimer for ghostState in currentGameState.getGhostStates())  # scared timer added

# Abbreviation
better = betterEvaluationFunction
