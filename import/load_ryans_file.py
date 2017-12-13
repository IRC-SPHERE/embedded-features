#!/usr/bin/env python3

import sys
import os
import struct

#
# Expected input file format example (tab-separated):
#
# type    timestamp       macaddress      seqno   batteryLevel    rssi    numSamples      s1x     s1y     s1z     s2x     s2y     s2z s3x     s3y     s3z     s4x     s4y     s4z     s5x     s5y     s5z     Gateway
# L       2017-07-12T09:15:29.0157        a0:e6:f8:ad:91:06       402762  240     -74     5       0.875   -0.0625 0.40625 0.90625 -0.0625 0.375   0.90625 -0.0625 0.40625 0.90625 -0.0625 0.375   0.90625 -0.0625 0.375   house3gw4
#

OUT_DIR = "./"

# extract data about this much seconds
NUM_SECONDS = 600

SAMPLING_RATE = 25
PER_LINE = 5
NUM_LINES = NUM_SECONDS * SAMPLING_RATE // PER_LINE

##########################################

def create_out_dir(outdir):
    try:
        os.mkdir(outdir)
    except Exception as ex:
        pass

##########################################

def main():
    create_out_dir(OUT_DIR)
    if len(sys.argv) > 1:
        filename = sys.argv[1]
    else:
        filename = "combinedall.tab"

    with open(filename, "r") as f:
        binaryname = os.path.splitext(filename)[0] + ".bin"
        cname = os.path.splitext(filename)[0] + ".c"
        with open(binaryname, "wb") as outfb:
            sdata = []
            for line in f.readlines()[1:1+NUM_LINES]:
                d = line.strip().split("\t")
                sample = [""] * 3
                for i in range(15):
                    a = d[7 + i]
                    ival = int(float(a) * 32)
                    # write it as 1 byte signed value
                    outfb.write(struct.pack('b', ival))
                    sample[i % 3] = "{: 3d}".format(ival)
                    if (i + 1) % 3 == 0:
                        sdata.append(", ".join(sample))

            s = "  {{" + "}}, \n  {{".join(sdata) + "}}"
            with open(cname, "w") as outfc:
                outfc.write(s)


###########################################

if __name__ == '__main__':
    main()
    print("all done!")

