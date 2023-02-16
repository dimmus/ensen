#include "ensen_signal_generator.h"

// void
// signal_generate(Point (*points)[],
//                 index_t n_peaks,
//                 Peak peaks[],
//                 Noise noise,
//                 index_t n_points)
// {
//     index_t i;

//     for (i = 0; i < n_points; i++)
//     {
//         (*points)[i].x = i;
//         for (unsigned j = 0; j < n_peaks; j++)
//         {
//             (*points)[i].y += peaks[j].amplitude * gaussian((float)i, peaks[j].position, peaks[j].width);
//             if(noise.amplitude > 0) (*points)[i].y += noise.amplitude * random_pm_one();
//         }
//     }
// }

void
signal_generate(Points *points,
                index_t n_peaks,
                Peak peaks[],
                Noise noise,
                index_t n_points)
{
    index_t i;

    for (i = 0; i < n_points; i++)
    {
        // (*points).x[i] = i;
        for (unsigned j = 0; j < n_peaks; j++)
        {
            (*points).y[i] += peaks[j].amplitude * gaussian((*points).x[i], peaks[j].position, peaks[j].width);
            if (noise.amplitude > 0) (*points).y[i] += noise.amplitude * random_pm_one();
        }
    }
}
