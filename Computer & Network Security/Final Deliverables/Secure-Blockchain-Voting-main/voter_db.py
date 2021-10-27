"""
A database to keep track of all the voter who have been registered.
"""


class VoterDB:

    def __init__(self):
        self.voter_ids = []
        self.public_keys = []

    # adds a hashed voter ID and public key to database
    def add_entry(self, voter_id, public_key):
        """
        adds entry to database
        :param <String/Int> voter_id:
        :param <Key> public_key:
        """
        self.voter_ids.append(voter_id)
        self.public_keys.append(public_key)

    # checks to see if an hashed voter ID is already present in database
    def has_id(self, voter_id):
        """
        :param <String/Int> voter_id:
        :return <boolean> is in database:
        """
        for v_id in self.voter_ids:
            if v_id == voter_id:
                return True
        return False

    # gets a particular entry from database
    def get_entry(self, index):
        voter_id = self.voter_ids[index]
        public_key = self.public_keys[index]
        return voter_id, public_key
