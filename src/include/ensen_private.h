#ifndef ENSEN_PRIVATE__H
#define ENSEN_PRIVATE_H

#ifdef __GNUC__
# if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#  define __UNUSED__ __attribute__ ((__unused__))
# else
#  define __UNUSED__
# endif
#endif

#include <time.h> // get_time()

double get_run_time(void);

double 
get_run_time(void)
{
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec + tv.tv_nsec / 1000000000.0;
}

#endif