#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ensen_private.h"
#include "ensen_signal_form_random.h"
#include "math/random/ensen_math_random.h"

void
random_signal_generate(Random_Signal_Parameters rsp, data_t (*signal)[])
{
    // Setup bases.
    struct _basis
    {
        float frequency;
        float phase_offset;
        float amplitude;
    };

    // Create vector of bases.
    typedef struct _basis Basis;
    Basis bases[rsp.n_bases];
    
    // Generate bases.
    for(u_int8_t b = 0; b < rsp.n_bases; b++)
    {
        // Create new basis.
        Basis new_basis;
        // Get random frequency from 0 to max_frequency.
        new_basis.frequency = random_range_zero_one() * rsp.max_frequency;
        // Get random phase offset from 0 to 2*PI.
        new_basis.phase_offset = random_range_zero_one() * 2.0 * M_PI;
        // Get random amplitude from 0 to 1/frequency.  This gives more power to lower frequencies.
        new_basis.amplitude = random_range_zero_one() / new_basis.frequency;
        // Add basis to bases.
        // bases.push_back(new_basis);
        bases[b] = new_basis;
    }

    // Create signal.
    // Iterate over points.
    float point = 0;
    float t = 0;
    float N = (float)rsp.n_points;
    float sum = 0;
    float squared_sum = 0;
    for(u_int32_t n = 0; n < rsp.n_points; n++)
    {
        // Build up the point via superposition of the basis vectors.
        // Initialize point.
        point = 0;
        // Calculate t as time in single period.
        t = (float)(n) / (N-1);
        // Iterate over the bases.
        for(index_t bb = 0; bb < rsp.n_bases; bb++)
        {
            point += bases[bb].amplitude * sin(bases[bb].phase_offset + bases[bb].frequency * 2.0 * M_PI * t);
        }
        // Add point to the vector.        
        (*signal)[n] = point;
        // Add point to mean and standard deviation calculations.
        sum += point;
        squared_sum += point*point;
    }

    // Scale signal to desired mean and standard deviation.
    // Calculate mean and standard deviation.
    float mean = sum / N;
    float standard_deviation = sqrt((squared_sum - 2 * mean * sum + N * mean * mean)/N);
    // Calculate scale factor for standard deviation adjustment.
    float stdev_scale = rsp.desired_std_deviation / standard_deviation;
    for(index_t n = 0; n < rsp.n_points; n++)
    {
        (*signal)[n] = ((*signal)[n] - mean) * stdev_scale + rsp.desired_mean;
        // Add noise to the point if specified noise percentage is not zero.
        if(rsp.noise_percentage > 0)
        {
            (*signal)[n] += random_range_pm_one() * rsp.noise_percentage * rsp.desired_std_deviation;
        }
    }
}
