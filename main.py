import kdtree
import numpy as np
import struct
import time
from typing import List


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

t = time.time()
id, points = read_training_file("data/training_16777216_5.dat")
print("File IO took", time.time() - t)

# points = [[1,2,3], [4,5,6]]

# a = np.array(points)

# print(a.dtype)
# print(a.dtype == np.int64)
# print(isinstance(a.dtype, np.int64))

t = time.time()
tree = kdtree.PyKDTree(points)

print(time.time() - t)

# q = [0,0,1]

