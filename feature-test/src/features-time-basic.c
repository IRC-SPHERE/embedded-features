#include "qsort.h"

// -----------------------------------------------------------

void feature_mean_i_axis(int axis)
{
    int i;
    int32_t sum = 0;
    LOG("axis=%d\n", axis);
    for (i = 0; i < NSAMPLES; ++i) {
        if (i >= TIME_WINDOW_SIZE) {
            sum -= data[i - TIME_WINDOW_SIZE].v[axis];
        }
        sum += data[i].v[axis];

        OUTPUT_I(sum / TIME_WINDOW_SIZE, result_i.v[axis]);
        LOG("\n");
    }
}

void feature_mean_i(void)
{
    feature_mean_i_axis(0);
    feature_mean_i_axis(1);
    feature_mean_i_axis(2);
}

// -----------------------------------------------------------

void feature_mean_periodic_i_axis(int axis)
{
    int i, j;
    LOG("axis=%d\n", axis);
    for (i = 0; i <= NSAMPLES - TIME_WINDOW_SIZE; i += PERIODIC_COMPUTATION_WINDOW_SIZE) {
        int32_t sum = 0;
        for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
            sum += data[i + j].v[axis];
        }
        
        OUTPUT_I(sum / TIME_WINDOW_SIZE, result_i.v[axis]);
        LOG("\n");
    }
}

void feature_mean_periodic_i(void)
{
    feature_mean_periodic_i_axis(0);
    feature_mean_periodic_i_axis(1);
    feature_mean_periodic_i_axis(2);
}

// -----------------------------------------------------------

void feature_mean_f_axis(int axis)
{
    int i;
    float sum = 0;
    LOG("axis=%d\n", axis);
    for (i = 0; i < NSAMPLES; ++i) {
        if (i >= TIME_WINDOW_SIZE) {
            sum -= data[i - TIME_WINDOW_SIZE].v[axis];
        }
        sum += data[i].v[axis];
        OUTPUT_F(sum / TIME_WINDOW_SIZE, result_f.v[axis]);
        LOG("\n");
    }
}

void feature_mean_f(void)
{
    feature_mean_f_axis(0);
    feature_mean_f_axis(1);
    feature_mean_f_axis(2);
}

// -----------------------------------------------------------

void feature_mean_periodic_f_axis(int axis)
{
    int i, j;
    LOG("axis=%d\n", axis);
    for (i = 0; i <= NSAMPLES - TIME_WINDOW_SIZE; i += PERIODIC_COMPUTATION_WINDOW_SIZE) {
        float sum = 0;
        for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
            sum += data[i + j].v[axis];
        }

        OUTPUT_F(sum / TIME_WINDOW_SIZE, result_f.v[axis]);
        LOG("\n");
    }
}

void feature_mean_periodic_f(void)
{
    feature_mean_periodic_f_axis(0);
    feature_mean_periodic_f_axis(1);
    feature_mean_periodic_f_axis(2);
}

// -----------------------------------------------------------

void feature_magnitude_sq_i(void)
{
    int i;
    for (i = 0; i < NSAMPLES; ++i) {
        uint32_t m = (int32_t)data[i].v[0] * data[i].v[0]
                + data[i].v[1] * data[i].v[1]
                + data[i].v[2] * data[i].v[2];
        // note that this results in the squared magnitude!
        OUTPUT_I(m, result_i.v[0]);
        LOG("\n");
    }
}

// -----------------------------------------------------------

void feature_magnitude_f(void)
{
    int i;
    for (i = 0; i < NSAMPLES; ++i) {
        float r = (int32_t)data[i].v[0] * data[i].v[0]
                + data[i].v[1] * data[i].v[1]
                + data[i].v[2] * data[i].v[2];
#if 1
        OUTPUT_F(tsqrtf(r), result_f.v[0]);
#else
        // this requires linking with -lm
        OUTPUT_F(sqrtf(r), result_f.v[0]);
#endif
        LOG("\n");
    }
}

// -----------------------------------------------------------


