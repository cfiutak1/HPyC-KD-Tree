import os
import time


NUM_SAMPLES = 20
TRAINING_FILE = "data/training_10000000_5.dat"
QUERY_FILE = "data/query_100000_5_10.dat"
OUTPUT_FILE = f"timings_{time.time()}.txt"
os.system(f"touch {OUTPUT_FILE}")



for i in range(NUM_SAMPLES):
    os.system(f"./program2 1 {TRAINING_FILE} {QUERY_FILE} results.out >> {OUTPUT_FILE}")

timings = {}

f = open(OUTPUT_FILE, "r")

for line in f:
    try:
        metric = line.split(" ")[0]
        timing = float(line.split(" ")[1])

        if metric in timings:
            timings[metric] += timing
        else:
            timings[metric] = timing

    except Exception:
        pass

for item in timings:
    print(item, timings[item] / NUM_SAMPLES)