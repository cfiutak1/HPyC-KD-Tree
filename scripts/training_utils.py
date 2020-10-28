import argparse
import struct
import numpy as np
from scripts.shared_file_utils import *


def get_training_generation_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser()
    parser.add_argument("num_points", type=int, help="Number of points")
    parser.add_argument("num_dimensions", type=int, help="Number of dimensions")
    parser.add_argument(
        "distribution",
        type=int,
        help=f"Type of distribution\n"
             f"0: Uniform Distribution\n"
             f"1: Centered Uniform Distribution\n"
             f"2: Beta Distribution\n"
             f"3: Exponential Distribution",
        choices=[0, 1, 2, 3]
    )

    return parser


def generate_training_file(num_points, num_dimensions, distribution) -> None:
    training_file_name = f"./data/training/{distribution_name_map[distribution]}/{num_points}_{num_dimensions}.dat"

    training_fptr = open(training_file_name, "wb")

    # Write training file header
    training_fptr.write(b"TRAINING")
    training_fptr.write(struct.pack("=q", 0))
    training_fptr.write(struct.pack("=q", num_points))
    training_fptr.write(struct.pack("=q", num_dimensions))

    # Write training file data
    for i in range(0, num_points):
        buff = bytes()

        for j in range(0, num_dimensions):
            buff += struct.pack("=f", random_funcs[distribution](*random_func_params[distribution]))

        training_fptr.write(buff)

    training_fptr.close()


def read_training_file(training_file_name: str) -> np.ndarray:
    training_file = open(training_file_name, "rb")

    assert struct.unpack("8s", training_file.read(8))[0].decode("utf-8") == "TRAINING"

    training_file_id = struct.unpack("Q", training_file.read(8))[0]
    num_points = struct.unpack("Q", training_file.read(8))[0]
    num_dimensions = struct.unpack("Q", training_file.read(8))[0]

    points = [struct.unpack("f" * num_dimensions, training_file.read(4 * num_dimensions)) for i in range(num_points)]
    points = np.asarray(points, dtype=np.float32)

    training_file.close()

    return points
