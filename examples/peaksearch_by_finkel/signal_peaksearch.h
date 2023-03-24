#ifndef SIGNAL_PEAKSEARCH_H
#define SIGNAL_PEAKSEARCH_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "signal_data.h"

// Whenever the output array gets full, it is expanded by this quantity:
#define CHUNK_SIZE 10

// Statistics
#define FULL    0
#define PARTIAL 1

#ifdef __cplusplus
extern "C"
{
#endif

// Structure holding algorithm data and parameters:
typedef struct {
  data_t  * data_v;   // vector of data
  index_t   data_s;   // number of data points
  data_t    sigmas;   // number of sigmas defining the threshold
  index_t   win_size; // moving windows size
  data_t    stdev;    // search_peaks() will put here the overal st.dev.
} PeakSearch;

typedef struct {
  data_t  mean;
  data_t  sd;
  data_t  max;
  index_t max_idx;
} Statistics;

data_t
mean(PeakSearch       * ps,
     index_t    const   start,
     index_t    const   end);

void
statistics(PeakSearch       * ps,
           index_t    const   start,
           char       const   type,
           Statistics       * stat);

// Performs the peak search on the data and according to the parameters in ps.
// The indexes of found peaks are returned in peaks_v.
// ps.stdev will be loaded with the overall standard deviation.
index_t
search_data_peaks(PeakSearch *ps, index_t **peaks_v, data_t *d, index_t n_points);

#ifdef __cplusplus
}
#endif

#endif
