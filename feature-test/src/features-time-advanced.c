// ------------------------------------------

// The following must hold: #bins * divider == 256,
// where 256 is the number of possible accelerometer sample values.
#define NUM_TIME_HISTOGRAM_BINS 16
#define TIME_HISTOGRAM_DIVIDER  16

void feature_histogram_periodic_axis(int axis)
{
    int i, j;
    int bins[NUM_TIME_HISTOGRAM_BINS];
    LOG("axis=%d\n", axis);
    for (i = 0; i <= NSAMPLES - TIME_WINDOW_SIZE; i += PERIODIC_COMPUTATION_WINDOW_SIZE) {
        memset(bins, 0, sizeof(bins));
        for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
            uint8_t ui = (uint8_t)data[i].v[axis] + 128;
            bins[ui / TIME_HISTOGRAM_DIVIDER]++;
        }
        for (j = 0; j < NUM_TIME_HISTOGRAM_BINS; ++j) {
            OUTPUT_I(bins[j], result_i.v[axis]);
        }
        LOG("\n");
    }
}

void feature_histogram_periodic(void)
{
    feature_histogram_periodic_axis(0);
    feature_histogram_periodic_axis(1);
    feature_histogram_periodic_axis(2);
}

// ------------------------------------------

//
// This is precomputed for window size 40 - don't use it with other sizes!
//
// The algorithm used to construct this:
//   int i;
//   const int ws = 40;
//   for(i = 1; i < ws; i++)  {
//     float p = (float)i / ws;
//     printf("%f,\n", -p * log2(p));
//   }
//
#if TIME_WINDOW_SIZE == 40
const float entropy_lookup_table[TIME_WINDOW_SIZE] = {
    0.000000,
    0.133048,
    0.216096,
    0.280272,
    0.332193,
    0.375000,
    0.410545,
    0.440050,
    0.464386,
    0.484201,
    0.500000,
    0.512187,
    0.521090,
    0.526984,
    0.530101,
    0.530639,
    0.528771,
    0.524648,
    0.518401,
    0.510150,
    0.500000,
    0.488046,
    0.474373,
    0.459061,
    0.442179,
    0.423795,
    0.403967,
    0.382752,
    0.360201,
    0.336362,
    0.311278,
    0.284992,
    0.257542,
    0.228966,
    0.199295,
    0.168564,
    0.136803,
    0.104039,
    0.070301,
    0.035613,
};
#elif TIME_WINDOW_SIZE == 32
const float entropy_lookup_table[TIME_WINDOW_SIZE] = {
    0.000000,
    0.156250,
    0.250000,
    0.320160,
    0.375000,
    0.418449,
    0.452820,
    0.479641,
    0.500000,
    0.514709,
    0.524397,
    0.529570,
    0.530639,
    0.527946,
    0.521782,
    0.512395,
    0.500000,
    0.484785,
    0.466917,
    0.446543,
    0.423795,
    0.398792,
    0.371641,
    0.342440,
    0.311278,
    0.278237,
    0.243393,
    0.206814,
    0.168564,
    0.128705,
    0.087290,
    0.044372,
};
#elif TIME_WINDOW_SIZE == 64
const float entropy_lookup_table[TIME_WINDOW_SIZE] = {
    0.000000,
    0.093750,
    0.156250,
    0.206955,
    0.250000,
    0.287349,
    0.320160,
    0.349196,
    0.375000,
    0.397979,
    0.418449,
    0.436660,
    0.452820,
    0.467098,
    0.479641,
    0.490573,
    0.500000,
    0.508018,
    0.514709,
    0.520147,
    0.524397,
    0.527521,
    0.529570,
    0.530595,
    0.530639,
    0.529744,
    0.527946,
    0.525282,
    0.521782,
    0.517477,
    0.512395,
    0.506561,
    0.500000,
    0.492734,
    0.484785,
    0.476173,
    0.466917,
    0.457035,
    0.446543,
    0.435458,
    0.423795,
    0.411568,
    0.398792,
    0.385478,
    0.371641,
    0.357291,
    0.342440,
    0.327099,
    0.311278,
    0.294988,
    0.278237,
    0.261036,
    0.243393,
    0.225316,
    0.206814,
    0.187894,
    0.168564,
    0.148832,
    0.128705,
    0.108188,
    0.087290,
    0.066016,
    0.044372,
    0.022365,
};
#elif TIME_WINDOW_SIZE == 128
const float entropy_lookup_table[TIME_WINDOW_SIZE] = {
    0.000000,
    0.054688,
    0.093750,
    0.126915,
    0.156250,
    0.182737,
    0.206955,
    0.229285,
    0.250000,
    0.269302,
    0.287349,
    0.304268,
    0.320160,
    0.335112,
    0.349196,
    0.362474,
    0.375000,
    0.386821,
    0.397979,
    0.408511,
    0.418449,
    0.427823,
    0.436660,
    0.444985,
    0.452820,
    0.460184,
    0.467098,
    0.473578,
    0.479641,
    0.485301,
    0.490573,
    0.495468,
    0.500000,
    0.504180,
    0.508018,
    0.511524,
    0.514709,
    0.517580,
    0.520147,
    0.522417,
    0.524397,
    0.526097,
    0.527521,
    0.528677,
    0.529570,
    0.530208,
    0.530595,
    0.530737,
    0.530639,
    0.530306,
    0.529744,
    0.528956,
    0.527946,
    0.526720,
    0.525282,
    0.523634,
    0.521782,
    0.519729,
    0.517477,
    0.515032,
    0.512395,
    0.509570,
    0.506561,
    0.503370,
    0.500000,
    0.496454,
    0.492734,
    0.488844,
    0.484785,
    0.480561,
    0.476173,
    0.471625,
    0.466917,
    0.462053,
    0.457035,
    0.451864,
    0.446543,
    0.441074,
    0.435458,
    0.429698,
    0.423795,
    0.417751,
    0.411568,
    0.405248,
    0.398792,
    0.392201,
    0.385478,
    0.378624,
    0.371641,
    0.364529,
    0.357291,
    0.349927,
    0.342440,
    0.334830,
    0.327099,
    0.319248,
    0.311278,
    0.303191,
    0.294988,
    0.286669,
    0.278237,
    0.269693,
    0.261036,
    0.252269,
    0.243393,
    0.234408,
    0.225316,
    0.216117,
    0.206814,
    0.197406,
    0.187894,
    0.178280,
    0.168564,
    0.158748,
    0.148832,
    0.138818,
    0.128705,
    0.118495,
    0.108188,
    0.097787,
    0.087290,
    0.076700,
    0.066016,
    0.055240,
    0.044372,
    0.033414,
    0.022365,
    0.011227,
};
#endif


