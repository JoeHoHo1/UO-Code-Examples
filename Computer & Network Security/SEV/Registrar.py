"""
Simulation of registrar
"""
# install cryptography
from cryptography.hazmat.primitives.asymmetric import rsa
import hashlib
import random

# check to see if a voter has the right to vote
# (currently no check, so randomly assign)
def is_valid_voter(voter_id):
    return bool(random.getrandbits(1))

# check database to see if voter has already be registered
def has_registered(voter_id, voter_db):
    return voter_db.has_id(voter_id)

# generate a pair of public and private keys for voter
def generate_key_pair():
    private_key = rsa.generate_private_key(
        public_exponent=65537,
        key_size=2048
    )
    public_key = private_key.public_key()
    return private_key, public_key

# hash a voters ID to put in database
def hash_id(voter_id):
    return hashlib.sha256(bytes(str(voter_id), encoding='utf-8')).hexdigest()

# register voter for voting, rejecting invalid voters
def registrar(voter_id, voter_db):
    if not has_registered(voter_id, voter_db):
        if is_valid_voter(voter_id):
            voter_id_hash = hash_id(voter_id)
            key_pair = generate_key_pair()
            # push a hash of voters ID and public key to voter database
            voter_db.add_entry(voter_id_hash, key_pair[1])
            # send private and public key to voter
            return key_pair
        else:
            print("Invalid voter ID.")
    else:
        print("Voter ID already registered.")