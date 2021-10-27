import copy
import unittest

from blockchain import Blockchain
from voting_block import VBlock


class TestTransaction(unittest.TestCase):

    def test_create_transaction_candidate(self):
        """
        Test creating a transaction with a valid candidate
        """
        blockchain = Blockchain()
        blockchain.add_candidate('recipient')
        _, valid = blockchain.create_transaction('sender', 'recipient', 1)
        transaction = blockchain.last_transaction

        # Let's now validate the transaction
        self.assertTrue(valid)
        self.assertEqual(transaction.sender, 'sender')
        self.assertEqual(transaction.recipient, 'recipient')
        self.assertEqual(transaction.amount, 1)

    def test_create_transaction_not_candidate(self):
        """
        Test creating a transaction with an invalid candidate
        """
        blockchain = Blockchain()
        transaction, valid = blockchain.create_transaction('sender', 'recipient', 1)

        # Let's now validate the transaction
        self.assertIsNone(transaction)
        self.assertFalse(valid)

    def test_create_negative_transaction(self):
        """
        Test creating a transaction with a negative amount
        """
        blockchain = Blockchain()
        transaction, valid = blockchain.create_transaction('sender', 'recipient', -1)

        # Let's now validate the transaction
        self.assertIsNone(transaction)
        self.assertFalse(valid)


class TestBlock(unittest.TestCase):
    def test_block_hash(self):
        """
        Test hashing blocks
        """
        block = VBlock(1, [], '0', 0, '0')

        # If we recalculate the hash on the block we should get the same result as we have stored
        self.assertEqual(block.hash, block.hash_block())


class TestBlockchain(unittest.TestCase):

    def test_validate_empty_chain(self):
        """
        Test validating an empty chain
        """

        blockchain = Blockchain()
        blockchain.mine()

        self.assertTrue(blockchain.validate_chain(blockchain.full_chain))

    def test_validate_chain_with_tempered_block_nonce(self):
        """
        Test validating a chain with a tempered block nonce
        """
        blockchain = Blockchain()
        last_block = blockchain.mine()

        # First we look that a new block could be mined
        self.assertIsNotNone(last_block)

        chain = blockchain.full_chain

        # Hack a block
        chain.append(VBlock(1, [], 'sig', 1, last_block.hash))

        self.assertFalse(blockchain.validate_chain(blockchain.full_chain))

    def test_replace_chain(self):
        """
        Test that the chain is replaced for a larger one
        :return:
        """

        blockchain1 = Blockchain()
        blockchain1.mine()

        blockchain2 = copy.deepcopy(blockchain1)
        blockchain2.mine()

        # Now let's make sure that each blockchain has its own number of blocks
        self.assertEqual(2, len(blockchain1.full_chain))
        self.assertEqual(3, len(blockchain2.full_chain))

        # Then let's replace blockchain1 with blockchain2
        blockchain1.replace_chain(blockchain2.full_chain)

        self.assertEqual(3, len(blockchain1.full_chain))
        self.assertEqual(3, len(blockchain2.full_chain))
        self.assertEqual(blockchain1.last_block.hash, blockchain2.last_block.hash)

    def test_replace_chain_keep_original(self):
        """
        Test that the chain is not replaced for a smaller one
        :return:
        """

        blockchain1 = Blockchain()
        blockchain1.mine()

        blockchain2 = copy.deepcopy(blockchain1)
        blockchain1.mine()

        # Now let's make sure that each blockchain has its own number of blocks
        self.assertEqual(3, len(blockchain1.full_chain))
        self.assertEqual(2, len(blockchain2.full_chain))

        # Then let's replace blockchain1 with blockchain2
        blockchain1.replace_chain(blockchain2.full_chain)

        self.assertEqual(3, len(blockchain1.full_chain))
        self.assertEqual(2, len(blockchain2.full_chain))


if __name__ == '__main__':
    unittest.main(argv=['first-arg-is-ignored'], exit=False)
