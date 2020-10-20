import hpyc
import numpy as np
import struct
import time
from typing import List
import sklearn.neighbors


def read_training_file(training_file_name: str) -> (int, List[List[float]]):
    training_file = open(training_file_name, "rb")

    assert struct.unpack("8s", training_file.read(8))[0].decode("utf-8") == "TRAINING"

    training_file_id = struct.unpack("Q", training_file.read(8))[0]

    num_points = struct.unpack("Q", training_file.read(8))[0]
    num_dimensions = struct.unpack("Q", training_file.read(8))[0]

    points = [struct.unpack("f" * num_dimensions, training_file.read(4 * num_dimensions)) for i in range(num_points)]
    points = np.asarray(points)

    training_file.close()

    return training_file_id, points


def read_query_file(query_file_name: str) -> (int, int, List[List[float]]):
    query_file = open(query_file_name, "rb")

    assert struct.unpack("8s", query_file.read(8))[0].decode("utf-8") == "QUERY\0\0\0"

    query_file_id = struct.unpack("Q", query_file.read(8))[0]
    num_queries = struct.unpack("Q", query_file.read(8))[0]
    num_dimensions = struct.unpack("Q", query_file.read(8))[0]
    num_neighbors = struct.unpack("Q", query_file.read(8))[0]

    points = [struct.unpack("f" * num_dimensions, query_file.read(4 * num_dimensions)) for i in range(num_queries)]
    points = np.asarray(points)

    query_file.close()

    return query_file_id, num_neighbors, points


def run_hpyc_benchmark(tree_size):
    # TODO - The cython code doesn't seem to modify the actual numpy arrays - is that the slowdown?
    id, training_points = read_training_file(f"/home/colin/HPyC-KD-Tree/data/training_{tree_size}_5.dat")
    id, num_neighbors, query_points = read_query_file("/home/colin/HPyC-KD-Tree/data/query_1_5_10.dat")

    build_start = time.time()
    tree = hpyc.PyKDTree(training_points)
    build_time = time.time() - build_start

    query_start = time.time()
    dist, ind = tree.nearest_neighbors(query_points[0], num_neighbors)
    query_time = time.time() - query_start

    return build_time, query_time


def run_sklearn_benchmark(tree_size):
    id, training_points = read_training_file(f"/home/colin/HPyC-KD-Tree/data/training_{tree_size}_5.dat")
    id, num_neighbors, query_points = read_query_file("/home/colin/HPyC-KD-Tree/data/query_1_5_10.dat")

    build_start = time.time()
    tree = sklearn.neighbors.KDTree(np.array(training_points))
    build_time = time.time() - build_start

    query_start = time.time()
    dist, ind = tree.query(np.array(query_points[0]).reshape(1, -1), k=num_neighbors)
    query_time = time.time() - query_start

    return build_time, query_time


def run_benchmarks(benchmark_function, tree_size):
    build_times = []
    query_times = []

    for i in range(200):
        b, q = benchmark_function(tree_size)
        build_times.append(b)
        query_times.append(q)

    print(min(build_times), min(query_times), sep=",", end=",")


def main():
    for i in range(19, 26):
        run_benchmarks(run_hpyc_benchmark, 1 << i)
        run_benchmarks(run_sklearn_benchmark, 1 << i)

        print()


if __name__ == "__main__":
    main()


