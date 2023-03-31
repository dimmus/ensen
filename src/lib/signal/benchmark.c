#include "signal/ensen_benchmark.h"

#include <time.h> // get_time()

double 
get_run_time(void)
{
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec + tv.tv_nsec / 1000000000.0;
}