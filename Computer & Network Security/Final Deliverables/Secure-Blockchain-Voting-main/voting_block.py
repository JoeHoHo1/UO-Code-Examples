"""
A basic block that votes are stored on in a voting blockchain system.
"""
import hashlib
from time import time


class VBlock:

    def __init__(self, index, transactions, signature, nonce, previous_hash):
        """
        Constructs a new block
        :param index:
        :param transactions:
        :param signature:
        :param previous_hash:
        """
        self.index = index
        self.timestamp = time()
        self.transactions = transactions    # vote
        self.signature = signature
        self.nonce = nonce
        self.previous_hash = previous_hash
        self.hash = self.hash_block()

    def hash_block(self):
        """
        Calculates the hash of the block
        :return hashed block:
        """
        block_hash = hashlib.sha256()
        block_hash.update(
            str(self.index).encode('utf-8') +
            str(self.timestamp).encode('utf-8') +
            str(self.transactions).encode('utf-8') +
            str(self.signature).encode('utf-8') +
            str(self.nonce).encode('utf-8') +
            str(self.previous_hash).encode('utf-8')
        )
        return block_hash.hexdigest()
