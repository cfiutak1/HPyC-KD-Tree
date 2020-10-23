import argparse
import struct
import scripts.shared_file_utils
import numpy as np


def get_query_generation_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser()
    parser.add_argument("num_points", type=int, help="Number of queries")
    parser.add_argument("num_dimensions", type=int, help="Number of dimensions")
    parser.add_argument("num_neighbors", type=int, help="Number of neighbors to return for each query")
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


def generate_query_file(query_args: argparse.ArgumentParser):
    args = query_args.parse_args()

    query_file_name = f"../data/query/{distribution_name_map[args.distribution]}/{args.num_points}_{args.num_dimensions}_{args.num_neighbors}.dat"

    query_fptr = open(query_file_name, "wb")

    # Write query file header
    query_fptr.write(b"QUERY\0\0\0")
    query_fptr.write(struct.pack("=q", 0))
    query_fptr.write(struct.pack("=q", args.num_points))
    query_fptr.write(struct.pack("=q", args.num_dimensions))
    query_fptr.write(struct.pack("=q", args.num_neighbors))

    # Write query file data
    for i in range(0, args.num_points):
        buff = bytes()

        for j in range(0, args.num_dimensions):
            buff += struct.pack("=f", random_funcs[args.distribution](*random_func_params[args.distribution]))

        query_fptr.write(buff)

    query_fptr.close()


def read_query_file(query_file_name: str) -> (np.ndarray, int):
    query_file = open(query_file_name, "rb")

    assert struct.unpack("8s", query_file.read(8))[0].decode("utf-8") == "QUERY\0\0\0"

    query_file_id = struct.unpack("Q", query_file.read(8))[0]
    num_queries = struct.unpack("Q", query_file.read(8))[0]
    num_dimensions = struct.unpack("Q", query_file.read(8))[0]
    num_neighbors = struct.unpack("Q", query_file.read(8))[0]

    points = [struct.unpack("f" * num_dimensions, query_file.read(4 * num_dimensions)) for i in range(num_queries)]
    points = np.asarray(points)

    query_file.close()

    return points, num_neighbors