#ifndef ENSEN_MATH_H
#define ENSEN_MATH_H

#include <math.h>
#include <complex.h>

#include "math/random/ensen_math_random.h"
#include "math/random/ensen_math_random_noise.h"
// #include "math/random/ensen_math_fftw_utils.h"

#define REAL(z,i) ((z)[2*(i)]) // gsl_fft
#define IMAG(z,i) ((z)[2*(i)+1])  // gsl_fft

#include <gsl/gsl_statistics.h>
#include <gsl/gsl_fit.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft.h>
#include <gsl/gsl_fft_complex.h>

#endif