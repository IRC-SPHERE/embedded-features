#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#ifdef CONFIG_ARCH
#include <zephyr.h>
#include <misc/printk.h>
#include "qsort.h"
#else
#include "adaptation.h"
#endif

#include "sqrt.h"

#include "main.h"

// -----------------------------------------------------------

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

#define OUTPUT_I(x, variable)  OUTPUT(x, variable, "%d ")
#define OUTPUT_IL(x, variable) OUTPUT(x, variable, "%lld ")
#define OUTPUT_F(x, variable)  OUTPUT(x, variable, "%f ")

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

// -----------------------------------------------------------

// the input data
#if CONTIKI_TARGET_Z1
// take only 7500 samples: 10000 or more are not supported by the compiler
const accel_t data[7500] =
#else
// take as many samples as provided
const accel_t data[] =
#endif
{
#ifdef FEATURE_TEST_INPUT
# include FEATURE_TEST_INPUT
#else
# include "../../data/00001-1.c"
#endif
};

// the total number of samples
#define NSAMPLES (sizeof(data) / sizeof(*data))

// -----------------------------------------------------------

typedef void (*feature_function)(void);

typedef struct {
    const char *name;
    feature_function f;
    int class;
} test_t;

// the result of the accel calculations is stored here
volatile result_i_t result_i;
volatile result_f_t result_f;

// -----------------------------------------------------------

#include "features-time-basic.c"
#include "features-time-median.c"
#include "features-time-advanced.c"

#ifndef CONTIKI_TARGET_Z1
// The msp430 compiler is not able to compile the FFT code
#include "features-frequency.c"
#endif

// -----------------------------------------------------------
void test(const test_t *t)
{
    s64_t start = k_uptime_get();
    int i;
    LOG("Start feature: %s\n", t->name);
    for (i = 0; i < NUM_REPETITIONS[t->class]; ++i) {
        t->f();
    }
    // print time needed
    s64_t delta = k_uptime_delta(&start);
#if 1
    printk("Feature: %s Time: %lu usec per %u samples\n",
            t->name,
            (long unsigned)(delta * 1000.0 / NUM_REPETITIONS[t->class]),
            (unsigned)NSAMPLES);
#else
    printk("Feature: %s Time: %lld ms (%d times)\n",
            t->name, delta, NUM_REPETITIONS[t->class]);
#endif
}

// -----------------------------------------------------------

void feature_memory_access(void)
{
    void *v;
    v = &v;
    uint8_t *x = v;
    uint8_t *end = x - 1000;
    volatile int c = 0;
    while (x >= end) {
        if(*x == 0xc1) {
            c++;
        }
        x--;
    }
//    printf("c=%d\n", c);
}

// -----------------------------------------------------------

