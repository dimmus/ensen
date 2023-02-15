#ifndef MATH_RANDOM_H
#define MATH_RANDOM_H

void random_seed(void);

int random_range(int lower, int upper);
int random_range_uniform(int rangeLow, int rangeHigh);

double random_zero_one(void);
double random_pm_one(void);

#endif