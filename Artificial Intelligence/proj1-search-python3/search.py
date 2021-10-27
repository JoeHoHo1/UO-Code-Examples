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


from game import Directions
n = Directions.NORTH
e = Directions.EAST
s = Directions.SOUTH
w = Directions.WEST

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

    # Initialize empty Stack
    stack = util.Stack()

    stack.push([(problem.getStartState(), "Stop", 0)])

    # Initialise list of visited nodes
    vis = []

    # While not empty
    while not stack.isEmpty():
        # get the path
        path = stack.pop()

        # current state
        curr_state = path[-1][0]

        # if the current state is the goal state,
        if problem.isGoalState(curr_state):
            # return the actions to the goal state
            return [x[1] for x in path][1:]

        # if the current state not visited
        if curr_state not in vis:
            # mark as visited
            vis.append(curr_state)

            # for all the successors
            for succ in problem.getSuccessors(curr_state):
                # if the successor's state unvisited
                if succ[0] not in vis:
                    # Copy parent's path
                    succPath = path[:]
                    # Set the path to parent's path + successor
                    succPath.append(succ)
                    # Push the path
                    stack.push(succPath)

    # if fails return False
    return False

def breadthFirstSearch(problem):
    """Search the shallowest nodes in the search tree first."""

    # Initialize empty Queue
    queue = util.Queue()

    queue.push([(problem.getStartState(), "Stop", 0)])

    # Initialise list of visited nodes
    vis = []

    # While not empty
    while not queue.isEmpty():
        # get the path
        path = queue.pop()

        # current state
        curr_state = path[-1][0]

        # if the current state is the goal state,
        if problem.isGoalState(curr_state):
            # return the actions to the goal state
            return [x[1] for x in path][1:]

        # if the current state not visited
        if curr_state not in vis:
            # mark as visited
            vis.append(curr_state)

            # for all the successors
            for succ in problem.getSuccessors(curr_state):
                # if the successor's state unvisited
                if succ[0] not in vis:
                    # Copy parent's path
                    succPath = path[:]
                    # Set the path to parent's path + successor
                    succPath.append(succ)
                    # Push the path
                    queue.push(succPath)

    # if fails return False
    return False

def uniformCostSearch(problem):
    """Search the node of least total cost first."""

    # calculate cost and initialize empty PriorityQueue
    queue = util.PriorityQueueWithFunction(lambda path: problem.getCostOfActions([x[1] for x in path][1:]))

    queue.push([(problem.getStartState(), "Stop", 0)])

    # Initialise list of visited nodes
    vis = []

    # While not empty
    while not queue.isEmpty():
        # get the path
        path = queue.pop()

        # current state
        curr_state = path[-1][0]

        # if the current state is the goal state,
        if problem.isGoalState(curr_state):
            # return the actions to the goal state
            return [x[1] for x in path][1:]

        # if the current state not visited
        if curr_state not in vis:
            # mark as visited
            vis.append(curr_state)

            # for all the successors
            for succ in problem.getSuccessors(curr_state):
                # if the successor's state unvisited
                if succ[0] not in vis:
                    # Copy parent's path
                    succPath = path[:]
                    # Set the path to parent's path + successor
                    succPath.append(succ)
                    # Push the path
                    queue.push(succPath)

    # if fails return False
    return False

def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the nearest
    goal in the provided SearchProblem.  This heuristic is trivial.
    """
    return 0

def aStarSearch(problem, heuristic=nullHeuristic):
    """Search the node that has the lowest combined cost and heuristic first."""
    # calculate cost with heuristic and initialize empty PriorityQueue
    queue = util.PriorityQueueWithFunction(lambda path: problem.getCostOfActions([x[1] for x in path][1:]) + heuristic(path[-1][0], problem))

    queue.push([(problem.getStartState(), "Stop", 0)])

    # Initialise list of visited nodes
    vis = []

    # While not empty
    while not queue.isEmpty():
        # get the path
        path = queue.pop()

        # current state
        curr_state = path[-1][0]

        # if the current state is the goal state,
        if problem.isGoalState(curr_state):
            # return the actions to the goal state
            return [x[1] for x in path][1:]

        # if the current state not visited
        if curr_state not in vis:
            # mark as visited
            vis.append(curr_state)

            # for all the successors
            for succ in problem.getSuccessors(curr_state):
                # if the successor's state unvisited
                if succ[0] not in vis:
                    # Copy parent's path
                    succPath = path[:]
                    # Set the path to parent's path + successor
                    succPath.append(succ)
                    # Push the path
                    queue.push(succPath)

    # if fails return False
    return False

# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch
