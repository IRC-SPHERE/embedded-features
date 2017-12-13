// ------------------------------------------

// Integer FFT
#include "intfft.c"

// Floating point FFT
#include "fft.c"

// Logarithm function
#include "log.c"

//
// The FFT is not post-processed (reordered or normalized).
// Therefore the structure of the FFT result is this:
//
//    [DC] [F1] [F2] ... [FNQ-1] [FNQ] [-FNQ-1] ... [-F2] [-F1]
//      0    1   2   ...   N/2-1  N/2    N/2+1  ...  N-2   N-1
//
// where DC is the direct current, and FNQ is the Nyquist frequency.
// For real-valued signals [-F_i] = -[F_i],
// i.e. |[-F_i]| = |[F_i]|, so just half of the results is effectively useful.
//

// ------------------------------------------

// The following must hold: (#bins - 1) * (2 * divider) == FREQUENCY_FEATURE_WINDOW_SIZE
#define NUM_FREQUENCY_HISTOGRAM_BINS 9
#define FREQUENCY_HISTOGRAM_DIVIDER (FREQUENCY_FEATURE_WINDOW_SIZE / 16)

// ------------------------------------------

typedef void spectral_feature_function_f_t(float re[], float im[], int axis);
typedef void spectral_feature_function_i_t(int16_t re[], int16_t im[], int axis);

#define tlog2f(x) __ieee754_log2(x)

#if CONTIKI_TARGET_NRF52DK
// The `fabs` and `fabsf` functions are not defined for this target
float fabsf(float x)
{
    return x > 0 ? x : -x;
}
#endif

// ------------------------------------------

void spectral_feature_maxima_f(float re[], float im[], int axis)
{
    int j;
    // search for the maximal nonzero frequency, starting from the highest (offset N/2 + 1)
    // TODO: what to use as the epsilon here?
    float epsilon = 0.1;
    for (j = FREQUENCY_FEATURE_WINDOW_SIZE / 2 + 1; j >= 0; --j) {
        float msq = re[j] * re[j] + im[j] * im[j];
        if (fabsf(msq) > epsilon) {
            OUTPUT_F(msq, result_f.v[axis]);
            LOG("\n");
            break;
        }
    }
}

void spectral_feature_maxima_i(int16_t re[], int16_t im[], int axis)
{
    int j;
    // search for the maximal nonzero frequency, starting from the highest (offset N/2 + 1)
    for (j = FREQUENCY_FEATURE_WINDOW_SIZE / 2 + 1; j >= 0; --j) {
        uint32_t msq = (uint32_t)re[j] * re[j] + im[j] * im[j];
        if (msq != 0) {
            OUTPUT_I(msq, result_i.v[axis]);
            LOG("\n");
            break;
        }
    }
}

void spectral_feature_density_f(float re[], float im[], int axis)
{
    int j;
    for (j = 0; j <= FREQUENCY_FEATURE_WINDOW_SIZE / 2; ++j) {
        float msq = re[j] * re[j] + im[j] * im[j];
        OUTPUT_F(msq, result_f.v[axis]);
        LOG("\n");
    }
}

void spectral_feature_density_i(int16_t re[], int16_t im[], int axis)
{
    int j;
    for (j = 0; j <= FREQUENCY_FEATURE_WINDOW_SIZE / 2; ++j) {
        uint32_t msq = (uint32_t)re[j] * re[j] + im[j] * im[j];
        OUTPUT_I(msq, result_i.v[axis]);
        LOG("\n");
    }
}

// This discards the DC component (frequency == 0) and the negative frequencies.
// XXX: what if the DC component is the dominating one?
void spectral_feature_centroid_f(float re[], float im[], int axis)
{
    int j;
    float numerator = 0, denominator = 0;
    float normalization_coefficient = 1.0 / (FREQUENCY_FEATURE_WINDOW_SIZE * FREQUENCY_FEATURE_WINDOW_SIZE);
    for (j = 1; j <= FREQUENCY_FEATURE_WINDOW_SIZE / 2; ++j) {
        // `m` is the magnitude
        float m = tsqrtf(normalization_coefficient * (re[j] * re[j] + im[j] * im[j]));
        numerator += j * m;
        denominator += m;
    }
    OUTPUT_F(numerator / denominator, result_f.v[axis]);
    LOG("\n");
}

