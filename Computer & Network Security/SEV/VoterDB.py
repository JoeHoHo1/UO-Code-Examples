# keeps track of all the voter IDs and public keys in use
class VoterDB(dict):
    def __init__(self, voter_ids=[], public_keys=[]):
        self.voter_ids = voter_ids
        self.public_keys = public_keys

    # adds a hashed voter ID and public key to database
    def add_entry(self, voter_id, public_key):
        self.voter_ids.append(voter_id)
        self.public_keys.append(public_key)

    # checks to see if an hashed voter ID is already present in database
    def has_id(self, voter_id):
        for id in self.voter_ids:
            if id == voter_id:
                return True
        return False

    # gets a particular entry from database
    def get_entry(self, index):
        voter_id = self.voter_ids[index]
        public_key = self.public_keys[index]
        return voter_id, public_key