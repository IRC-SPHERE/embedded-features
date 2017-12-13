#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define NSAMPLES 100000
//#define NSAMPLES 5555

#define SAMPLING_HZ 25

// define in terms of number of packets; 1 or 2 seconds
#define WINDOW_SIZE (2 * SAMPLING_HZ)

// -----------------------------------------------------------

#define NUM_AXIS 3

typedef struct {
    int8_t v[NUM_AXIS];
} accel_t;

typedef struct {
    int32_t v[NUM_AXIS];
} result_i_t;

typedef struct {
    float v[NUM_AXIS];
} result_f_t;

// the input data
accel_t data[NSAMPLES];

#define DO_LOG_OUTPUT 0

#if DO_LOG_OUTPUT
#define LOG(...) printk(__VA_ARGS__)
#else
#define LOG(...)
#endif

#if DO_LOG_OUTPUT
#define OUTPUT(x, variable, format) printk(format, x)
#else
#define OUTPUT(x, variable, format) variable = x
#endif

#define OUTPUT_I(x, variable) OUTPUT(x, variable, "%d")
#define OUTPUT_F(x, variable) OUTPUT(x, variable, "%f")

#define FREQUENCY_FEATURE_WINDOW_SIZE 128

// the result of the accel calculations is stored here
volatile result_i_t result_i;
volatile result_i_t result_ia[FREQUENCY_FEATURE_WINDOW_SIZE];
volatile result_f_t result_f;
volatile result_f_t result_fa[FREQUENCY_FEATURE_WINDOW_SIZE];

// -----------------------------------------------------------

// the fast median and quintile implementation
#include "../src/median.c"

// -----------------------------------------------------------

static int cmp_readings(const void *v1, const void *v2)
{
    const int8_t *x1 = v1;
    const int8_t *x2 = v2;
    return (int)*x1 - (int)*x2;
}

// -----------------------------------------------------------

void feature_median_axis_slow(int axis, result_i_t *result1, result_i_t *result2)
{
    int i;
    for(i = 0; i < NSAMPLES; ++i) {
        int8_t buffer[WINDOW_SIZE];
        int j, k;
        if (i > WINDOW_SIZE - 1) {
            j = i - (WINDOW_SIZE - 1);
        } else {
            j = 0;
        }
        for(k = 0; j <= i; ++j, ++k) {
            buffer[k] = data[j].v[axis];
        }
        qsort(buffer, k, 1, cmp_readings);
        // median is in the middle
        if (k & 1) {
            // well-defined
            result1[i].v[axis] = buffer[k / 2];
            result2[i].v[axis] = buffer[k / 2];
        } else {
            // one of two
            result1[i].v[axis] = buffer[(k - 1) / 2];
            result2[i].v[axis] = buffer[k / 2];
        }

        /* if (axis == 0 && i > 350) { */
        /* printf("%d (%d): ", i, k); */
        /* for(j = 0; j < k; ++j) { */
        /*     printf("%d ", buffer[j]); */
        /* } */
        /* printf("\n"); */
        /* printf("  r=%d %d\n", (int)result1[i].v[axis], (int)result2[i].v[axis]); */
        /* } */
    }
}

// -----------------------------------------------------------

void feature_quantile_axis_slow(int axis, int low_multiplier, int high_multiplier, result_i_t *result1, result_i_t *result2)
{
    int i;
    int total_multiplier = low_multiplier + high_multiplier;
    for(i = 0; i < NSAMPLES; ++i) {
        int8_t buffer[WINDOW_SIZE];
        int j, k, pos;
        if (i > WINDOW_SIZE - 1) {
            j = i - (WINDOW_SIZE - 1);
        } else {
            j = 0;
        }
        for(k = 0; j <= i; ++j, ++k) {
            buffer[k] = data[j].v[axis];
        }
        qsort(buffer, k, 1, cmp_readings);

        // median is in the middle
        /* if (k & 1) { */
        /*     // well-defined */
        /*     result1[i].v[axis] = buffer[k / 2]; */
        /*     result2[i].v[axis] = buffer[k / 2]; */
        /* } else { */
        /*     // one of two */
        /*     result1[i].v[axis] = buffer[(k - 1) / 2]; */
        /*     result2[i].v[axis] = buffer[k / 2]; */
        /* } */
        pos = (low_multiplier * (k - 1)) / total_multiplier;
        result1[i].v[axis] = buffer[pos];
        pos = (low_multiplier * (k - 1) + total_multiplier - 1) / total_multiplier;
        result2[i].v[axis] = buffer[pos];

	/* printf("%d (%d): ", i, k); */
        /* for(j = 0; j < k; ++j) { */
        /*     printf("%d ", buffer[j]); */
        /* } */
        /* printf("\n"); */
        /* printf("  r=%d %d\n", (int)result1[i].v[axis], (int)result2[i].v[axis]); */
    }
}

// -----------------------------------------------------------

int main()
{
    int i, axis;
    result_i_t result_fast[NSAMPLES], result_slow1[NSAMPLES], result_slow2[NSAMPLES];
    bool ok;

    srand(0);

    for(i = 0; i < NSAMPLES; ++i) {
        for(axis = 0; axis < 3; ++axis) {
            data[i].v[axis] = rand();
        }
    }

    printf("x\n");
    feature_median_axis_test(0, result_fast);
    printf("x slow\n");
    feature_median_axis_slow(0, result_slow1, result_slow2);
    printf("y\n");
    feature_median_axis_test(1, result_fast);
    printf("y slow\n");
    feature_median_axis_slow(1, result_slow1, result_slow2);
    printf("z\n");
    feature_median_axis_test(2, result_fast);
    printf("z slow\n");
    feature_median_axis_slow(2, result_slow1, result_slow2);

    for(i = 0; i < NSAMPLES; ++i) {
        for(axis = 0; axis < NUM_AXIS; ++axis) {
            if(result_slow1[i].v[axis] != result_fast[i].v[axis]
                    && result_slow2[i].v[axis] != result_fast[i].v[axis]) {
                printf("%d, %d: %d/%d vs %d\n", i, axis,
                        (int)result_slow1[i].v[axis],
                        (int)result_slow2[i].v[axis],
                        (int)result_fast[i].v[axis]);
            }
        }
    }

    printf("x: 25%%\n");
    feature_quantile_axis_test(0, 1, 3, result_fast);
    printf("x slow\n");
    feature_quantile_axis_slow(0, 1, 3, result_slow1, result_slow2);
    printf("y: 50%%\n");
    feature_quantile_axis_test(1, 1, 1, result_fast);
    printf("y slow\n");
    feature_quantile_axis_slow(1, 1, 1, result_slow1, result_slow2);
    printf("z: 75%%\n");
    feature_quantile_axis_test(2, 3, 1, result_fast);
    printf("z slow\n");
    feature_quantile_axis_slow(2, 3, 1, result_slow1, result_slow2);

    ok = true;
    for(i = 0; i < NSAMPLES; ++i) {
        for(axis = 0; axis < NUM_AXIS; ++axis) {
            if(result_slow1[i].v[axis] != result_fast[i].v[axis]
                    && result_slow2[i].v[axis] != result_fast[i].v[axis]) {
                printf("%d, %d: %d/%d vs %d\n", i, axis,
                        (int)result_slow1[i].v[axis],
                        (int)result_slow2[i].v[axis],
                        (int)result_fast[i].v[axis]);
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