// XXX: not sure this is the correct definition of entropy of a complex signal
void spectral_feature_entropy_f(float re[], float im[], int axis)
{
    int j;
    float entropy = 0;
    float squared_sum = 0;
    float msq[FREQUENCY_FEATURE_WINDOW_SIZE];
    float normalization_coefficient = 1.0 / (FREQUENCY_FEATURE_WINDOW_SIZE * FREQUENCY_FEATURE_WINDOW_SIZE);
    for (j = 0; j < FREQUENCY_FEATURE_WINDOW_SIZE; ++j) {
        msq[j] = normalization_coefficient * (re[j] * re[j] + im[j] * im[j]); // calculate the squared module |x|^2
        squared_sum += msq[j];
    }
    for (j = 0; j < FREQUENCY_FEATURE_WINDOW_SIZE; ++j) {
        float q = msq[j] / squared_sum;
        if (q) {
            entropy += q * tlog2f(q);
        }
    }
    entropy = -entropy;
    OUTPUT_F(entropy, result_f.v[axis]);
    LOG("\n");
}

void spectral_feature_histogram_i(int16_t re[], int16_t im[], int axis)
{
    int i;
    uint32_t msq;
    uint32_t bins[NUM_FREQUENCY_HISTOGRAM_BINS] = {0};

    msq = (uint32_t)re[0] * re[0] + im[0] * im[0];
    bins[0] = msq;

    for (i = 1; i < FREQUENCY_FEATURE_WINDOW_SIZE / 2 + 1; ++i) {
        uint8_t ui = (i - 1) / FREQUENCY_HISTOGRAM_DIVIDER + 1;
        msq = (uint32_t)re[i] * re[i] + im[i] * im[i];
        bins[ui] += msq;
    }

    for (i = 0; i < NUM_FREQUENCY_HISTOGRAM_BINS; ++i) {
        OUTPUT_I(bins[i], result_i.v[axis]);
    }
    LOG("\n");
}

void spectral_feature_histogram_f(float re[], float im[], int axis)
{
    int i;
    float msq;
    float bins[NUM_FREQUENCY_HISTOGRAM_BINS] = {0};

    msq = re[0] * re[0] + im[0] * im[0];
    bins[0] = msq;

    for (i = 1; i < FREQUENCY_FEATURE_WINDOW_SIZE / 2 + 1; ++i) {
        uint8_t ui = (i - 1) / FREQUENCY_HISTOGRAM_DIVIDER + 1;
        msq = re[i] * re[i] + im[i] * im[i];
        bins[ui] += msq;
    }

    for (i = 0; i < NUM_FREQUENCY_HISTOGRAM_BINS; ++i) {
        OUTPUT_F(bins[i], result_f.v[axis]);
    }
    LOG("\n");
}

// ------------------------------------------

void feature_spectral_f(spectral_feature_function_f_t f, int axis)
{
    int i, j;
    float re[FREQUENCY_FEATURE_WINDOW_SIZE];
    float im[FREQUENCY_FEATURE_WINDOW_SIZE];

    LOG("axis=%d\n", axis);

    for (i = 0; i <= NSAMPLES - FREQUENCY_FEATURE_WINDOW_SIZE;
         i += PERIODIC_COMPUTATION_WINDOW_SIZE) {

        for (j = 0; j < FREQUENCY_FEATURE_WINDOW_SIZE; ++j) {
            re[j] = data[i + j].v[axis];
        }
        memset(im, 0, sizeof(im));

        // own FFT implementation
        fft(re, im, FREQUENCY_FEATURE_WINDOW_SIZE);
        f(re, im, axis);       
    }
}

