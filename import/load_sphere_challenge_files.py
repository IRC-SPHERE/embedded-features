#!/usr/bin/env python3

import sys
import os
import struct
import copy

#
# Expected input file format example (comma-separated):
#
# t,x,y,z,Kitchen_AP,Lounge_AP,Upstairs_AP,Study_AP
# 0.017856,0.944,-0.28,0.152,-93.0,-95.0,-79.0,
#

OUT_DIR = "../data"

INPUTS = [
    "00001",
    "00002",
    "00003",
    "00004",
    "00005",
    "00007",
]

# extract data about this much seconds
NUM_SECONDS = 1500

SAMPLING_RATE = 20

PER_SAMPLE = 1.0 / SAMPLING_RATE

NUM_SAMPLES = NUM_SECONDS * SAMPLING_RATE

SCALING_FACTOR = 128 // 4
MAX_VAL = 127
MIN_VAL = -128

##########################################

def create_out_dir(outdir):
    try:
        os.mkdir(outdir)
    except Exception as ex:
        pass

def scale(x):
    xi = int(round(float(x) * SCALING_FACTOR, 0))
    if xi > MAX_VAL:
        print("bad value", x)
        xi = MAX_VAL
    elif xi < MIN_VAL:
        print("bad value", x)
        xi = MIN_VAL
    return xi

##########################################

def load_file(filename):
    with open(filename, "r") as f:

        tdata = []
        sdata = []
        oldt = None
        oldv = [0, 0, 0]
        total = 0
        totalmissing = 0
        for line in f.readlines()[1:]:
            d = line.strip().split(",")
            t = float(d[0])
            x = d[1]
            y = d[2]
            z = d[3]
            v = list(map(scale, [x, y, z]))

            if oldt is None:
                oldt = t - 0.05
            nummissing = 0
            oldoldt = oldt
            while oldt + 0.05 + 0.002 < t:
                #print("missing sample before t=", t, oldt)
                nummissing += 1
                total += 1
                totalmissing += 1
                oldt += 0.05
                sdata.append(oldv)
                tdata.append(oldt)

                if total >= NUM_SAMPLES:
                    break

#            if nummissing:
#                print("missing before t=", t, " num=", nummissing, " old=", (oldoldt + 0.05))

            total += 1

            oldt = t
            oldv = copy.copy(v)
            sdata.append(v) #", ".join(map(str, v)))
            tdata.append(t)

            if total >= NUM_SAMPLES:
                break

        print("pdr: {:0.6f}".format(100.0 - 100.0 * float(totalmissing) / total))
        print("t=", t)

        return sdata, tdata

##########################################

def main():
    create_out_dir(OUT_DIR)

    for dirname in INPUTS:
        filename = "train/" + dirname + "/acceleration.csv"
        frmt = "{}/{}-{}.c"
        outfilename1 = frmt.format(OUT_DIR, dirname, 1)
        outfilename2 = frmt.format(OUT_DIR, dirname, 2)
        
        sdata, tdata = load_file(filename)

        outdata = []
        for v in sdata:
            outdata.append(", ".join(map(str, v)))

        first_part  = outdata[:NUM_SAMPLES//2]
        second_part = outdata[NUM_SAMPLES//2:]

        s1 = "  {{" + "}}, \n  {{".join(first_part) + "}}"
        s2 = "  {{" + "}}, \n  {{".join(second_part) + "}}"
        with open(outfilename1, "w") as outfc:
            outfc.write(s1)
        with open(outfilename2, "w") as outfc:
            outfc.write(s2)

###########################################

if __name__ == '__main__':
    main()
    print("all done!")

