#ifndef ENSEN_BENCHMARK_H
#define ENSEN_BENCHMARK_H

#include <time.h> // get_time()

static double 
get_run_time(void)
{
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec + tv.tv_nsec / 1000000000.0;
}

#endif