void feature_spectral_i(spectral_feature_function_i_t f, int axis)
{
    int i, j;
    int16_t re[FREQUENCY_FEATURE_WINDOW_SIZE];
    int16_t im[FREQUENCY_FEATURE_WINDOW_SIZE];

    LOG("axis=%d\n", axis);

    for (i = 0; i <= NSAMPLES - FREQUENCY_FEATURE_WINDOW_SIZE;
         i += PERIODIC_COMPUTATION_WINDOW_SIZE) {

        for (j = 0; j < FREQUENCY_FEATURE_WINDOW_SIZE; ++j) {
            re[j] = data[i + j].v[axis];
        }
        memset(im, 0, sizeof(im));

        intfft(re, im, FREQUENCY_FEATURE_WINDOW_SIZE);

        f(re, im, axis);
    } 
}

// ------------------------------------------

void feature_spectral_maxima_i(void)
{
    feature_spectral_i(spectral_feature_maxima_i, 0);
    feature_spectral_i(spectral_feature_maxima_i, 1);
    feature_spectral_i(spectral_feature_maxima_i, 2);
}

// ------------------------------------------

void feature_spectral_maxima_f(void)
{
    feature_spectral_f(spectral_feature_maxima_f, 0);
    feature_spectral_f(spectral_feature_maxima_f, 1);
    feature_spectral_f(spectral_feature_maxima_f, 2);
}

// ------------------------------------------

void feature_spectral_density_i(void)
{
    feature_spectral_i(spectral_feature_density_i, 0);
    feature_spectral_i(spectral_feature_density_i, 1);
    feature_spectral_i(spectral_feature_density_i, 2);
}

// ------------------------------------------

void feature_spectral_density_f(void)
{
    feature_spectral_f(spectral_feature_density_f, 0);
    feature_spectral_f(spectral_feature_density_f, 1);
    feature_spectral_f(spectral_feature_density_f, 2);
}

// ------------------------------------------

void feature_spectral_centroid_f(void)
{
    feature_spectral_f(spectral_feature_centroid_f, 0);
    feature_spectral_f(spectral_feature_centroid_f, 1);
    feature_spectral_f(spectral_feature_centroid_f, 2);
}

// ------------------------------------------

void feature_spectral_flux_f_axis(int axis)
{
    int i, j;
    float re[FREQUENCY_FEATURE_WINDOW_SIZE];
    float im[FREQUENCY_FEATURE_WINDOW_SIZE];

    float re_old[FREQUENCY_FEATURE_WINDOW_SIZE];
    float im_old[FREQUENCY_FEATURE_WINDOW_SIZE];

    LOG("axis=%d\n", axis);

    for (i = 0; i <= NSAMPLES - FREQUENCY_FEATURE_WINDOW_SIZE;
         i += PERIODIC_COMPUTATION_WINDOW_SIZE) {

        for (j = 0; j < FREQUENCY_FEATURE_WINDOW_SIZE; ++j) {
            re[j] = data[i + j].v[axis];
        }
        memset(im, 0, sizeof(im));

        fft(re, im, FREQUENCY_FEATURE_WINDOW_SIZE);

        if(i > 0) {
            float sum = 0;
            for (j = 0; j < FREQUENCY_FEATURE_WINDOW_SIZE; ++j) {
                float x = re[j] - re_old[j];
                float y = im[j] - im_old[j];
                float m = x * x + y * y;
                sum += m;
            }
            OUTPUT_F(tsqrtf(sum), result_f.v[axis]);
            LOG("\n");
        }

        memcpy(im_old, im, sizeof(im));
        memcpy(re_old, re, sizeof(re));
    } 
}

void feature_spectral_flux_f(void)
{
    feature_spectral_flux_f_axis(0);
    feature_spectral_flux_f_axis(1);
    feature_spectral_flux_f_axis(2);
}

// ------------------------------------------

void feature_spectral_entropy_f(void)
{
    feature_spectral_f(spectral_feature_entropy_f, 0);
    feature_spectral_f(spectral_feature_entropy_f, 1);
    feature_spectral_f(spectral_feature_entropy_f, 2);
}

// ------------------------------------------

