#ifndef ENSEN_PRIVATE_H
#define ENSEN_PRIVATE_H

#ifdef __GNUC__
# if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#  define __UNUSED__ __attribute__ ((__unused__))
# else
#  define __UNUSED__
# endif
#endif

/* hint to make sure function result is actually used */
#ifdef __GNUC__
#  define ATTR_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#  define ATTR_WARN_UNUSED_RESULT
#endif

/* the function return value points to memory (2 args for 'size * tot') */
#if defined(__GNUC__) && !defined(__clang__)
#  define ATTR_ALLOC_SIZE(args...) __attribute__((alloc_size(args)))
#else
#  define ATTR_ALLOC_SIZE(...)
#endif

/* hint to mark function arguments expected to be non-null
 * if no arguments are given to the macro, all of pointer
 * arguments would be expected to be non-null
 */
#ifdef __GNUC__
#  define ATTR_NONNULL(args...) __attribute__((nonnull(args)))
#else
#  define ATTR_NONNULL(...)
#endif

/* hint to treat any non-null function return value cannot alias any other pointer */
#ifdef __GNUC__
#  define ATTR_MALLOC __attribute__((malloc))
#else
#  define ATTR_MALLOC
#endif

#if (defined(__GNUC__) || defined(__clang__)) && defined(__cplusplus)
extern "C++" {
/** Some magic to be sure we don't have reference in the type. */
template<typename T> static inline T decltype_helper(T x)
{
  return x;
}
#define typeof(x) decltype(decltype_helper(x))
}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> /* bool */
#include <stdint.h> /* SIZE_MAX */

typedef unsigned int uint;

/* Color */
#define RED(string)     "\x1b[31m" string "\x1b[0m"
#define GREEN(string)   "\x1b[32m" string "\x1b[0m"
#define YELLOW(string)  "\x1b[33m" string "\x1b[0m"
#define BLUE(string)    "\x1b[34m" string "\x1b[0m"
#define MAGENTA(string) "\x1b[35m" string "\x1b[0m"
#define CYAN(string)    "\x1b[36m" string "\x1b[0m"

/* Time */
#include <unistd.h> /* usleep() */
extern int usleep (__useconds_t __useconds); /* from <unistd.h> */

/* Show LOG */
/* #define LOG_TIME */

/* Data */
typedef double     data_t;
typedef u_int16_t index_t;

typedef struct _point Point;
struct _point
{
    data_t x;
    data_t y;
};

typedef struct
{
    data_t * x;
    data_t * y;
    data_t * lambda;
} Points;

typedef struct _peak Peak;
struct _peak
{
    data_t amplitude;
    data_t position;
    data_t width;
    data_t timeshift;
};

typedef struct _peaks Peaks;
struct _peaks
{
    Peak   * peak;
    index_t  total_number;
};

typedef struct _noise Noise;
struct _noise
{
    data_t amplitude;
    index_t color;
};

typedef struct _smooth Smooth;
struct _smooth
{
    index_t width;
    index_t level;
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
    index_t show_vs_smooth;
    index_t show_vs_noise;
};

typedef struct _peak_search Peak_Search;
struct _peak_search
{
    data_t  threshold_slope;
    data_t  threshold_amp;
    index_t drop_tail;
    index_t peaks_real_number;
    index_t peaks_array_number;
    index_t peak_search_number;
};

typedef struct _temp Temperature;
struct _temp
{
    data_t  room;
    data_t  max;
    index_t tick;
    data_t  coefficient;
    index_t apply;
};

typedef struct _signal_parameters Signal_Parameters;
struct _signal_parameters
{
    index_t      n_points;
    index_t      n_peaks;
    Peak       * peak;
    Noise        noise;
    Smooth       smooth;
    index_t      generation_max;
    index_t      generation_frequency;
    Temperature  temp;
    Plot         plot;
    Peak_Search  search;
};

typedef struct _signal_statistics Signal_Statistics;
struct _signal_statistics
{
    data_t generation_time;
    data_t peak_search_time;
    index_t n_drops;
    data_t delta_temp;
};

#endif
