/**
   @brief   Implements random number generation and seed.

   This module implements different seed mechanizme and 
   random nubers generation with different algorithms and ranges.
*/
#ifndef ENSEN_MATH_RANDOM_H
#define ENSEN_MATH_RANDOM_H

#include "ensen_private.h"

/// @brief Initializes the Mersenne Twister algorithm with a seed value.
///  To use:
///    srand(time(NULL));
///    mt_init(rand());
/// @param seed 
void mt_init(unsigned long seed);

/// @brief Generates a random number using the Mersenne Twister algorithm
/// @param no 
/// @return value of type unsigned long
unsigned long mt_rand(void);

/// @brief Seed for random number generation
/// @param no
void init_rnd(void);

/// @brief Random number generator
/// @param no
/// @return data_t
data_t rnd(void);

/// @brief Seed random generator using current time
/// @param algorithm Algorithm of random seed (1 - srand, 2 - srand48) 
void random_seed(index_t algorithm);

/// @brief Generates random numbers in range
/// @param lower Lower range limit value 
/// @param upper Upper range limit value
/// @return random value of type int
int random_range(int lower, int upper);

/// @brief Generates random numbers in range uniformly
/// @param lower Lower range limit value 
/// @param upper Upper range limit value
/// @return random value of type int
int random_range_uniform(int lower, int upper);

/// @brief Generates random numbers in range (0,1)
/// @param  no
/// @return random value of type data_t
data_t random_range_zero_one(void);

/// @brief Generates random numbers in range (-1,1)
/// @param  no
/// @return random value of type data_t
data_t random_range_pm_one(void);

#endif