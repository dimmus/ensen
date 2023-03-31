#include "ensen_exp.h"
#include <fftw3.h>

double
exp_broaden(const index_t size, double * in, double * out, const double t) 
{
  double start_time = get_run_time();

  const index_t n = size * 2;
  fftw_complex in1[n], out1[n], in2[n], out2[n], in3[n], out3[n];
  double real = 0.f, imag = 0.f;
  index_t i = 0;

  // PLOT(in[ii], 0, size, "Input data");

  // Plan 1:  rearange input signal (double)
  fftw_plan p1 = fftw_plan_dft_1d(n, in1, out1, FFTW_FORWARD, FFTW_ESTIMATE);
  index_t hfl = round(size/2);
  for (i = 0; i < n; ++i) {
    real = ((i < hfl) || (i > (hfl + size))) ? in[1] * 1.0 : in[i - hfl + 1]; // asume symmetrical tails of signal
    imag = 0.f;
    in1[i] = real + I * imag;
  }
  // PLOT(creal(in1[ii]), 0, n, "Streatched input data");
  fftw_execute(p1);
  // PLOT(creal(out1[ii]), 0, n, "FFTW(data)");
  fftw_destroy_plan(p1);

  // Plan 2: create exponential signal
  fftw_plan p2 = fftw_plan_dft_1d(n, in2, out2, FFTW_FORWARD, FFTW_ESTIMATE);
  double sum = 0;
  for (i = 0; i < n; ++i) {
      real = exp(-(i+1)/t);
      imag = 0.f;
      in2[i] = real + I * imag;
      sum += real;
  }
  // PLOT(creal(in2[ii]), 0, n, "a=exp");
  fftw_execute(p2);
  // PLOT(creal(out2[ii]), 0, n, "FFTW(a)");
  fftw_destroy_plan(p2);

  // Plan 3: Multiply to FFT signals and find inverse
  fftw_plan p3 = fftw_plan_dft_1d(n, in3, out3, FFTW_FORWARD, FFTW_ESTIMATE);
  for (i = 0; i < n; i++)
  {
    in3[i] = out1[i] * out2[i];
  }
  // PLOT(creal(in3[ii]), 0, n, "FFT(data) * FFT(a)");
  fftw_execute(p3);
  // PLOT((creal(out3[ii])/sum)/n, 0, n, "IFFT");
  fftw_destroy_plan(p3);

  // Compress
  // PLOT(in0[ii], 0, size, "Input data");
  index_t ii = 0;
  for (index_t p = (n-(size/2)+1); p >= (size/2+2); p--)
  {
    out[ii] = (creal(out3[p])/sum)/n;
    ii = ii + 1;
  }

  double end_time = get_run_time();
  return end_time - start_time;
}

double
exp_gaussian(const index_t size, double * in, double * out, double pos, double wid, double timeconstant)
{
#ifdef LOG_TIME
  double start_time = get_run_time();
#endif
  // Exponentially-convoluted gaussian(x,pos,wid) = gaussian peak centered on pos, half-width=wid
  // x may be scalar, vector, or matrix, pos and wid both scalar
  data_t yy[size];
  data_t arg = 0.0;
  for (index_t i = 0; i < size; i++)
  {
    arg = (in[i] - pos)/(0.60056120439323 * wid);
    yy[i] = exp(-(arg * arg));
  }

#ifdef LOG_TIME
  double t_broden = exp_broaden(size, yy, out, timeconstant);
  printf("Time exp broadening:\t%f\n", t_broden);
  double end_time = get_run_time();
  return (end_time - start_time) - t_broden;
#else
  exp_broaden(size, yy, out, timeconstant);
  return 0.f;
#endif
}

double
signal_generate_exp(Points *points, index_t n_peaks, Peak peaks[], Noise noise, index_t n_points)
{
    index_t i = 0, j = 0;
    data_t y[n_points];
    data_t ampl_coeff = 0.f;
#ifdef LOG_TIME
    double time_exp = 0.f;
#endif
    double start_time = get_run_time();

    for (j = 0; j < n_peaks; j++)
    {
      for (index_t n = 0; n < n_points; n++) y[n] = 0.0;
#ifdef LOG_TIME
      time_exp = exp_gaussian(n_points, (*points).x, y, peaks[j].position, peaks[j].width, peaks->timeshift);
      printf("Time exp for peak %d:\t%f\n",   j, time_exp);
      printf("Amplitude of peak %d:\t%f\n",   j, peaks[j].amplitude);
#else
      exp_gaussian(n_points, (*points).x, y, peaks[j].position, peaks[j].width, peaks->timeshift);
#endif
      ampl_coeff = peaks[0].amplitude / max(y, n_points);
      for (i = 0; i < n_points; i++)
      {
        (*points).y[i] += peaks[j].amplitude * ampl_coeff * y[i];
        if (noise.amplitude > 0) (*points).y[i] += noise.amplitude * random_pm_one();
      }
    }
    double end_time = get_run_time();
    return end_time - start_time;
}