void feature_min_axis(int axis)
{
    int i;
    int mn = INT_MAX;
    LOG("axis=%d\n", axis);
    for (i = 0; i < NSAMPLES; ++i) {
        mn = min(mn, data[i].v[axis]);

        OUTPUT_I(mn, result_i.v[axis]);
        LOG("\n");
    }
}

void feature_min(void)
{
    feature_min_axis(0);
    feature_min_axis(1);
    feature_min_axis(2);
}

// -----------------------------------------------------------

void feature_max_axis(int axis)
{
    int i;
    int mx = INT_MIN;
    LOG("axis=%d\n", axis);
    for (i = 0; i < NSAMPLES; ++i) {
        mx = max(mx, data[i].v[axis]);
        OUTPUT_I(mx, result_i.v[axis]);
        LOG("\n");
    }
}

void feature_max(void)
{
    feature_max_axis(0);
    feature_max_axis(1);
    feature_max_axis(2);
}

// -----------------------------------------------------------

void feature_min_periodic_axis(int axis)
{
    int i, j;
    LOG("axis=%d\n", axis);
    for (i = 0; i <= NSAMPLES - TIME_WINDOW_SIZE; i += PERIODIC_COMPUTATION_WINDOW_SIZE) {
        int mn = INT_MAX;
        for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
            mn = min(mn, data[i + j].v[axis]);
        }
        
        OUTPUT_I(mn, result_i.v[axis]);
        LOG("\n");
    }
}

void feature_min_periodic(void)
{
    feature_min_periodic_axis(0);
    feature_min_periodic_axis(1);
    feature_min_periodic_axis(2);
}

// -----------------------------------------------------------

void feature_max_periodic_axis(int axis)
{
    int i, j;
    LOG("axis=%d\n", axis);
    for (i = 0; i <= NSAMPLES - TIME_WINDOW_SIZE; i += PERIODIC_COMPUTATION_WINDOW_SIZE) {
        int mx = INT_MIN;
        for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
            mx = max(mx, data[i + j].v[axis]);
        }
        
        OUTPUT_I(mx, result_i.v[axis]);
        LOG("\n");
    }
}

void feature_max_periodic(void)
{
    feature_max_periodic_axis(0);
    feature_max_periodic_axis(1);
    feature_max_periodic_axis(2);
}

// -----------------------------------------------------------

void feature_variance_axis(int axis)
{
    int i;
    int32_t sum = 0;
    uint32_t squared_sum = 0;
    LOG("axis=%d\n", axis);
    for (i = 0; i < NSAMPLES; ++i) {
        if (i >= TIME_WINDOW_SIZE) {
            // remove a value
            int8_t v = data[i - TIME_WINDOW_SIZE].v[axis];
            sum -= v;
            squared_sum -= (int)v * v;
        }

        // add a value
        int8_t v = data[i].v[axis];
        sum += v;
        squared_sum += (int)v * v;

        int32_t avg = sum / TIME_WINDOW_SIZE;
        int32_t squared_avg = squared_sum / TIME_WINDOW_SIZE;

        OUTPUT_I(squared_avg - avg, result_i.v[axis]);
        LOG("\n");
    }
}

void feature_variance(void)
{
    feature_variance_axis(0);
    feature_variance_axis(1);
    feature_variance_axis(2);
}

// -----------------------------------------------------------

void feature_variance_periodic_axis(int axis)
{
    int i, j;
    LOG("axis=%d\n", axis);
    for (i = 0; i <= NSAMPLES - TIME_WINDOW_SIZE; i += PERIODIC_COMPUTATION_WINDOW_SIZE) {
        int32_t sum = 0;
        uint32_t squared_sum = 0;
        for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
            int8_t v = data[i + j].v[axis];
            sum -= v;
            squared_sum -= (int)v * v;
        }

        int32_t avg = sum / TIME_WINDOW_SIZE;
        int32_t squared_avg = squared_sum / TIME_WINDOW_SIZE;

        OUTPUT_I(squared_avg - avg, result_i.v[axis]);
        LOG("\n");
    }
}

void feature_variance_periodic(void)
{
    feature_variance_periodic_axis(0);
    feature_variance_periodic_axis(1);
    feature_variance_periodic_axis(2);
}