void feature_spectral_ma_f(void)
{
    int i, j;
    float re[FREQUENCY_FEATURE_WINDOW_SIZE][3];
    float im[FREQUENCY_FEATURE_WINDOW_SIZE][3];

    for (i = 0; i <= NSAMPLES - FREQUENCY_FEATURE_WINDOW_SIZE;
         i += PERIODIC_COMPUTATION_WINDOW_SIZE) {

        for (j = 0; j < FREQUENCY_FEATURE_WINDOW_SIZE; ++j) {
            re[j][0] = data[i + j].v[0];
            re[j][1] = data[i + j].v[1];
            re[j][2] = data[i + j].v[2];
        }
        memset(im, 0, sizeof(im));

        fft(re[0], im[0], FREQUENCY_FEATURE_WINDOW_SIZE);
        fft(re[1], im[1], FREQUENCY_FEATURE_WINDOW_SIZE);
        fft(re[2], im[2], FREQUENCY_FEATURE_WINDOW_SIZE);

        float sum = 0;
        for (j = 0; j <= FREQUENCY_FEATURE_WINDOW_SIZE / 2; ++j) {
            float amsq = re[j][0] * re[j][0] + im[j][0] * im[j][0];
            float bmsq = re[j][1] * re[j][1] + im[j][1] * im[j][1];
            float cmsq = re[j][2] * re[j][2] + im[j][2] * im[j][2];
            float msq = tsqrtf(amsq + bmsq + cmsq);
            if(j != 0 && j != FREQUENCY_FEATURE_WINDOW_SIZE / 2) {
                // account both for the negative and positive frequency
                sum += 2 * msq;
            } else {
                sum += msq;
            }
        }
        OUTPUT_F(sum, result_f.v[0]);
        LOG("\n");
    } 
}

// ------------------------------------------

void feature_spectral_ma_squared_i(void)
{
    int i, j;
    int16_t re[FREQUENCY_FEATURE_WINDOW_SIZE][3];
    int16_t im[FREQUENCY_FEATURE_WINDOW_SIZE][3];

    for (i = 0; i <= NSAMPLES - FREQUENCY_FEATURE_WINDOW_SIZE;
         i += PERIODIC_COMPUTATION_WINDOW_SIZE) {

        for (j = 0; j < FREQUENCY_FEATURE_WINDOW_SIZE; ++j) {
            re[j][0] = data[i + j].v[0];
            re[j][1] = data[i + j].v[1];
            re[j][2] = data[i + j].v[2];
        }
        memset(im, 0, sizeof(im));

        intfft(re[0], im[0], FREQUENCY_FEATURE_WINDOW_SIZE);
        intfft(re[1], im[1], FREQUENCY_FEATURE_WINDOW_SIZE);
        intfft(re[2], im[2], FREQUENCY_FEATURE_WINDOW_SIZE);

        uint64_t sum = 0;
        for (j = 0; j <= FREQUENCY_FEATURE_WINDOW_SIZE / 2; ++j) {
            // XXX: this could also be done using 32-bit arithmetic I guess?
            uint64_t amsq = (uint64_t)re[j][0] * re[j][0] + im[j][0] * im[j][0];
            uint64_t bmsq = (uint64_t)re[j][1] * re[j][1] + im[j][1] * im[j][1];
            uint64_t cmsq = (uint64_t)re[j][2] * re[j][2] + im[j][2] * im[j][2];
            uint64_t msq = amsq + bmsq + cmsq;
            if(j != 0 && j != FREQUENCY_FEATURE_WINDOW_SIZE / 2) {
                // account both for the negative and positive frequency
                sum += 2 * msq;
            } else {
                sum += msq;
            }
        }
        OUTPUT_IL((long long int)sum, result_i.v[0]);
        LOG("\n");
    } 
}

// ------------------------------------------

void feature_spectral_histogram_i(void)
{
    feature_spectral_i(spectral_feature_histogram_i, 0);
    feature_spectral_i(spectral_feature_histogram_i, 1);
    feature_spectral_i(spectral_feature_histogram_i, 2);
}

// ------------------------------------------

void feature_spectral_histogram_f(void)
{
    feature_spectral_f(spectral_feature_histogram_f, 0);
    feature_spectral_f(spectral_feature_histogram_f, 1);
    feature_spectral_f(spectral_feature_histogram_f, 2);
}

// ------------------------------------------
