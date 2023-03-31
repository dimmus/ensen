#ifndef ENSEN_SHOW_H
#define ENSEN_SHOW_H

#include "signal/ensen_signal.h"

void show_statistics(Signal_Parameters conf, Signal_Statistics stat, Points temp_gen, Points temp_sens_1, Points temp_sens_2, Points temp_sens_3, Points temp_sens_4);
void show_generator_info(const Signal_Parameters conf, const Signal_Statistics stat, const index_t n_step);
void show_psearch_info(const Signal_Parameters conf, const index_t i_gen, const index_t i_sens, const index_t n_peaks, 
                  const Points temp_sens_1, const Points temp_sens_2, const Points temp_sens_3, const Points temp_sens_4);

#endif