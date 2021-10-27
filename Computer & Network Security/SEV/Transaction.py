from time import time

class Transaction:
    def __init__(self, sender, recipient, amount = 1):
        """
        Creates a new transaction
        :param sender: <str> sender account
        :param recipient: <str> recipient account
        :param amount: <float> amount to be transferred
        """
        self.sender = sender
        self.recipient = recipient
        self.timestamp = time()
        self.amount = amount

    def validate(self):
        """
        Checks if a transaction is valid
        :return: <bool> True if it is valid, False if not.
        """

        # Prevent partial/negative transactions (whole vote/no stealing)
        if self.amount != 1:
            return False
        """
        IF WE WANT TO CHECK FOR VALID RECIPIENT
        """
        """
        # Prevent vote cast to non-candidate
        if self.recipient != VALIDRECIPIENT
            return False
        """
        return True