// -----------------------------------------------------------

void feature_std_axis(int axis)
{
    int i;
    int32_t sum = 0;
    uint32_t squared_sum = 0;
    LOG("axis=%d\n", axis);
    for (i = 0; i < NSAMPLES; ++i) {
        if (i >= TIME_WINDOW_SIZE) {
            // remove a value
            int8_t v = data[i - TIME_WINDOW_SIZE].v[axis];
            sum -= v;
            squared_sum -= (int)v * v;
        }

        // add a value
        int8_t v = data[i].v[axis];
        sum += v;
        squared_sum += (int)v * v;

        int32_t avg = sum / TIME_WINDOW_SIZE;
        int32_t squared_avg = squared_sum / TIME_WINDOW_SIZE;
        OUTPUT_F(tsqrtf(squared_avg - avg), result_f.v[axis]);
        LOG("\n");
    }
}

void feature_std(void)
{
    feature_std_axis(0);
    feature_std_axis(1);
    feature_std_axis(2);
}

// -----------------------------------------------------------

void feature_std_periodic_axis(int axis)
{
    int i, j;
    LOG("axis=%d\n", axis);
    for (i = 0; i <= NSAMPLES - TIME_WINDOW_SIZE; i += PERIODIC_COMPUTATION_WINDOW_SIZE) {
        int32_t sum = 0;
        uint32_t squared_sum = 0;
        for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
            int8_t v = data[i + j].v[axis];
            sum += v;
            squared_sum += (int)v * v;
        }

        int32_t avg = sum / TIME_WINDOW_SIZE;
        int32_t squared_avg = squared_sum / TIME_WINDOW_SIZE;

        OUTPUT_F(tsqrtf(squared_avg - avg), result_f.v[axis]);
        LOG("\n");
    }
}

void feature_std_periodic(void)
{
    feature_std_periodic_axis(0);
    feature_std_periodic_axis(1);
    feature_std_periodic_axis(2);
}

// -----------------------------------------------------------

void feature_zero_crossings_axis(int axis)
{
    int i;
    int8_t old1 = data[0].v[axis];
    int8_t old2 = data[0].v[axis];
    unsigned num_crossings = 0;
    LOG("axis=%d\n", axis);
    for (i = 1; i < NSAMPLES; ++i) {
        if (i >= TIME_WINDOW_SIZE) {
            int index = i - TIME_WINDOW_SIZE;
            if ((data[index].v[axis] > 0 && old1 < 0)
                || (data[index].v[axis] < 0 && old1 > 0)) {
                num_crossings--;
                old1 = data[index].v[axis];
            }
        }

        if ((data[i].v[axis] > 0 && old2 < 0)
                || (data[i].v[axis] < 0 && old2 > 0)) {
            old2 = data[i].v[axis];
            num_crossings++;
        }

        OUTPUT_I(num_crossings, result_i.v[axis]);
        LOG("\n");
    }
}

void feature_zero_crossings(void)
{
    feature_zero_crossings_axis(0);
    feature_zero_crossings_axis(1);
    feature_zero_crossings_axis(2);
}

// -----------------------------------------------------------

void feature_zero_crossings_periodic_axis(int axis)
{
    int i, j;
    LOG("axis=%d\n", axis);
    for (i = 0; i <= NSAMPLES - TIME_WINDOW_SIZE;
         i += PERIODIC_COMPUTATION_WINDOW_SIZE) {
        int8_t old = data[i].v[axis];
        unsigned num_crossings = 0;
        for (j = 1; j < TIME_WINDOW_SIZE; ++j) {
            if ((data[i + j].v[axis] > 0 && old < 0)
                    || (data[i + j].v[axis] < 0 && old > 0)) {
                num_crossings++;
                old = data[i + j].v[axis];
            }
        }
        OUTPUT_I(num_crossings, result_i.v[axis]);
        LOG("\n");
    }
}

void feature_zero_crossings_periodic(void)
{
    feature_zero_crossings_periodic_axis(0);
    feature_zero_crossings_periodic_axis(1);
    feature_zero_crossings_periodic_axis(2);
}
 
// -----------------------------------------------------------
