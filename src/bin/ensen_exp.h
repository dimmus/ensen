#ifndef ENSEN_EXP_H
#define ENSEN_EXP_H

#include "math/ensen_math.h"
#include "signal/ensen_benchmark.h"
#include "signal/ensen_signal.h"

double exp_broaden(const index_t size, double * in, double * out, const double t);
double exp_gaussian(const index_t size, double * in, double * out, double pos, double wid, double timeconstant);

// double exp_gaussian(const index_t size, double * in, double * out, double pos, double wid, double timeconstant);

/// @brief Zero pads input and convolutes result by an exponential decay
/// of time constant "t" by multiplying Fourier transforms and inverse
/// transforming the result.
/// @param size Size of input
/// @param input Input data array 
/// @param output Output data array
/// @param t Time constant
/// @return Time of exacution (in sec.)
// double exp_broaden(const index_t size, double * input, double * output, const double t);

double signal_generate_exp(Points *points, index_t n_peaks, Peak peaks[], Noise noise, index_t n_points);

#endif