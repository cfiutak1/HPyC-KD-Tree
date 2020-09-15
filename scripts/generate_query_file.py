#!/usr/bin/python

import argparse
from argparse import RawTextHelpFormatter
import datetime
import struct
import random
from time import sleep
import sys

def getUniqueFileID():
    now = datetime.datetime.now()
    return int(now.strftime("%d%H%M%S"))

parser = argparse.ArgumentParser(formatter_class=RawTextHelpFormatter)
parser.add_argument("queryCount", type=int,help="Number of queries")
parser.add_argument("cols", type=int,help="Number of dimensions")
parser.add_argument("dist", type=int,help="""Type of distribution\n0: Uniform Distribution\n1: Centered Uniform
        Distribution\n2: Beta distribution\n3: Exponential distribution""",choices=[0, 1, 2,3])
parser.add_argument("k", type=int,help="Number of neighbors to return for each query")
args = parser.parse_args()

fileid = getUniqueFileID()
queryfilename = "query_" + str(fileid) + ".dat"

dp = open(queryfilename,"wb")

cols = args.cols
dist = args.dist
queryCount = args.queryCount
k = args.k

#header for query file
dp.write("QUERY")
dp.write(struct.pack("=h",0))
dp.write(struct.pack("=B",0))
dp.write(struct.pack("=q",fileid))
dp.write(struct.pack("=q",queryCount))
dp.write(struct.pack("=q",args.cols))
dp.write(struct.pack("=q",k))

funcList = [random.uniform,random.gauss,random.betavariate,random.expovariate]
paramList = [(-1000,1000),(0,1000),(30,20),(0.01,)]

# Generating data for query file
part = int(queryCount/100)
print("\nQuery file generation progress:")
for i in range(0,queryCount):
    #progress bar
    if(part!=0 and i%part==0):
        sys.stdout.write('\r')
        sys.stdout.write("[%-100s] %d%%" % ('='*(i/part), (i/part)))
        sys.stdout.flush()
    buff = str()
    for j in range(0,cols):
        buff+=struct.pack("=f",funcList[dist](*paramList[dist]))
    dp.write(buff)
dp.close()

print("query file : " + queryfilename);
