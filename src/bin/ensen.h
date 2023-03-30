#ifndef ENSEN_H
#define ENSEN_H

#include "ensen_private.h"
#include "signal/ensen_benchmark.h"

#include "signal/ensen_signal.h"
#include "math/ensen_math.h"
#include "config/ensen_config.h"
#include "config/ensen_config_dictionary.h"
#include "ui/ensen_ui.h"

#define PLOT(data, from, to, title)       \
  for (index_t ii = from; ii <= to; ii++) \
  {                                       \
    data_points.x[ii] = ii;               \
    data_points.y[ii] = data;             \
  }                                       \
  gnuplot_plot_points(win, &data_points, to, title); \

int test_signal(const char * conf_name);
int test_signal_exp(const char * conf_name);
void find_peak_of_exp_gaussian(void);
void show_statistics(Signal_Parameters conf, Signal_Statistics stat, 
                     Points data_temp0, Points data_temp1, Points data_temp2, Points data_temp3, Points data_temp4);
void free_gnuplot(Signal_Parameters conf, gnuplot_ctrl **win);
void free_data(Signal_Parameters conf, Points data);

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