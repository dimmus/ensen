#ifndef SIGNAL_FORM_RANDOM_H
#define SIGNAL_FORM_RANDOM_H

#include "signal_data.h"
// typedef float Signal[];
// typedef Signal Signals[];

typedef struct _random_signal_parameters Random_Signal_Parameters;

struct _random_signal_parameters
{
    index_t n_points;        /* the length of the generated signal as a number of points */
    float desired_mean;          /* the average amplitude of the generated signal */
    float desired_std_deviation; /* the standard deviation of the generated signal's amplitude from desired_mean */
    index_t  n_bases;        /* (OPTIONAL)  This controls the number of random sine waves that are superimposed to create the random signal. More bases increase the complexity of the signal but takes longer to compute */
    float max_frequency;         /* (OPTIONAL) The maximum frequency of the signal's content, in Hz. DEFAULT = 10.0. This controls the maximum number of peaks/valleys in the signal over the specified number of points */
    float noise_percentage;      /* (OPTIONAL) he amount of noise to superimpose on the signal, in percentage of the desired standard deviation. DEFAULT = 0.0. This enables Gaussian (white) noise to be added to the random signal*/
};

void random_signal_generate(Random_Signal_Parameters rsp, data_t (*signal)[]);

#endif