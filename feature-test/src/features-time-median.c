// ----------------------------------------------------------

void feature_median_axis_test(int axis, result_i_t *result)
{
    int i;
    // this algo (ab)uses the fact that only 256 values are possible
    int8_t stats[256] = {0};
    uint8_t median = data[0].v[axis] + 128;
    unsigned num_greater = 0;
    unsigned num_smaller = 0;
    unsigned num_total = 0;
    for(i = 0; i < NSAMPLES; ++i) {
        int v;
        if(i >= TIME_WINDOW_SIZE) {
            // remove a value
            v = data[i - TIME_WINDOW_SIZE].v[axis] + 128;
            stats[v]--;
            num_total--;
            if (v > median) {
                num_greater--;
            } else if (v < median) {
                num_smaller--;
            } else if(stats[v] == 0) {
                if (num_smaller > num_greater) {
                    while(stats[median] == 0) {
                        median--;
                    }
                    num_smaller -= stats[median];
                } else {
                    while(stats[median] == 0) {
                        median++;
                    }
                    num_greater -= stats[median];
                }
            }
        }

        // add a value
        v = data[i].v[axis] + 128;
        stats[v]++;
        num_total++;
        if (v > median) {
            num_greater++;
        } else if(v < median) {
            num_smaller++;
        }

        // rebalance
        if(num_greater > num_smaller + 1) {
            //printf("move up\n");
            if (num_greater > num_smaller + stats[median]) {
                // move up
                num_smaller += stats[median];
                median++;
                while(stats[median] == 0) {
                    median++;
                }
                num_greater -= stats[median];
            }
        }
        else if(num_smaller > num_greater + 1) {
            //printf("move down\n");
            if (num_smaller > num_greater + stats[median]) {
                // move down
                num_greater += stats[median];
                median--;
                while(stats[median] == 0) {
                    median--;
                }
                num_smaller -= stats[median];
            }
        }

        /* printf("%d: ", i); */
        /* printf("%d (%d vs %d)\n", (int8_t)(median - 128), num_smaller, num_greater); */

        /* if (axis == 0 && i > 350) { */
        /* printf("%d: ", i); */
        /* int j, k; */
        /* for(j = 0; j < 256; ++j) { */
        /*     for(k = 0; k < stats[j]; ++k) { */
        /*      printf("%d ", j - 128); */
        /*     } */
        /* } */
        /* printf("\n"); */
        /* } */

        result[i].v[axis] = (int8_t)(median - 128);
    }
}

void feature_median_axis(int axis)
{
    int i;
    // this algo (ab)uses the fact that only 256 values are possible
    int8_t stats[256] = {0};
    uint8_t median = data[0].v[axis] + 128;
    unsigned num_greater = 0;
    unsigned num_smaller = 0;
    unsigned num_total = 0;

    LOG("axis=%d\n", axis);
    for (i = 0; i < NSAMPLES; ++i) {
        int v;
        if (i >= TIME_WINDOW_SIZE) {
            // remove a value
            v = data[i - TIME_WINDOW_SIZE].v[axis] + 128;
            stats[v]--;
            num_total--;
            if (v > median) {
                num_greater--;
            } else if (v < median) {
                num_smaller--;
            } else if(stats[v] == 0) {
                if (num_smaller > num_greater) {
                    while(stats[median] == 0) {
                        median--;
                    }
                    num_smaller -= stats[median];
                } else {
                    while(stats[median] == 0) {
                        median++;
                    }
                    num_greater = stats[median];
                }
            }
        }

        // add a value
        v = data[i].v[axis] + 128;
        stats[v]++;
        num_total++;
        if (v > median) {
            num_greater++;
        } else if(v < median) {
            num_smaller++;
        }

        // rebalance
        if(num_greater > num_smaller + 1) {
            //printf("move up\n");
            if (num_greater > num_smaller + stats[median]) {
                // move up
                num_smaller += stats[median];
                median++;
                while(stats[median] == 0) {
                    median++;
                }
                num_greater -= stats[median];
            }
        }
        else if(num_smaller > num_greater + 1) {
            //printf("move down\n");
            if (num_smaller > num_greater + stats[median]) {
                // move down
                num_greater += stats[median];
                median--;
                while(stats[median] == 0) {
                    median--;
                }
                num_smaller -= stats[median];
            }
        }
 
        OUTPUT_I((int8_t)(median - 128), result_i.v[axis]);
        LOG("\n");
    }
}

// ----------------------------------------------------------

