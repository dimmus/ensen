#ifndef SIGNAL_DATA_H
#define SIGNAL_DATA_H

#include <stdlib.h>

// Types for data and indexes:
typedef double     data_t;
typedef u_int16_t index_t;

// Types for signal elements
typedef struct _point Point;
struct _point
{
    data_t x;
    data_t y;
};

typedef struct 
{
    data_t *x;
    data_t *y;
    data_t *lambda;
} Points;

typedef struct _peak Peak;
struct _peak
{   
    data_t amplitude;
    data_t position;
    data_t width;
};

typedef struct _peaks Peaks;
struct _peaks
{   
    Peak * peak;
    index_t total_number;
};

typedef struct _noise Noise;
struct _noise
{
    data_t amplitude;
};

typedef struct _plot Plot;
struct _plot
{
    data_t  x_min;
    data_t  x_max;
    data_t  y_min;
    data_t  y_max;
    index_t show_signal;
    index_t show_smooth;
    index_t show_derivative;
    index_t show_markers;
    index_t show_temperature;
};

typedef struct _peak_search Peak_Search;
struct _peak_search
{
    data_t  threshold_slope;
    data_t  threshold_amp;
    index_t drop_tail;
    index_t peaks_real_number;
    index_t peaks_array_number;
};

typedef struct _temp Temperature;
struct _temp
{
    data_t  room;
    data_t  max;
    index_t tick;
};

typedef struct _signal_parameters Signal_Parameters;
struct _signal_parameters
{
    index_t n_points;
    index_t n_peaks;
    Peak   *peak;
    Noise   noise;
    data_t  smooth_width;
    index_t generation_max;
    index_t generation_frequency;
    Temperature temp;
    Plot plot;
    Peak_Search search;
};

#endif