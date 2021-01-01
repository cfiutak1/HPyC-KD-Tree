import hpyc
import numpy as np
import struct
import time
from typing import List
import sklearn.neighbors
import scripts.query_utils
import scripts.training_utils


def run_hpyc_benchmark(tree_size):
    print(tree_size)
    # TODO - The cython code doesn't seem to modify the actual numpy arrays - is that the slowdown?
    training_points = scripts.training_utils.read_training_file(f"./data/training/uniform/{tree_size}_5.dat")
    query_points, num_neighbors = scripts.query_utils.read_query_file("./data/query/uniform/1_5_10.dat")

    print("Done reading data")

    build_start = time.time()
    tree = hpyc.PyKDTree(training_points)
    build_time = time.time() - build_start

    print(build_time)

    query_start = time.time()
    dist, ind = tree.nearest_neighbors(query_points[0], num_neighbors)
    query_time = time.time() - query_start

    return build_time, query_time


def run_sklearn_benchmark(tree_size):
    training_points = scripts.training_utils.read_training_file(f"./data/training/uniform/{tree_size}_5.dat")
    query_points, num_neighbors = scripts.query_utils.read_query_file("./data/query/uniform/1_5_10.dat")

    build_start = time.time()
    tree = sklearn.neighbors.KDTree(training_points)
    build_time = time.time() - build_start

    query_point = np.array(query_points[0]).reshape(1, -1)

    query_start = time.time()
    dist, ind = tree.query(query_point, k=num_neighbors)
    query_time = time.time() - query_start

    return build_time, query_time


def run_benchmarks(benchmark_function, tree_size, num_trials):
    build_times = []
    query_times = []

    for i in range(num_trials):
        b, q = benchmark_function(tree_size)
        build_times.append(b)
        query_times.append(q)

    print(min(build_times), min(query_times), sep=",", end=",")


def main():
    num_trials = 1
    num_points = 1 << 24

    run_benchmarks(run_hpyc_benchmark, num_points, num_trials)
    # run_benchmarks(run_sklearn_benchmark, 1 << i, num_trials)

    print()


if __name__ == "__main__":
    main()



