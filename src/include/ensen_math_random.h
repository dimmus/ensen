#ifndef ENSEN_MATH_RANDOM_H
#define ENSEN_MATH_RANDOM_H

#ifndef ENSEN_PRIVATE_H
    #include "ensen_private.h"
#endif

void random_seed(void);

int random_range(int lower, int upper);
int random_range_uniform(int rangeLow, int rangeHigh);

data_t random_zero_one(void);
data_t random_pm_one(void);

#endif