static inline float calc_entropy(unsigned c)
{
    return entropy_lookup_table[c];
}

void feature_entropy_f_axis(int axis)
{
    int i;
    // this algo (ab)uses the fact that only 256 values are possible
    uint8_t stats[256] = {0};
    float entropy = 0.0;
    LOG("axis=%d\n", axis);
    for (i = 0; i < NSAMPLES; ++i) {
        int v1, v2;

        if (i < TIME_WINDOW_SIZE) {
            // lets just have 0 as the entropy of the first K elements for simplicity
            v1 = data[i].v[axis] + 128;
            stats[v1]++;
            LOG("0\n"); // just add padding zeroes at the start
            continue;
        }

        // remove a value
        v1 = data[i - TIME_WINDOW_SIZE].v[axis] + 128;
        v2 = data[i].v[axis] + 128;
        if (v1 == v2) {
            continue;
        }

        entropy -= calc_entropy(stats[v1]);
        stats[v1]--;
        if (stats[v1]) {
            entropy += calc_entropy(stats[v1]);
        }
    
        if (stats[v2]) {
            entropy -= calc_entropy(stats[v2]);
        }
        stats[v2]++;
        entropy += calc_entropy(stats[v2]);

        OUTPUT_F(entropy, result_f.v[axis]);
        LOG("\n");
    }
}

void feature_entropy_f(void)
{
    feature_entropy_f_axis(0);
    feature_entropy_f_axis(1);
    feature_entropy_f_axis(2);
}

// ------------------------------------------

void feature_entropy_periodic_f_axis(int axis)
{
    int i, j;
    LOG("axis=%d\n", axis);
    for (i = 0; i <= NSAMPLES - TIME_WINDOW_SIZE; i += PERIODIC_COMPUTATION_WINDOW_SIZE) {
        float entropy = 0.0;
        uint8_t stats[256] = {0};
        for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
            int v = data[i + j].v[axis] + 128;
            stats[v]++;
        }

        for (j = 0; j < 256; ++j) {
            entropy += calc_entropy(stats[j]);
        }
        OUTPUT_F(entropy, result_f.v[axis]);
        LOG("\n");
    }
}

void feature_entropy_periodic_f(void)
{
    feature_entropy_periodic_f_axis(0);
    feature_entropy_periodic_f_axis(1);
    feature_entropy_periodic_f_axis(2);
}

// ------------------------------------------