//
// This is the same algorithm idea as for median, but it also can
// get get data in arbitrary quintiles.
// The plain median is expected to be slightly faster.
//
void feature_quantile_axis_test(int axis, int low_multiplier, int high_multiplier, result_i_t *result)
{
    int i;
    // this algo (ab)uses the fact that only 256 values are possible
    int8_t stats[256] = {0};
    uint8_t quantile = data[0].v[axis] + 128;
    unsigned num_greater = 0;
    unsigned num_smaller = 0;
    unsigned num_total = 0;
    int total_multiplier = low_multiplier + high_multiplier;
    for(i = 0; i < NSAMPLES; ++i) {
        int v;
        if(i >= TIME_WINDOW_SIZE) {
            // remove a value
            v = data[i - TIME_WINDOW_SIZE].v[axis] + 128;
            stats[v]--;
            num_total--;
            if (v > quantile) {
                num_greater--;
            } else if (v < quantile) {
                num_smaller--;
            } else if(stats[v] == 0) {
                if (num_smaller > num_greater) {
                    while(stats[quantile] == 0) {
                        quantile--;
                    }
                    num_smaller -= stats[quantile];
                } else {
                    while(stats[quantile] == 0) {
                        quantile++;
                    }
                    num_greater -= stats[quantile];
                }
            }
        }

        // add a value
        v = data[i].v[axis] + 128;
        stats[v]++;
        num_total++;
        if (v > quantile) {
            num_greater++;
        } else if(v < quantile) {
            num_smaller++;
        }

        // rebalance
        if(num_greater * low_multiplier > num_smaller * high_multiplier + total_multiplier - 1) {
            //printf("move up\n");
            if (num_greater * low_multiplier > (num_smaller + stats[quantile]) * high_multiplier) {
                // move up
                num_smaller += stats[quantile];
                quantile++;
                while(stats[quantile] == 0) {
                    quantile++;
                }
                num_greater -= stats[quantile];
            }
        }
        else if(num_smaller * high_multiplier >
                num_greater * low_multiplier + total_multiplier - 1) {
            //printf("move down\n");
            if (num_smaller * high_multiplier > (num_greater + stats[quantile]) * low_multiplier) {
                // move down
                num_greater += stats[quantile];
                quantile--;
                while(stats[quantile] == 0) {
                    quantile--;
                }
                num_smaller -= stats[quantile];
            }
        }

        /* printf("%d: ", i); */
        /* printf("%d (%d vs %d)\n", (int8_t)(median - 128), num_smaller, num_greater); */

        /* printf("%d: ", i); */
        /* int j, k; */
        /* for(j = 0; j < 256; ++j) { */
        /*     for(k = 0; k < stats[j]; ++k) { */
        /*      printf("%d ", j - 128); */
        /*     } */
        /* } */
        /* printf("\n"); */
        /* } */

        result[i].v[axis] = (int8_t)(quantile - 128);
    }
}

// ----------------------------------------------------------


void feature_quantile_axis(int axis, int low_multiplier, int high_multiplier)
{
    int i;
    // this algo (ab)uses the fact that only 256 values are possible
    int8_t stats[256] = {0};
    uint8_t quantile = data[0].v[axis] + 128;
    unsigned num_greater = 0;
    unsigned num_smaller = 0;
    unsigned num_total = 0;
    int total_multiplier = low_multiplier + high_multiplier;

    LOG("axis=%d\n", axis);

    for (i = 0; i < NSAMPLES; ++i) {
        int v;
        if(i >= TIME_WINDOW_SIZE) {
            // remove a value
            v = data[i - TIME_WINDOW_SIZE].v[axis] + 128;
            stats[v]--;
            num_total--;
            if (v > quantile) {
                num_greater--;
            } else if (v < quantile) {
                num_smaller--;
            } else if(stats[v] == 0) {
                if (num_smaller > num_greater) {
                    while(stats[quantile] == 0) {
                        quantile--;
                    }
                    num_smaller -= stats[quantile];
                } else {
                    while(stats[quantile] == 0) {
                        quantile++;
                    }
                    num_greater -= stats[quantile];
                }
            }
        }

        // add a value
        v = data[i].v[axis] + 128;
        stats[v]++;
        num_total++;
        if (v > quantile) {
            num_greater++;
        } else if(v < quantile) {
            num_smaller++;
        }

        // rebalance
        if(num_greater * low_multiplier > num_smaller * high_multiplier + total_multiplier - 1) {
            //printf("move up\n");
            if (num_greater * low_multiplier > (num_smaller + stats[quantile]) * high_multiplier) {
                // move up
                num_smaller += stats[quantile];
                quantile++;
                while(stats[quantile] == 0) {
                    quantile++;
                }
                num_greater -= stats[quantile];
            }
        }
        else if(num_smaller * high_multiplier >
                num_greater * low_multiplier + total_multiplier - 1) {
            //printf("move down\n");
            if (num_smaller * high_multiplier > (num_greater + stats[quantile]) * low_multiplier) {
                // move down
                num_greater += stats[quantile];
                quantile--;
                while(stats[quantile] == 0) {
                    quantile--;
                }
                num_smaller -= stats[quantile];
            }
        }

        /* printf("%d: ", i); */
        /* int j, k; */
        /* for (j = 0; j < 256; ++j) { */
        /*     for (k = 0; k < stats[j]; ++k) { */
        /*      printf("%d ", j - 128); */
        /*     } */
        /* } */
        /* printf("\n"); */

        OUTPUT_I((int8_t)(quantile - 128), result_i.v[axis]);
        LOG("\n");
    }
}


