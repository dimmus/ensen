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
    float amplitude;
    double position;
    float width;
};

typedef struct _noise Noise;
struct _noise
{
    float amplitude;
};

#endif