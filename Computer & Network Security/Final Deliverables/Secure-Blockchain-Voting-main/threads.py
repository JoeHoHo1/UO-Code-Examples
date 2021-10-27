"""
A thread handler to use threading when handling user input.

Threading (for flavor) from:
    https://www.semicolonworld.com/question/43105/how-to-get-the-return-value-from-a-thread-in-python
"""
from threading import Thread


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
