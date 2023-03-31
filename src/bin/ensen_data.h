#ifndef ENSEN_DATA_H
#define ENSEN_DATA_H

#include "ensen_private.h"

void data_clear(data_t * x, const index_t size);
void data_convert_to_lambda(data_t * lambda, const data_t lambda_begin, const data_t lambda_end, const index_t size);
void data_arrays_set(Signal_Parameters conf, Points *data, Points *data_temp,
                Points *temp_gen, Points *temp_sens_1, Points *temp_sens_2, Points *temp_sens_3, Points *temp_sens_4);

#endif