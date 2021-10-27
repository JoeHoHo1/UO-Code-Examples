"""
The record of a vote being cast.
"""
from time import time


class Transaction:

    def __init__(self, sender, recipient, candidate, signature, amount=1):
        """
        Creates a new transaction
        :param sender: <str> sender account
        :param recipient: <str> recipient account
        :param candidate: <boolean> if candidate is recipient
        :param signature: <signature> signature of transaction encrypted using private key
        :param amount: <float> amount to be transferred
        """
        self.sender = sender
        self.recipient = recipient
        self.timestamp = time()
        self.candidate = candidate
        self.signature = signature
        self.amount = amount

    def validate(self):
        """
        Checks if a transaction is valid
        :return: <bool> True if it is valid, False if not.
        """
        # Prevent partial/negative transactions (whole vote/no stealing)
        if self.amount != 1:
            return False

        # Prevent vote cast to non-candidate
        if not self.candidate:
            return False

        return True
