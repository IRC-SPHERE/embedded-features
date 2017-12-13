#include "bitreverse.h"

#if FREQUENCY_FEATURE_WINDOW_SIZE == 256
#define FFT_NUM_BITS 8
#elif FREQUENCY_FEATURE_WINDOW_SIZE == 128
#define FFT_NUM_BITS 7
#elif FREQUENCY_FEATURE_WINDOW_SIZE == 64
#define FFT_NUM_BITS 6
#elif FREQUENCY_FEATURE_WINDOW_SIZE == 32
#define FFT_NUM_BITS 5
#else
#error Define FFT_NUM_BITS for your FFT window size!
#endif


#define FFT_TABLE_SIZE 256
#if FREQUENCY_FEATURE_WINDOW_SIZE > FFT_TABLE_SIZE
#error Only FFT up to 256 elements supported!
#endif

#define FFT_TABLE_BITS 8 // equal to log2(FFT_TABLE_SIZE)

//
// Lookup table of sine values from 0 to `pi`.
// Note: using symmetry, could optimize this to 1/2 times the size
//
const float sin_table[FFT_TABLE_SIZE] =
{
    0.000000, 0.012272, 0.024541, 0.036807, 0.049068, 0.061321, 0.073565, 0.085797,
    0.098017, 0.110222, 0.122411, 0.134581, 0.146730, 0.158858, 0.170962, 0.183040,
    0.195090, 0.207111, 0.219101, 0.231058, 0.242980, 0.254866, 0.266713, 0.278520,
    0.290285, 0.302006, 0.313682, 0.325310, 0.336890, 0.348419, 0.359895, 0.371317,
    0.382683, 0.393992, 0.405241, 0.416430, 0.427555, 0.438616, 0.449611, 0.460539,
    0.471397, 0.482184, 0.492898, 0.503538, 0.514103, 0.524590, 0.534998, 0.545325,
    0.555570, 0.565732, 0.575808, 0.585798, 0.595699, 0.605511, 0.615232, 0.624860,
    0.634393, 0.643832, 0.653173, 0.662416, 0.671559, 0.680601, 0.689541, 0.698376,
    0.707107, 0.715731, 0.724247, 0.732654, 0.740951, 0.749136, 0.757209, 0.765167,
    0.773010, 0.780737, 0.788346, 0.795837, 0.803208, 0.810457, 0.817585, 0.824589,
    0.831470, 0.838225, 0.844854, 0.851355, 0.857729, 0.863973, 0.870087, 0.876070,
    0.881921, 0.887640, 0.893224, 0.898674, 0.903989, 0.909168, 0.914210, 0.919114,
    0.923880, 0.928506, 0.932993, 0.937339, 0.941544, 0.945607, 0.949528, 0.953306,
    0.956940, 0.960431, 0.963776, 0.966976, 0.970031, 0.972940, 0.975702, 0.978317,
    0.980785, 0.983105, 0.985278, 0.987301, 0.989177, 0.990903, 0.992480, 0.993907,
    0.995185, 0.996313, 0.997290, 0.998118, 0.998795, 0.999322, 0.999699, 0.999925,
    1.000000, 0.999925, 0.999699, 0.999322, 0.998795, 0.998118, 0.997290, 0.996313,
    0.995185, 0.993907, 0.992480, 0.990903, 0.989177, 0.987301, 0.985278, 0.983105,
    0.980785, 0.978317, 0.975702, 0.972940, 0.970031, 0.966976, 0.963776, 0.960431,
    0.956940, 0.953306, 0.949528, 0.945607, 0.941544, 0.937339, 0.932993, 0.928506,
    0.923880, 0.919114, 0.914210, 0.909168, 0.903989, 0.898674, 0.893224, 0.887640,
    0.881921, 0.876070, 0.870087, 0.863973, 0.857729, 0.851355, 0.844854, 0.838225,
    0.831470, 0.824589, 0.817585, 0.810457, 0.803208, 0.795837, 0.788346, 0.780737,
    0.773010, 0.765167, 0.757209, 0.749136, 0.740951, 0.732654, 0.724247, 0.715731,
    0.707107, 0.698376, 0.689541, 0.680601, 0.671559, 0.662416, 0.653173, 0.643832,
    0.634393, 0.624859, 0.615232, 0.605511, 0.595699, 0.585798, 0.575808, 0.565732,
    0.555570, 0.545325, 0.534998, 0.524590, 0.514103, 0.503538, 0.492898, 0.482184,
    0.471397, 0.460539, 0.449611, 0.438616, 0.427555, 0.416429, 0.405241, 0.393992,
    0.382683, 0.371317, 0.359895, 0.348419, 0.336890, 0.325310, 0.313682, 0.302006,
    0.290285, 0.278520, 0.266713, 0.254866, 0.242980, 0.231058, 0.219101, 0.207111,
    0.195090, 0.183040, 0.170962, 0.158858, 0.146731, 0.134581, 0.122411, 0.110222,
    0.098017, 0.085797, 0.073564, 0.061321, 0.049068, 0.036807, 0.024541, 0.012271,
};

