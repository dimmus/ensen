#ifndef ENSEN_DATA_H
#define ENSEN_DATA_H

#include "ensen_private.h"

void data_clear(data_t * x, const index_t size);
void data_convert_to_lambda(data_t * lambda, const data_t lambda_begin, const data_t lambda_end, const index_t size);
void data_arrays_set(const Signal_Parameters conf, PointsArrays points);

#endif