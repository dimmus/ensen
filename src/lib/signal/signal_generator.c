#include "ensen_signal_generator.h"
#include "ensen_benchmark.h"

data_t
signal_generate(Points *points,
                index_t n_peaks,
                Peak peaks[],
                Noise noise,
                index_t n_points)
{
    index_t i = 0;
    double start_time = get_run_time();

    for (i = 0; i < n_points; i++)
    {
        // (*points).x[i] = i;
        for (unsigned j = 0; j < n_peaks; j++)
        {
            (*points).y[i] += peaks[j].amplitude * gaussian((*points).x[i], peaks[j].position, peaks[j].width);
            if (noise.amplitude > 0)
            {
              if (noise.color == 0) (*points).y[i] += noise.amplitude * random_pm_one();  
              if (noise.color == 1) (*points).y[i] += noise.amplitude * genWhiteNoise();
              if (noise.color == 2) (*points).y[i] += noise.amplitude * genBrownNoiseCorr();
              if (noise.color == 3) (*points).y[i] += noise.amplitude * genVioletNoise();
            //   if (noise_color == 4) (*points).y[i] += noise.amplitude * genPinkNoise(WHAT_TO_SET_HERE);
            //   if (noise_color == 4) (*points).y[i] += noise.amplitude * genBlueNoise(WHAT_TO_SET_HERE);
            } 
        }
    }

    double end_time = get_run_time();
    return end_time - start_time;
}

