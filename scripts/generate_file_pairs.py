import os

NUM_DIMENSIONS = 3
DISTRIBUTION = 0
NUM_NEIGHBORS = 10

for i in range(4, 26):
    os.system(f"python3 generate_training_file.py {1 << i} {NUM_DIMENSIONS} {DISTRIBUTION}")
    # os.system(f"python3 generate_query_file.py {1 << i} {NUM_DIMENSIONS} {DISTRIBUTION} {NUM_NEIGHBORS}")