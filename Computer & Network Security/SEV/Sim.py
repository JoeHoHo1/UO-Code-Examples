from time import sleep
import queue
import random
import sys
from threading import Thread
from io import StringIO
import Registrar
import Blockchain
import VoterDB

class ThreadWithReturnValue(Thread):
    def __init__(self, group=None, target=None, name=None, args=(), kwargs=None, *, daemon=None):
        Thread.__init__(self, group, target, name, args, kwargs, daemon=daemon)

        self._return = None

    def run(self):
        if self._target is not None:
            self._return = self._target(*self._args, **self._kwargs)

    def join(self):
        Thread.join(self)
        return self._return


class Sim:
    # helper methods
    def sim_reg():
        # init
        database = VoterDB()
        blockchain = Blockchain()
        # redirect stdout
        old_stdout = sys.stdout
        sys.stdout = redir = StringIO()
        # simulate 100 attempts to register/vote
        for i in range(100):
            id = random.randrange(1, 50)
            print("\n--------------------------------------------------------------\n" +
                  "Attempt for ID: " + str(id) +
                  "\n--------------------------------------------------------------")
            keypair = Registrar.registrar(id, database)
            print("Key Pair (will hide in final version): " + str(keypair))
            # If registration is succesful
            if not keypair:
                "Don't Vote"
            else:
                "Vote"

        sys.stdout = old_stdout
        results = redir.getvalue()
        return results

    def animate(process):
        while process.is_alive():
            chars = "⢿⣻⣽⣾⣷⣯⣟⡿"
            for char in chars:
                sys.stdout.write('\r' + 'loading ' + char)
                sleep(.1)
                sys.stdout.flush()
        sys.stdout.write('\rFinished!     ')

    print("This is a simulation of multiple users attempting to register and vote.\n\
-------------------------------------------------------------------------------\n\
Please comment out the Sim() function as designated in Main for actual use.\n\n\
As the valid voters are randomized, entering the same ID more than once may\n\
result in a user being \"valid\" one time and not another. Use in conjunction with\n\
an actual verification system to avoid this issue.")
    print("\n")
    # Wait for Enter key to be pressed
    input("Press Enter to continue...")

    # Loading animation during process
    que = queue.Queue()

    t = ThreadWithReturnValue(target=sim_reg, args=())
    t.start()
    animate(t)

    print("\n")
    # Wait for Enter key to be pressed
    input("Press Enter to see results...")
    print(t.join())