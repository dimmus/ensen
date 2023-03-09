#include <stdio.h>
#include <stdlib.h>
#include <time.h> // for random seed with srand48()
#include <sys/time.h>

#include "ensen_math_random.h"

// Seed random generator using current time
void
random_seed(void)
{
    // VAR 1: using time.h
	// srand48(time(NULL));
    srand(time(NULL) ^ (getpid() << 16));

    // VAR 2: using sys/time.h
    // struct timeval tv;
    // gettimeofday(&tv,NULL);
    // srand(tv.tv_usec);
}

// Generates random numbers in range [lower, upper].
int 
random_range(int lower, int upper)
{
	return (rand() % (upper - lower + 1)) + lower;
}

// Generate uniform distribution of random numbers in range
int 
random_range_uniform(int lower, int upper) {
    double myRand = rand()/(1.0 + RAND_MAX); 
    int range = upper - lower + 1;
    int myRand_scaled = (myRand * range) + lower;
    return myRand_scaled;
}

data_t 
random_zero_one(void)
{
    return (data_t)drand48();
}

data_t 
random_pm_one(void)
{
    return ((double)rand()/RAND_MAX)*2 - 1;
    // return (data_t)mrand48()/RAND_MAX;
}

