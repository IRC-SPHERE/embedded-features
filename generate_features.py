#!/usr/bin/python3

import sys
import os
import subprocess

SELF_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(SELF_DIR, "import"))

import load_sphere_challenge_files
import postprocess

##########################################

OUT_DIR = "export/"
DATA_DIR = "import/train/"
FEATURE_TEST_DATA_DIR = "data/"

TEMP_OUTPUT_FILE = "output.txt"

##########################################

def create_out_dir(outdir):
    try:
        os.mkdir(outdir)
    except Exception as ex:
        print(ex)
        pass

##########################################

def main():
    create_out_dir(OUT_DIR)
    create_out_dir(FEATURE_TEST_DATA_DIR)

    for dirname in load_sphere_challenge_files.INPUTS:
        filename = DATA_DIR + dirname + "/acceleration_corrected.csv"
        sdata, tdata = load_sphere_challenge_files.load_file(filename)

        outdata = map(lambda v: ", ".join(map(str, v)), sdata)

        s = "  {{" + "}}, \n  {{".join(outdata) + "}}"
        coutfilename = SELF_DIR + "/" + FEATURE_TEST_DATA_DIR + dirname + ".c"
        with open(coutfilename, "w") as outfc:
            outfc.write(s)

        os.environ["FEATURE_TEST_INPUT"] = coutfilename
        os.environ["DO_LOG_OUTPUT"] = "1"
        os.environ["ARCHITECTURE"] = "native"

        # compile and run on the specific input data
        for window_size in [32, 64, 128]:
            out_dirname = OUT_DIR + "window_size_" + str(window_size) + "/"
            create_out_dir(out_dirname)

            os.chdir(SELF_DIR + "/feature-test")
            os.environ["WINDOW_SIZE"] = str(window_size)
            out = subprocess.check_output("make run", shell=True)
            os.chdir(SELF_DIR)

            # output the generated input data to a file
            with open(TEMP_OUTPUT_FILE, "wb") as outfile:
                outfile.write(out)

            # process the temporary output file and produce the .csv
            csvfilename = out_dirname + dirname + ".csv"
            postprocess.build_csv(sdata, tdata, csvfilename, TEMP_OUTPUT_FILE, window_size)

            # remove the temporary output file
            try:
                os.remove(TEMP_OUTPUT_FILE)
            except Exception as ex:
                print(ex)
                pass

###########################################

if __name__ == '__main__':
    main()
    print("all done!")
