#ifndef SIGNAL_GENERATOR_H
#define SIGNAL_GENERATOR_H

#include "signal_data.h"

/**
    @brief Generate signal with multiple strictly defined peaks
    @param points Array of points to generate (x and y)  
    @param n_peaks Number of peak to generate  
    @param peaks Array of peak parameters
    @param noise Noise parameters
    @param n_points Number of point to generate
    @return   void

    Generate signal with n_points with multiple peaks defined by the n_peaks number 
    and peaks array with parameters for each peak. 

    @code
    index_t n_points = 1000;
    Point (*points)[] = malloc(sizeof(double) * 2 * n_points);

    index_t n_peaks = 3;
    Peak peaks[n_peaks];

    peaks[0].amplitude = 1.0;
    peaks[0].position = 300.0;
    peaks[0].width = 100.0;

    peaks[1].amplitude = 0.8;
    peaks[1].position = 500.0;
    peaks[1].width = 100.0;

    peaks[2].amplitude = 0.6;
    peaks[2].position = 700.0;
    peaks[2].width = 100.0;

    Noise noise;
    noise.amplitude = 0.2;

    signal_generate(points, n_peaks, peaks, noise, n_points);
    @endcode
**/
void signal_generate(Point (*points)[], index_t n_peaks, Peak peaks[], Noise noise, index_t n_points);

#endif