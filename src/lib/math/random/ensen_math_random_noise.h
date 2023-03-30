#ifndef ENSEN_MATH_RANDOM_NOISE_H
#define ENSEN_MATH_RANDOM_NOISE_H

// For random number generation
void init_rnd(void);
float rnd(void);

// PDF and CDF for Gaussian Noise
float gaussianPDF(float x);
float gaussianCDF(float x);

// Newtons Method for Calculating Gaussian noise value
float NEWTON(float(*PDF)(float), float(*CDF)(float), float V);

// Noise Generators. All colors of noise
float genWhiteNoise(void);
float fixBrownNoiseU(float x);
float fixBrownNoiseL(float x);
float genBrownNoiseCorr(void);
float genBrownNoisePure(void);
float genVioletNoise(void);
float genPinkNoise(float* P, float* A, int depth);
float genBlueNoise(float* B, float* A, int depth);

// Autocorrrelation functions  and initialization
// for pink and blue noise generation 
float* autoCorr(int depth, float alpha);
float* initPink(int depth, float alpha);
float* initBlue(int depth, float alpha);

#endif