#include <stdint.h>

#ifndef MAIN_H
#define MAIN_H

#define SAMPLING_HZ 20

#ifndef TIME_WINDOW_SIZE
#define TIME_WINDOW_SIZE 64
#endif

// This may be bigger, assuming that it would make it work better.
// It can't be too big on a real device because of RAM limitations.
#ifndef FREQUENCY_FEATURE_WINDOW_SIZE
#define FREQUENCY_FEATURE_WINDOW_SIZE 64
#endif

#if TIME_WINDOW_SIZE == 32
#pragma message "TIME_WINDOW_SIZE == 32"
#elif TIME_WINDOW_SIZE == 64
#pragma message "TIME_WINDOW_SIZE == 64"
#elif TIME_WINDOW_SIZE == 128
#pragma message "TIME_WINDOW_SIZE == 128"
#endif

// Once per second
#define PERIODIC_COMPUTATION_WINDOW_SIZE SAMPLING_HZ

#define VERY_FAST 0
#define FAST 1
#define MODERATE 2
#define SLOW 3

// If this is set to true, repetitions are not done;
// instead, the output is logged for each.
#ifndef DO_LOG_OUTPUT
#define DO_LOG_OUTPUT 0
#endif

#if DO_LOG_OUTPUT

static const int NUM_REPETITIONS[4] = {
    [VERY_FAST] = 1,
    [FAST] = 1,
    [MODERATE] = 1,
    [SLOW] = 1
};

#else // DO_LOG_OUTPUT

#if CONTIKI_TARGET_SRF06_CC26XX || CONTIKI_TARGET_ZOUL || CONTIKI_TARGET_NRF52DK
static const int NUM_REPETITIONS[4] = {
    [VERY_FAST] = 1000,
    [FAST] = 10,
    [MODERATE] = 1,
    [SLOW] = 1
};
#elif CONTIKI_TARGET_Z1
static const int NUM_REPETITIONS[4] = {
    [VERY_FAST] = 10,
    [FAST] = 10,
    [MODERATE] = 1,
    [SLOW] = 1
};
#else
static const int NUM_REPETITIONS[4] = {
    [VERY_FAST] = 10000,
    [FAST] = 1000,
    [MODERATE] = 100,
    [SLOW] = 10
};
#endif
#endif // DO_LOG_OUTPUT


#endif
