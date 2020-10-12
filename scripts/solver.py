import struct
import sys
from typing import List, Tuple
import sklearn.neighbors
import datetime
import numpy as np
import time


def read_training_file(training_file_name: str) -> (int, List[List[float]]):
    training_file = open(training_file_name, "rb")

    assert struct.unpack("8s", training_file.read(8))[0].decode("utf-8") == "TRAINING"

    training_file_id = struct.unpack("Q", training_file.read(8))[0]
    # print(training_file_id)
    num_points = struct.unpack("Q", training_file.read(8))[0]
    num_dimensions = struct.unpack("Q", training_file.read(8))[0]

    # print(num_points, type(num_points))

    points = [struct.unpack("f" * num_dimensions, training_file.read(4 * num_dimensions)) for i in range(num_points)]

    training_file.close()

    return training_file_id, points


def read_query_file(query_file_name: str) -> (int, List[List[float]]):
    query_file = open(query_file_name, "rb")
    # print(struct.unpack("8s", query_file.read(8))[0])

    assert struct.unpack("8s", query_file.read(8))[0].decode("utf-8") == "QUERY\0\0\0"

    query_file_id = struct.unpack("Q", query_file.read(8))[0]
    num_queries = struct.unpack("Q", query_file.read(8))[0]
    num_dimensions = struct.unpack("Q", query_file.read(8))[0]
    num_neighbors = struct.unpack("Q", query_file.read(8))[0]

    points = [struct.unpack("f" * num_dimensions, query_file.read(4 * num_dimensions)) for i in range(num_queries)]

    query_file.close()

    return query_file_id, num_neighbors, points


def read_result_file(result_file_name: str):
    result_file = open(result_file_name, "rb")
    # print(struct.unpack("8s", query_file.read(8))[0])

    print(struct.unpack("8s", result_file.read(8))[0].decode("utf-8"))
    print(struct.unpack("Q", result_file.read(8))[0])
    print(struct.unpack("Q", result_file.read(8))[0])
    print(struct.unpack("Q", result_file.read(8))[0])
    print(struct.unpack("Q", result_file.read(8))[0])
    print(struct.unpack("Q", result_file.read(8))[0])



def generate_results_file(training_points: List[List[float]], query_points: List[List[float]], file_ids: Tuple[int], num_neighbors: int, results_file_name: str) -> None:
    distance_metric = sklearn.neighbors.DistanceMetric.get_metric("euclidean")
    t = time.time()
    tree = sklearn.neighbors.KDTree(np.array(training_points), metric=distance_metric)
    print(f"sklearn construction took {time.time() - t}")

    results_file = open(results_file_name, "wb")

    results_file.write(b"RESULT\0\0")
    results_file.write(struct.pack("=q", file_ids[0]))
    results_file.write(struct.pack("=q", file_ids[1]))
    results_file_id = int(datetime.datetime.now().strftime("%d%H%M%S"))
    results_file.write(struct.pack("=q", 69))
    num_queries = len(query_points)
    results_file.write(struct.pack("=q", num_queries))
    num_dimensions = len(query_points[0])
    results_file.write(struct.pack("=q", num_dimensions))
    results_file.write(struct.pack("=q", num_neighbors))

    t = time.time()
    for i in range(num_queries):
        dist, ind = tree.query(np.array(query_points[i]).reshape(1, -1), k=num_neighbors)

        # print(ind[0])
        # print(list(ind))

        for index in ind[0]:
            for dim in range(num_dimensions):
                results_file.write(struct.pack("=f", training_points[index][dim]))

    print(f"sklearn queries and file IO took {time.time() - t}")


    results_file.close()


def generate_results_file_one_query(training_points: List[List[float]], query_points: List[List[float]], file_ids: Tuple[int], num_neighbors: int, results_file_name: str) -> None:
    distance_metric = sklearn.neighbors.DistanceMetric.get_metric("euclidean")
    t = time.time()
    tree = sklearn.neighbors.KDTree(np.array(training_points), metric=distance_metric)
    build_time = time.time() - t

    results_file = open(results_file_name, "wb")

    results_file.write(b"RESULT\0\0")
    results_file.write(struct.pack("=q", file_ids[0]))
    results_file.write(struct.pack("=q", file_ids[1]))
    results_file_id = int(datetime.datetime.now().strftime("%d%H%M%S"))
    results_file.write(struct.pack("=q", 69))
    num_queries = len(query_points)
    results_file.write(struct.pack("=q", num_queries))
    num_dimensions = len(query_points[0])
    results_file.write(struct.pack("=q", num_dimensions))
    results_file.write(struct.pack("=q", num_neighbors))

    t = time.time()

    dist, ind = tree.query(np.array(query_points[0]).reshape(1, -1), k=num_neighbors)

    # print(ind[0])
    # print(list(ind))
    print(f"{len(training_points)}, {build_time}, {time.time() - t}")

    for index in ind[0]:
        for dim in range(num_dimensions):
            results_file.write(struct.pack("=f", training_points[index][dim]))

    # print(f"sklearn query and file IO took {time.time() - t}")


    results_file.close()

def main():
    if len(sys.argv) != 4:
        print("Error: Program requires 3 arguments")
        exit(1)

    training_file_name = sys.argv[1]
    query_file_name = sys.argv[2]
    results_file_name = sys.argv[3]

    training_file_id, training_points = read_training_file(training_file_name)
    query_file_id, num_neighbors, query_points = read_query_file(query_file_name)

    generate_results_file_one_query(training_points, query_points, (training_file_id, query_file_id), num_neighbors, results_file_name)





if __name__ == "__main__":
    main()