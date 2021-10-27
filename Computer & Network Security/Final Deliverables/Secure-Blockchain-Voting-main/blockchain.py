"""
The voting blockchain system that allows all votes to be publicly verifiable.
"""
import hashlib
from transaction import Transaction
from voting_block import VBlock


class Blockchain:

    def __init__(self):
        self.__current_transactions = []
        self.__chain = []
        self.__candidates = []
        # Create genesis block
        self.create_genesis()

    def create_genesis(self):
        """
        Creates the Genesis block and passes it to the chain
        :return: None
        """
        genesis_block = VBlock(0, self.__current_transactions, '00', 0, '00')
        self.__chain.append(genesis_block)

    def add_candidate(self, recipient_add):
        """
        Creates a new block and passes it to the chain
        :param recipient_add:
        """
        self.__candidates.append(recipient_add)

    def add_block(self, block):
        """
        Creates a new block and passes it to the chain
        :param block: <Block> Block to add to the chain
        :return: <bool> True if the block is added to the chain, False if not.
        """
        if self.validate_block(block, self.last_block):
            self.__chain.append(block)

            # Remove transactions from the list
            self.__current_transactions = []

            return True

        return False

    def create_transaction(self, sender, recipient, signature, amount=1):
        """
        Creates a new transaction to go into the next block
        :param sender: <str> sender address
        :param recipient: <str> recipient address
        :param amount: <float> amount
        :param signature: <signature> encrypted with private key
        :return: <Transaction> generated transaction
        """
        candidate = recipient in self.candidate_list
        transaction = Transaction(sender, recipient, candidate, signature, amount)
        if transaction.validate():
            self.__current_transactions.append(transaction)

            return transaction, True

        return None, False

    def mine(self):
        """
        Mines a new block into the chain
        :return: result of the mining attempt and the new block
        """
        last_block = self.last_block
        index = last_block.index + 1
        previous_hash = last_block.hash

        # Let's start with the heavy duty, generating the proof of work
        nonce = self.generate_proof_of_work(last_block)

        if self.__current_transactions.__len__() == 0:
            signature = "0"
        else:
            signature = self.last_transaction.signature
        # Add the block to the new chain
        block = VBlock(index, self.__current_transactions, signature, nonce, previous_hash)

        if self.add_block(block):
            return block

        return None

    @staticmethod
    def validate_proof_of_work(last_nonce, last_hash, nonce):
        """
        Validates the nonce
        :param last_nonce: <int> Nonce of the last block
        :param nonce: <int> Current nonce to be validated
        :param last_hash: <str> Hash of the last block
        :return: <bool> True if correct, False if not.
        """
        sha = hashlib.sha256(f'{last_nonce}{last_hash}{nonce}'.encode())
        return sha.hexdigest()[:4] == '0000'

    def generate_proof_of_work(self, block):
        """
        Very simple proof of work algorithm:
        - Find a number 'p' such that hash(pp') contains 4 leading zeroes
        - Where p is the previous proof, and p' is the new proof
        :param block: <Block> reference to the last block object
        :return: <int> generated nonce
        """
        last_nonce = block.nonce
        last_hash = block.hash

        nonce = 0
        while not self.validate_proof_of_work(last_nonce, last_hash, nonce):
            nonce += 1

        return nonce

    def validate_block(self, current_block, previous_block):
        """
        Validates a block with reference to its previous
        :param current_block:
        :param previous_block:
        :return:
        """
        # Check the block index
        if current_block.index != previous_block.index + 1:
            return False

        if current_block.previous_hash != previous_block.hash:
            return False

        if current_block.hash != current_block.hash_block():
            return False

        if not self.validate_proof_of_work(previous_block.nonce, previous_block.hash, current_block.nonce):
            return False

        return True

    def validate_chain(self, chain_to_validate):
        """
        Verifies if a given chain is valid
        :param chain_to_validate: <[Block]>
        :return: <bool> True if the chain is valid
        """
        # First validate both genesis blocks
        if chain_to_validate[0].hash_block() != self.__chain[0].hash_block():
            return False

        # Then we compare each block with its previous one
        for x in range(1, len(chain_to_validate)):
            if not self.validate_block(chain_to_validate[x], chain_to_validate[x - 1]):
                return False

        return True

    def replace_chain(self, new_chain):
        """
        Attempts to replace the chain with a new one
        :param new_chain:
        :return: <bool> True if the chain was replaced, False if not.
        """
        # We only replace if the new chain is bigger than the current one
        if len(new_chain) <= len(self.__chain):
            return False

        # Validate the new chain
        if not self.validate_chain(new_chain):
            return False

        new_blocks = new_chain[len(self.__chain):]
        for block in new_blocks:
            self.add_block(block)

    @property
    def last_block(self):
        return self.__chain[-1]

    @property
    def last_transaction(self):
        return self.__current_transactions[-1]

    @property
    def pending_transactions(self):
        return self.__current_transactions

    @property
    def candidate_list(self):
        return self.__candidates

    @property
    def full_chain(self):
        return self.__chain