const test_t tests[] =
{
//    { "memory access     ", feature_memory_access, VERY_FAST},

    { "mean (f)          ", feature_mean_f, MODERATE },
    { "mean (f, p)       ", feature_mean_periodic_f, MODERATE },
    { "magnitude (f)     ", feature_magnitude_f, MODERATE },
        
#if 0
    // Time domain features
    { "mean (f)          ", feature_mean_f, MODERATE },
    { "mean (f, p)       ", feature_mean_periodic_f, MODERATE },
    { "mean (i)          ", feature_mean_i, VERY_FAST },
    { "mean (i, p)       ", feature_mean_periodic_i, VERY_FAST },
    { "quartile_25 (i)   ", feature_quartile_25, FAST },
    { "quartile_25 (i, p)", feature_quartile_25_periodic, FAST },
    { "median (i)        ", feature_median, FAST },
    { "median (i, p)     ", feature_median_periodic, FAST },
    { "quartile_75 (i)   ", feature_quartile_75, FAST },
    { "quartile_75 (i, p)", feature_quartile_75_periodic, FAST },
    { "quartiles (i, p)  ", feature_quartiles_periodic, FAST },
    { "min (i)           ", feature_min, VERY_FAST },
    { "min (i, p)        ", feature_min_periodic, VERY_FAST },
    { "max (i)           ", feature_max, VERY_FAST },
    { "max (i, p)        ", feature_max_periodic, VERY_FAST },
    { "variance (i)      ", feature_variance, FAST },
    { "variance (i, p)   ", feature_variance_periodic, FAST },
    { "std (f)           ", feature_std, MODERATE },
    { "std (f, p)        ", feature_std_periodic, MODERATE },
    { "magnitude^2 (i)   ", feature_magnitude_sq_i, VERY_FAST },
    { "magnitude (f)     ", feature_magnitude_f, MODERATE },
    { "0-crossings (i)   ", feature_zero_crossings, FAST },
    { "0-crossings (i, p)", feature_zero_crossings_periodic, FAST },
    { "entropy (f)       ", feature_entropy_f, MODERATE },
    { "entropy (f, p)    ", feature_entropy_periodic_f, MODERATE },
    { "histogram (i, p)  ", feature_histogram_periodic, FAST },
#endif

#if 0
#ifndef CONTIKI_TARGET_Z1
    // Frequency domain features
    // All these features are periodic (calculated once per second)
    { "spectral density (i)     ", feature_spectral_density_i, MODERATE },
    { "spectral density (f)     ", feature_spectral_density_f, MODERATE },
    { "spectral maxima (i)      ", feature_spectral_maxima_i, MODERATE },
    { "spectral maxima (f)      ", feature_spectral_maxima_f, MODERATE },
    { "spectral centroid (f)    ", feature_spectral_centroid_f, MODERATE },
    { "spectral flux (f)        ", feature_spectral_flux_f, MODERATE },
    { "spectral entropy (f)     ", feature_spectral_entropy_f, MODERATE },
    { "spectral magn. area (f)  ", feature_spectral_ma_f, MODERATE },
    { "spectral magn. area^2 (i)", feature_spectral_ma_squared_i, MODERATE },
    { "spectral histogram (i)   ", feature_spectral_histogram_i, MODERATE },
    { "spectral histogram (f)   ", feature_spectral_histogram_f, MODERATE },
#endif // CONTIKI_TARGET_Z1
#endif
};

// -----------------------------------------------------------

void do_tests(void)
{
    int i;

    printk("Starting tests, ARCH=%s F_CPU=%d MHz\n",
           CONFIG_ARCH, (int)(CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC / 1000000));
    for (i = 0; i < sizeof(tests) / sizeof(*tests); ++i) {
        test(&tests[i]);
    }
    printk("Done!\n");
}

// -----------------------------------------------------------

#if CONTIKI

#include "dev/watchdog.h"

PROCESS(test_process, "Test process");
AUTOSTART_PROCESSES(&test_process);

PROCESS_THREAD(test_process, ev, data)
{
  PROCESS_BEGIN();

  printk("start\n");

#if CONTIKI_TARGET_NRF52DK
  // Enable the FPU bits in the Coprocessor Access Control Register
  SCB->CPACR |= (3UL << 20) | (3UL << 22);
  // Data Synchronization Barrier
  __DSB();
  // Instruction Synchronization Barrier
  __ISB();
#else // CONTIKI_TARGET_NRF52DK

  // Stop the watchdog (breaks the system on NRF52DK).
  // Note: this function is used as not all platforms define watchdog_stop()!
  watchdog_init();

#endif
 
  do_tests();

  PROCESS_END();
}

#else

int main(void)
{
    do_tests();
}

#endif

// -----------------------------------------------------------

// soft:
// Starting tests, ARCH=sphere F_CPU=48 MHz
// Feature: mean (f)           Time: 24000 usec per 15000 samples
// Feature: mean (f, p)        Time: 31000 usec per 15000 samples
// Feature: magnitude (f)      Time: 148000 usec per 15000 samples

// normal (sqrtf from arm libmath):
// Feature: mean (f)           Time: 24000 usec per 15000 samples
// Feature: mean (f, p)        Time: 31000 usec per 15000 samples
// Feature: magnitude (f)      Time: 125000 usec per 15000 samples

// __sqrtf (hardware)
