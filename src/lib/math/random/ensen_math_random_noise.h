/**
   @brief   Implements gebnerationm of colored noise.

   This module implements noise filters of different colors: 
   white, brown, violet, pink and blue with different filtering algorithms.
*/
#ifndef ENSEN_MATH_RANDOM_NOISE_H
#define ENSEN_MATH_RANDOM_NOISE_H

#include "ensen_private.h"

/// @brief PDF for Gaussian Noise
/// @param x
/// @return data_t
data_t gaussianPDF(data_t x);

/// @brief CDF for Gaussian Noise
/// @param x
/// @return data_t
data_t gaussianCDF(data_t x);

/// @brief Newtons method to reconstruct noise value from
/// random number thrown on [0,1).
/// @param PDF
/// @param CDF
/// @param V
/// @return data_t
data_t NEWTON(data_t (*PDF)(data_t), data_t (*CDF)(data_t), data_t V);

// Noise Generators. All colors of noise

/// @brief White Noise Generator
/// @param  no
/// @return value of type data_t
/// The key to it all ! All function utilize genWhiteNoise a
/// a basis for colored noise generation.
data_t genWhiteNoise(void);

/// @brief Brown noise generator (corrected)
/// @param  no
/// @return value of type data_t
/// For Brown noise audio, it is necessary to prevent the noise
/// generator random walking too far from zero. So we need two
/// bounding functions for nice clean brown noise.
data_t genBrownNoiseCorr(void);

/// @brief Brown noise generator (pure)
/// @param  
/// @return value of type data_t
data_t genBrownNoisePure(void);

/// @brief Violet Noise Generator
/// Generate white noise and take discrete derivative
/// @param  no
/// @return value of type data_t
data_t genVioletNoise(void);

/// @brief Pink noise generator
/// Complicated. Instead of generating a value, a seed vector of
/// white noise is generated. The autocorrelation for 1/f noise
/// is then invoked for all subsequent noise values.
/// @param P 
/// @param A 
/// @param depth 
/// @return value of type data_t
data_t genPinkNoise(data_t *P, data_t *A, int depth);

/// @brief Azure Noise Generator
/// Same thing as pink noise, only a violet noise vector is used as a
/// seed vector. Note that the 'pinking' of violet noise produces blue
/// noise (i.e. noise fractioning)
/// @param B 
/// @param A 
/// @param depth 
/// @return value of type data_t
data_t genBlueNoise(data_t *B, data_t *A, int depth);

/// @brief Autocorrrelation functions for pink 
/// and blue noise generation
/// @param depth 
/// @param alpha 
/// @return value of type data_t
data_t *autoCorr(int depth, data_t alpha);

/// @brief Initialization for pink noise generation
/// @param depth 
/// @param alpha 
/// @return value of type data_t
data_t *initPink(int depth, data_t alpha);

/// @brief Initialization for blue noise generation
/// @param depth 
/// @param alpha 
/// @return value of type data_t
data_t *initBlue(int depth, data_t alpha);

#endif