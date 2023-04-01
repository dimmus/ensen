#ifndef ENSEN_MATH_RANDOM_NOISE_H
#define ENSEN_MATH_RANDOM_NOISE_H

#include "ensen_private.h"

// Mersenne Twister algorithm
void mt_init(unsigned long seed);
unsigned long mt_rand(void);

// For random number generation
void init_rnd(void);
data_t rnd(void);

// PDF and CDF for Gaussian Noise
data_t gaussianPDF(data_t x);
data_t gaussianCDF(data_t x);

// Newtons Method for Calculating Gaussian noise value
data_t NEWTON(data_t(*PDF)(data_t), data_t(*CDF)(data_t), data_t V);

// Noise Generators. All colors of noise
data_t genWhiteNoise(void);
data_t fixBrownNoiseU(data_t x);
data_t fixBrownNoiseL(data_t x);
data_t genBrownNoiseCorr(void);
data_t genBrownNoisePure(void);
data_t genVioletNoise(void);
data_t genPinkNoise(data_t* P, data_t* A, int depth);
data_t genBlueNoise(data_t* B, data_t* A, int depth);

// Autocorrrelation functions  and initialization
// for pink and blue noise generation 
data_t* autoCorr(int depth, data_t alpha);
data_t* initPink(int depth, data_t alpha);
data_t* initBlue(int depth, data_t alpha);

#endif