"""
A test of the voter registration system by simulating possible registrants.
"""
from time import sleep
import base64
import random
import sys
import queue
from cryptography.exceptions import InvalidSignature
from cryptography.exceptions import UnsupportedAlgorithm
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import padding
from voter_db import VoterDB
from registrar import registrar
from blockchain import Blockchain
from threads import ThreadWithReturnValue


ATTEMPTS = 40
VALID_VOTERS = 20


# helper methods
def sim_reg():
    """
    Multithreaded (for loading effect) simulation of creation of chain, creation of database, transactions,
    mining, voting, registering candidates and more
    """
    # init
    sleep(1)
    blockchain = Blockchain()
    database = VoterDB()
    blockchain.add_candidate('recipient 1')
    blockchain.add_candidate('recipient 2')
    blockchain.add_candidate('recipient 3')
    # redirect stdout
    buf = ""
    # simulate 100 attempts to register/vote
    for i in range(ATTEMPTS):
        v_id = random.randrange(1, VALID_VOTERS)
        buf += "\n--------------------------------------------------------------\n" + \
               "Attempt for ID: " + str(v_id) + \
               "\n--------------------------------------------------------------"
        private_key, public_key, msg = registrar(v_id, database)
        buf += "\n" + msg
        buf += "\nPrivate Key Object (Usually only shown to voter): " + str(private_key)
        buf += "\nPublic Key Object: " + str(public_key)
        # If registration is successful
        if not private_key:
            buf += "\nDon't Vote\n"
        else:
            rec_num = random.randrange(1, 6)
            plain_text = str(public_key) + ', recipient '+str(rec_num)+str(1)
            try:
                signature = private_key.sign(
                    data=plain_text.encode('utf-8'),
                    padding=padding.PSS(
                        mgf=padding.MGF1(hashes.SHA256()),
                        salt_length=padding.PSS.MAX_LENGTH
                    ),
                    algorithm=hashes.SHA256()

                )
                buf += "\nSignature: " + str(base64.urlsafe_b64encode(signature))

                # VERIFY JUST CREATED SIGNATURE USING PUBLIC KEY
                try:
                    public_key.verify(
                        signature=signature,
                        data=plain_text.encode('utf-8'),
                        padding=padding.PSS(
                            mgf=padding.MGF1(hashes.SHA256()),
                            salt_length=padding.PSS.MAX_LENGTH
                        ),
                        algorithm=hashes.SHA256()
                    )
                    is_signature_correct = True
                except InvalidSignature:
                    is_signature_correct = False

                buf += "\nSignature is correct: " + str(is_signature_correct)
            except UnsupportedAlgorithm:
                buf += "\nSigning failed"

            buf += "\nAttempting to vote for recipient " + str(rec_num)
            _, successful = blockchain.create_transaction(public_key, 'recipient '+str(rec_num), signature, 1)
            if successful:
                trans = blockchain.last_transaction
                buf += "\nSender: " + str(trans.sender)
                buf += "\nRecipient: " + trans.recipient
                buf += "\nSignature in transaction: " + str(base64.urlsafe_b64encode(trans.signature)) + "\n"
            else:
                buf += "\nInvalid candidate (only candidates 1 - 3 are registered), vote failed\n"

            buf += "Pending transactions: " + str(blockchain.pending_transactions)
            buf += "\nmining\n"
            blockchain.mine()
            buf += "Pending transactions: " + str(blockchain.pending_transactions) + "\n"

    sleep(1)
    return buf

def animate(process):
    """
    adds loading animation while processing a task
    :param <process> long task:
    """
    while process.is_alive():
        chars = "⢿⣻⣽⣾⣷⣯⣟⡿"
        for char in chars:
            sys.stdout.write('\r' + 'loading ' + char)
            sleep(.1)
            sys.stdout.flush()
    sys.stdout.write('\rFinished!     ')


print("This is a simulation of multiple users attempting to register and vote.")
sleep(.2)
      
print("-------------------------------------------------------------------------------")
sleep(.2)
print();
sleep(.2)
print("Please comment out the Sim() function as designated in Main for actual use.")
sleep(.2)
print("As the valid voters are randomized, entering the same ID more than once may")
sleep(.2)
print("result in a user being \"valid\" one time and not another. Use in conjunction with")
sleep(.2)
print("an actual verification system to avoid this issue.")
sleep(.2)
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
big_str = t.join()
# helps visualize scope of print out when line by line, feel free
# to just print big_str instead of this loop 
for line in big_str.splitlines():
    print(line)
    sleep(.03)

