import os
import sys

##############################################################

# once per second (independent on the window size used for calculation)
PROCESSING_WINDOW_SIZE = 20


NUM_TIME_HISTOGRAM_BINS = 16
NUM_FREQUENCY_HISTOGRAM_BINS = 9

N = 0

AXIS = ["x", "y", "z"]

##############################################################

def create_subs(names):
    r = {}
    for n in names:
        r[n] = [""] * N
    return r

def axis_conversion_function(lines, window_size, data_window_size):
    subs = create_subs(AXIS)
    line_counter = 0 if window_size == 1 else data_window_size - 1
    axis = -1
    for line in lines:
        if "axis" in line:
            axis += 1
            line_counter = 0 if window_size == 1 else data_window_size - 1
            continue
        if line_counter >= N:
            print("line counter out of range:", line_counter)
            break
        subs[AXIS[axis]][line_counter] = line
        line_counter += window_size
    return subs

def axis_fft_conversion_function(lines, window_size, data_window_size):
    subs = {}
    for a in AXIS:
        for bin in range(data_window_size // 2 + 1):
            name = a + "_" + str(bin)
            subs[name] = [""] * N

    line_counter = 0 if window_size == 1 else data_window_size - 1
    axis = -1
    frequency = 0
    num_frequencies = data_window_size // 2 + 1
    for line in lines:
        if "axis" in line:
            axis += 1
            line_counter = 0 if window_size == 1 else data_window_size - 1
            frequency = 0
            continue
        if line_counter >= N:
            print("line counter out of range:", line_counter)
            break
        name = AXIS[axis] + "_" + str(frequency)
        subs[name][line_counter] = line
        frequency += 1
        if frequency >= num_frequencies:  # this is the number of samples
            # start from DC again
            frequency = 0
            line_counter += window_size
    return subs

def simple_conversion_function(lines, window_size, data_window_size):
    subs = create_subs([""])
    line_counter = 0 if window_size == 1 else data_window_size - 1
    for line in lines:
        if line_counter >= N:
            print("line counter out of range:", line_counter)
            break
        subs[""][line_counter] = line
        line_counter += window_size
    return subs

def histogram_conversion_function(lines, window_size, num_bins, data_window_size):
    subs = {}
    for a in AXIS:
        for bin in range(num_bins):
            name = a + "_" + str(bin)
            subs[name] = [""] * N

    line_counter = 0 if window_size == 1 else data_window_size - 1
    axis = -1
    for line in lines:
        if "axis" in line:
            axis += 1
            line_counter = 0 if window_size == 1 else data_window_size - 1
            continue
        bins = line.split()
        for bin in range(num_bins):
            name = AXIS[axis] + "_" + str(bin)
            subs[name][line_counter] = bins[bin]
        line_counter += window_size
    return subs

##############################################################

def time_axis_conversion_function(lines, data_window_size):
    return axis_conversion_function(lines, 1, data_window_size)

def time_simple_conversion_function(lines, data_window_size):
    return simple_conversion_function(lines, 1, data_window_size)

def time_axis_periodic_conversion_function(lines, data_window_size):
    return axis_conversion_function(lines, PROCESSING_WINDOW_SIZE, data_window_size)

def time_simple_periodic_conversion_function(lines, data_window_size):
    return simple_conversion_function(lines, PROCESSING_WINDOW_SIZE, data_window_size)

def time_histogram_conversion_function(lines, data_window_size):
    return histogram_conversion_function(lines, PROCESSING_WINDOW_SIZE, NUM_TIME_HISTOGRAM_BINS, data_window_size)

##############################################################

def frequency_axis_conversion_function(lines, data_window_size):
    return axis_conversion_function(lines, 1, data_window_size)

def frequency_axis_fft_conversion_function(lines, data_window_size):
    return axis_fft_conversion_function(lines, PROCESSING_WINDOW_SIZE, data_window_size)

def frequency_simple_conversion_function(lines, data_window_size):
    return simple_conversion_function(lines, 1, data_window_size)

def frequency_axis_periodic_conversion_function(lines, data_window_size):
    return axis_conversion_function(lines, PROCESSING_WINDOW_SIZE, data_window_size)

def frequency_simple_periodic_conversion_function(lines, data_window_size):
    return simple_conversion_function(lines, PROCESSING_WINDOW_SIZE, data_window_size)

def frequency_histogram_conversion_function(lines, data_window_size):
    return histogram_conversion_function(lines, PROCESSING_WINDOW_SIZE, NUM_FREQUENCY_HISTOGRAM_BINS, data_window_size)

##############################################################

class Feature:
    def __init__(self, name, conversion_function):
        self.name = name
        self.conversion_function = conversion_function
        self.lines = []
        self.subs = {"" : []}

    def add_line(self, line):
        self.lines.append(line.strip())

    def process(self, data_window_size):
        print("Convert", self.name)
        self.subs = self.conversion_function(self.lines, data_window_size)

    def get_subnames(self):
        return map(lambda x: self.name + " " + x, sorted(self.subs.keys()))

    def get_values(self, i):
        row = []
        for sname in sorted(self.subs.keys()):
            row.append(self.subs[sname][i])
        return row

##############################################################

features = []

def add_feature(f):
    features.append(f)

def get_feature(name):
    for f in features:
        if f.name == name: return f
    return None

def initialize():
    global features
    features = []

    if False:
        # nonperiodic features
        add_feature(Feature("mean (f)", time_axis_conversion_function))
        add_feature(Feature("mean (i)", time_axis_conversion_function))
        add_feature(Feature("quartile_25 (i)", time_axis_conversion_function))
        add_feature(Feature("median (i)", time_axis_conversion_function))
        add_feature(Feature("quartile_75 (i)", time_axis_conversion_function))
        add_feature(Feature("min (i)", time_axis_conversion_function))
        add_feature(Feature("max (i)", time_axis_conversion_function))
        add_feature(Feature("variance (i)", time_axis_conversion_function))
        add_feature(Feature("std (f)", time_axis_conversion_function))
        add_feature(Feature("0-crossings (i)", time_axis_conversion_function))
        add_feature(Feature("entropy (f)", time_axis_conversion_function))

    if True:
        # periodic features + magnitude
        add_feature(Feature("mean (i, p)", time_axis_periodic_conversion_function))
        add_feature(Feature("mean (f, p)", time_axis_periodic_conversion_function))
        add_feature(Feature("quartile_25 (i, p)", time_axis_periodic_conversion_function))
        add_feature(Feature("median (i, p)", time_axis_periodic_conversion_function))
        add_feature(Feature("quartile_75 (i, p)", time_axis_periodic_conversion_function))
        add_feature(Feature("min (i, p)", time_axis_periodic_conversion_function))
        add_feature(Feature("max (i, p)", time_axis_periodic_conversion_function))
        add_feature(Feature("variance (i, p)", time_axis_periodic_conversion_function))
        add_feature(Feature("std (f, p)", time_axis_periodic_conversion_function))
        add_feature(Feature("0-crossings (i, p)", time_axis_periodic_conversion_function))
        add_feature(Feature("entropy (f, p)", time_axis_periodic_conversion_function))
        add_feature(Feature("histogram (i, p)", time_histogram_conversion_function))
        add_feature(Feature("magnitude (f)", time_simple_conversion_function))
        add_feature(Feature("magnitude^2 (i)", time_simple_conversion_function))

    if True:
        # Note: all of these are periodic in the sense that FFT / integer FFT is done once per window size
        add_feature(Feature("spectral density (i)", frequency_axis_fft_conversion_function))
        add_feature(Feature("spectral density (f)", frequency_axis_fft_conversion_function))
        add_feature(Feature("spectral maxima (i)", frequency_axis_periodic_conversion_function))
        add_feature(Feature("spectral maxima (f)", frequency_axis_periodic_conversion_function))
        add_feature(Feature("spectral centroid (f)", frequency_axis_periodic_conversion_function))
        add_feature(Feature("spectral flux (f)", frequency_axis_periodic_conversion_function))
        add_feature(Feature("spectral entropy (f)", frequency_axis_periodic_conversion_function))
        add_feature(Feature("spectral magn. area (f)", frequency_simple_periodic_conversion_function))
        add_feature(Feature("spectral magn. area^2 (i)", frequency_simple_periodic_conversion_function))
        add_feature(Feature("spectral histogram (i)", frequency_histogram_conversion_function))
        add_feature(Feature("spectral histogram (f)", frequency_histogram_conversion_function))


def build_csv(sdata, tdata, csvfilename, rawfilename, data_window_size):
    global N

    N = len(sdata)
    
    initialize()

    current_feature = None
    
    with open(rawfilename, "r") as infile:
        for line in infile:
            if "Start feature:" in line:
                # start of a feature block
                fname = line[14:].strip()
                current_feature = get_feature(fname)
                continue
            if "Feature:" in line:
                # end of a feature block
                current_feature = None
                continue
            if current_feature:
                current_feature.add_line(line)

        # the column titles
        names = ["t", "x", "y", "z"]

        for f in features:
            f.process(data_window_size)
            names += f.get_subnames()
    
        with open(csvfilename, "w") as outfile:
            # write the first line: the column titles
            outfile.write("\t".join(names) + "\n")

            for i in range(N):
                #for s, t in zip(sdata, tdata):
                s = sdata[i]
                t = tdata[i]
                row = [t, s[0], s[1], s[2]]
                for f in features:
                    row += f.get_values(i)
                outfile.write("\t".join(map(str, row)) + "\n")
