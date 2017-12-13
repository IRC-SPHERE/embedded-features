#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <complex.h>

// ----------------------------------------------------------

typedef float complex cplx;

// ----------------------------------------------------------

//#define FREQUENCY_FEATURE_WINDOW_SIZE 128
#define FREQUENCY_FEATURE_WINDOW_SIZE 64

// the fast FFT implementation
#include "../src/fft.c"

// ----------------------------------------------------------

void _fft_ref(cplx buf[], cplx out[], int n, int step)
{
    if (step < n) {
        _fft_ref(out, buf, n, step * 2);
        _fft_ref(out + step, buf + step, n, step * 2);
 
        for (int i = 0; i < n; i += 2 * step) {
            cplx t = cexpf(-I * M_PI * i / n);
            //printf("t = %f + %f i\n", creal(t), cimag(t));
            t *= out[i + step];
            buf[i / 2]     = out[i] + t;
            buf[(i + n)/2] = out[i] - t;
        }
    }
}

// Reference FFT implementation from Rosetta code
void fft_ref(cplx buf[], int n)
{
    cplx out[n];
    for (int i = 0; i < n; i++) out[i] = buf[i];
 
    _fft_ref(buf, out, n, 1);
}

// ----------------------------------------------------------

int main()
{
    int i, j;
    const int NUM_TESTS = 10000;
    bool ok;

    srand(0);

    ok = true;
    for(i = 0; i < NUM_TESTS; ++i) {
        float re[FREQUENCY_FEATURE_WINDOW_SIZE];
        float im[FREQUENCY_FEATURE_WINDOW_SIZE] = {0};
        cplx c[FREQUENCY_FEATURE_WINDOW_SIZE];

        for(j = 0; j < FREQUENCY_FEATURE_WINDOW_SIZE; ++j) {
            re[j] = (float)rand() / RAND_MAX;
            c[j] = re[j];
        }

        fft_ref(c, FREQUENCY_FEATURE_WINDOW_SIZE);
        fft(re, im, FREQUENCY_FEATURE_WINDOW_SIZE);

        for(j = 0; j < FREQUENCY_FEATURE_WINDOW_SIZE; ++j) {
            float x, y, u, v;
            x = creal(c[j]);
            y = cimag(c[j]);
            u = re[j];
            v = im[j];

            // printf("%d: %f %f\n", j, u, v);

            if (fabs(x - u) > 0.00001 || fabs(y - v) > 0.00001) {
                printf("%f %f   %f %f\n", x, u, y, v);
                ok = false;
            }
        }
    }

    if (ok) {
        printf("Success!\n");
        return 0;
    }
    return -1;
}

// ----------------------------------------------------------
