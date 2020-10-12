#!/usr/bin/python

import argparse
from argparse import RawTextHelpFormatter
import datetime
import struct
import random
# from time import sleep
import sys


def getUniqueFileID():
    now = datetime.datetime.now()
    return int(now.strftime("%d%H%M%S"))


parser = argparse.ArgumentParser(formatter_class=RawTextHelpFormatter)
parser.add_argument("rows", type=int, help="Number of points")
parser.add_argument("cols", type=int, help="Number of dimensions")
parser.add_argument("dist", type=int,help="""Type of distribution\n0: Uniform Distribution\n1: Centered Uniform
        Distribution\n2: Beta distribution\n3: Exponential distribution""",choices=[0, 1, 2,3])
args = parser.parse_args()

datafilename = f"../data/training_{args.rows}_{args.cols}.dat"
# datafilename = "data_" + str(fileid) + ".dat"

fp = open(datafilename, "wb")

cols = args.cols
rows = args.rows
dist = args.dist

# generate file header for datafile
fp.write(b"TRAINING")
fp.write(struct.pack("=q", getUniqueFileID()))
fp.write(struct.pack("=q", args.rows))
fp.write(struct.pack("=q", args.cols))

funcList = [random.uniform, random.gauss, random.betavariate, random.expovariate]
paramList = [(-1000, 1000), (0, 1000), (30, 20), (0.01,)]

for i in range(0, rows):
    buff = bytes()
    for j in range(0, cols):
        buff += struct.pack("=f", funcList[dist](*paramList[dist]))
    fp.write(buff)
fp.close()


print("\nData file : " + datafilename)
