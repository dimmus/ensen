#include "signal_data.h"
#include "signal_generator.h"
#include "signal_form_gaussian.h"
#include "math_random.h"

void
signal_generate(Point (*points)[],
                index_t n_peaks,
                Peak peaks[],
                Noise noise,
                index_t n_points)
{
    index_t i;

    for (i = 0; i < n_points; i++)
    {
        (*points)[i].x = i;
        for (unsigned j = 0; j < n_peaks; j++)
        {
            (*points)[i].y += peaks[j].amplitude * gaussian((float)i, peaks[j].position, peaks[j].width);
            if(noise.amplitude > 0) (*points)[i].y += noise.amplitude * random_pm_one();
        }
    }
}
