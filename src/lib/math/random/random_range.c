#include <stdio.h>
#include <stdlib.h>
// #include <time.h> // for random seed with srand48()
#include <sys/time.h>

#include "math_random.h"

// Seed random generator using current time
void
random_seed(void)
{
    // VAR 1: using time.h
	// srand48(time(NULL));

    // VAR 2: using sys/time.h
    struct timeval tv;
    gettimeofday(&tv,NULL);
    srand(tv.tv_usec);
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

double 
random_zero_one(void)
{
    return drand48();
}

double 
random_zero_one_mt19937(void)
{
    return random_mt19937ar_cok_real1();
}

double 
random_pm_one(void)
{
    // return ((double)rand()/RAND_MAX)*2 - 1;
    return (double)mrand48()/RAND_MAX;
}

double 
random_pm_one_mt19937(void)
{
    return random_mt19937ar_cok_real1()*2 - 1;
}