static inline float tsin(int i)
{
    return sin_table[i];
}

static inline float tcos(int i)
{
    if (i >= FFT_TABLE_SIZE / 2) {
        return -sin_table[i - FFT_TABLE_SIZE / 2];
    }
    return sin_table[i + FFT_TABLE_SIZE / 2];
}

//
// Recursive FFT implementation: the recursive subroutine
//
void fft_recursive(float xre[], float xim[], float yre[], float yim[], int step)
{
    int i;
    if (2 * step < FREQUENCY_FEATURE_WINDOW_SIZE) {
        fft_recursive(yre, yim, xre, xim, 2 * step);
        fft_recursive(yre + step, yim + step, xre + step, xim + step, 2 * step);
    }

    for (i = 0; i < FREQUENCY_FEATURE_WINDOW_SIZE; i += 2 * step) {
        float tre, tim;
        float ure, uim;
        float vre, vim;

        tre = tcos(i * (FFT_TABLE_SIZE / FREQUENCY_FEATURE_WINDOW_SIZE));
        tim = -tsin(i * (FFT_TABLE_SIZE / FREQUENCY_FEATURE_WINDOW_SIZE));

        ure = yre[i + step];
        uim = yim[i + step];

        vre = tre * ure - tim * uim;
        vim = tre * uim + tim * ure;

        xre[i / 2] = yre[i] + vre;
        xim[i / 2] = yim[i] + vim;

        xre[(i + FREQUENCY_FEATURE_WINDOW_SIZE) / 2] = yre[i] - vre;
        xim[(i + FREQUENCY_FEATURE_WINDOW_SIZE) / 2] = yim[i] - vim;
    }
}

//
// Recursive FFT implementation.
// Assumes n == FREQUENCY_FEATURE_WINDOW_SIZE
//
void fftr(float xre[], float xim[], int n)
{
    int i;
    float yre[FREQUENCY_FEATURE_WINDOW_SIZE], yim[FREQUENCY_FEATURE_WINDOW_SIZE];

    for (i = 0; i < FREQUENCY_FEATURE_WINDOW_SIZE; i++) {
        yre[i] = xre[i];
        yim[i] = xim[i];
    }

    fft_recursive(xre, xim, yre, yim, 1);
}

//
// Nonrecursive FFT implementation.
// assumes n == FREQUENCY_FEATURE_WINDOW_SIZE
//
void fft(float xre[], float xim[], int n)
{
    int i, shift;

    for (i = 0; i < FREQUENCY_FEATURE_WINDOW_SIZE; i++) {
        // swap the original and reversed values
        uint16_t irev = bitrev(i);

        if(i < irev) {
            float t;

            t = xre[i];
            xre[i] = xre[irev];
            xre[irev] = t;

            t = xim[i];
            xim[i] = xim[irev];
            xim[irev] = t;
        }
    }

    // go through all the matrix sizes from 2 to `N`
    for (shift = 0; (1 << shift) <= FREQUENCY_FEATURE_WINDOW_SIZE / 2; shift++) {
        int step = 1 << shift;
        int table_shift = FFT_TABLE_BITS - shift;
        int offset;

        // go through all the submatrix of size `step*2`
        for (offset = 0; offset < FREQUENCY_FEATURE_WINDOW_SIZE; offset += 2 * step) {

            // go through all the elements of the specific submatrix of size `step*2`
            for (i = 0; i < step; i++) {
                int index = i + offset;
                float tre, tim;
                float ure, uim;
                float wre, wim;

                wre = tcos(i << table_shift);
                wim = -tsin(i << table_shift);

                tre = wre * xre[index + step] - wim * xim[index + step];
                tim = wre * xim[index + step] + wim * xre[index + step];

                ure = xre[index];
                uim = xim[index];

                xre[index] = ure + tre;
                xim[index] = uim + tim;

                xre[index + step] = ure - tre;
                xim[index + step] = uim - tim;
            }
        }
    }
}