// -----------------------------------------------------------


void feature_median(void)
{
    feature_median_axis(0);
    feature_median_axis(1);
    feature_median_axis(2);
}

// -----------------------------------------------------------

int cmp(const void *v1, const void *v2)
{
    const int8_t *x1 = v1;
    const int8_t *x2 = v2;
    return (int)*x1 - (int)*x2;
}

//
// A slow, per-block implementation of the quartiles feature.
//
void feature_quartiles_axis_periodic(int axis)
{
    int i;
    int8_t buffer[TIME_WINDOW_SIZE] = {0};
    LOG("axis=%d\n", axis);
    for (i = 0; i < NSAMPLES; ++i) {
        buffer[i % TIME_WINDOW_SIZE] = data[i].v[axis];
        if ((i + 1) % TIME_WINDOW_SIZE == 0) {
            qsort(buffer, TIME_WINDOW_SIZE, 1, cmp);

            OUTPUT_I(buffer[TIME_WINDOW_SIZE / 4], result_i.v[axis]); // 25%
            OUTPUT_I(buffer[TIME_WINDOW_SIZE / 2], result_i.v[axis]); // 50%
            OUTPUT_I(buffer[TIME_WINDOW_SIZE * 3 / 4], result_i.v[axis]); // 75
            LOG("\n");
        }
    }
}

void feature_quartiles_periodic(void)
{
    feature_quartiles_axis_periodic(0);
    feature_quartiles_axis_periodic(1);
    feature_quartiles_axis_periodic(2);
}

// -----------------------------------------------------------

//
// Faster implementation of quartiles that does not use sort but tracks just a single quartile.
//
void feature_quartile_25(void)
{
    // at 25%; 1=25% and 3=75% of 4 (total parts)
    feature_quantile_axis(0, 1, 3);
    feature_quantile_axis(1, 1, 3);
    feature_quantile_axis(2, 1, 3);
}

// -----------------------------------------------------------

void feature_quartile_75(void)
{
    // at 75%; 1=75% and 3=25% of 4 (total parts)
    feature_quantile_axis(0, 3, 1);
    feature_quantile_axis(1, 3, 1);
    feature_quantile_axis(2, 3, 1);
}

// -----------------------------------------------------------

void feature_select_nth_periodic_axis(int axis, int nth)
{
    int i, j;
    LOG("axis=%d\n", axis);
    for (i = 0; i < NSAMPLES - TIME_WINDOW_SIZE; i += PERIODIC_COMPUTATION_WINDOW_SIZE) {
        // put all data in bins and walk through the bins while the nth element is found
        int8_t stats[256] = {0};
        for (j = 0; j < TIME_WINDOW_SIZE; ++j) {
            int v = data[i + j].v[axis] + 128;
            stats[v]++;
        }
        for (j = 0; j < 256; ++j) {
            if(stats[j] >= nth) break;
            nth -= stats[j];
        }        
        OUTPUT_I(j - 128, result_i.v[axis]);
        LOG("\n");
    }
}


void feature_select_nth_periodic(int nth)
{
    feature_select_nth_periodic_axis(0, nth);
    feature_select_nth_periodic_axis(1, nth);
    feature_select_nth_periodic_axis(2, nth);    
}         

// -----------------------------------------------------------

void feature_quartile_25_periodic(void)
{
    feature_select_nth_periodic(TIME_WINDOW_SIZE / 4);
}

void feature_median_periodic(void)
{
    feature_select_nth_periodic(TIME_WINDOW_SIZE / 2);
}

void feature_quartile_75_periodic(void)
{
    feature_select_nth_periodic(TIME_WINDOW_SIZE *  3 / 4);
}


// -----------------------------------------------------------
