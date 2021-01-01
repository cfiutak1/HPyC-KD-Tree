import struct
import numpy as np


def generate_results_file(result_points: np.ndarray, results_file_name: str) -> None:
    num_queries = result_points.shape[0]
    num_neighbors = len(result_points[0])
    num_dimensions = len(result_points[0][0])

    results_file = open(results_file_name, "wb")

    results_file.write(b"RESULT\0\0")
    results_file.write(struct.pack("=q", 0))
    results_file.write(struct.pack("=q", 0))
    results_file.write(struct.pack("=q", 0))
    results_file.write(struct.pack("=q", num_queries))
    results_file.write(struct.pack("=q", num_dimensions))
    results_file.write(struct.pack("=q", num_neighbors))

    for i in range(num_queries):
        for j in range(num_neighbors):
            for k in range(num_dimensions):
                results_file.write(struct.pack("=f", result_points[i][j][k]))

    results_file.close()