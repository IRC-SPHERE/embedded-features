#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <complex.h>

// ----------------------------------------------------------

// the log2 implementation
#include "../src/log.c"

// ----------------------------------------------------------

int main()
{
    int i, j;
    const int NUM_TESTS = 10000;
    bool ok;

    srand(0);

    ok = true;
    for(i = 0; i < NUM_TESTS; ++i) {
        float f = (float)rand() / RAND_MAX;

        float l1 = log2f(f);
        float l2 = __ieee754_log2(f);

        if (fabs(l1 - l2) > 0.00001) {
            printf("%f %f\n", l1, l2);
            ok = false;
        }
    }

    if (ok) {
        printf("Success!\n");
        return 0;
    }
    return -1;
}

// ----------------------------------------------------------
