#ifndef SIGNAL_DATA_H
#define SIGNAL_DATA_H

#include <stdlib.h>

// Types for data and indexes:
typedef float     data_t;
typedef u_int16_t index_t;

// Types for signal elements
typedef struct _point Point;
struct _point
{
    double x;
    double y;
};

typedef struct _peak Peak;
struct _peak
{   
    data_t amplitude;
    data_t position;
    data_t width;
};

typedef struct _noise Noise;
struct _noise
{
    data_t amplitude;
};

#endif