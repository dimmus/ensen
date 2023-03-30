#include <stdlib.h> // clearscr()
#include <stdio.h>
#include <stdbool.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef ENABLE_NLS
# include <libintl.h>
#endif

#include "ensen.h"
#include "ensen_mem_guarded.h"

static int
clearscreen(void)
{
  return system("cls||clear"); // win and linux
  // printf("\e[1;1H\e[2J"); // warning: non ISO-complient
}

static int
config_parameters_set(Signal_Parameters * param, dictionary *ini)
{
  // config_dump(ini, stderr);

  /* Peaks setup */
  (*param).n_points             = (index_t)config_getint(ini, "points:number", -1);
  (*param).n_peaks              = (index_t)config_getint(ini, "peaks:number", -1);

  (*param).peak = MEM_malloc_arrayN(4, sizeof(Peak), "config_parameters_set: peak[4]");
  (*param).peak->timeshift      = (index_t)config_getint(ini, "peaks:timeshift", -1);

  (*param).peak[0].amplitude    = (data_t)config_getdouble(ini, "peaks:[0].amplitude", -1.0);
  (*param).peak[0].position     = (data_t)config_getdouble(ini, "peaks:[0].position", -1.0);
  (*param).peak[0].width        = (data_t)config_getdouble(ini, "peaks:[0].width", -1.0);

  (*param).peak[1].amplitude    = (data_t)config_getdouble(ini, "peaks:[1].amplitude", -1.0);
  (*param).peak[1].position     = (data_t)config_getdouble(ini, "peaks:[1].position", -1.0);
  (*param).peak[1].width        = (data_t)config_getdouble(ini, "peaks:[1].width", -1.0);

  (*param).peak[2].amplitude    = (data_t)config_getdouble(ini, "peaks:[2].amplitude", -1.0);
  (*param).peak[2].position     = (data_t)config_getdouble(ini, "peaks:[2].position", -1.0);
  (*param).peak[2].width        = (data_t)config_getdouble(ini, "peaks:[2].width", -1.0);

  (*param).peak[3].amplitude    = (data_t)config_getdouble(ini, "peaks:[3].amplitude", -1.0);
  (*param).peak[3].position     = (data_t)config_getdouble(ini, "peaks:[3].position", -1.0);
  (*param).peak[3].width        = (data_t)config_getdouble(ini, "peaks:[3].width", -1.0);

  /* Noise setup */
  (*param).noise.amplitude      = (data_t)config_getdouble(ini, "noise:amplitude", -1.0);
  (*param).noise.color          = (index_t)config_getint(ini, "noise:color", -1);

  /* Smooth setup */
  (*param).smooth.width         = (index_t)config_getint(ini, "smooth:width", -1);
  (*param).smooth.level         = (index_t)config_getint(ini, "smooth:level", -1);

  /* Generation setup */
  (*param).generation_max       = (index_t)config_getint(ini, "generation:number", -1.0);
  (*param).generation_frequency = (index_t)config_getint(ini, "generation:frequency", -1.0); // Hz

  /* Temperature setup */
  (*param).temp.apply           = (index_t)config_getint(ini, "temperature:apply", -1.0);
  (*param).temp.tick            = (index_t)config_getint(ini, "temperature:tick", -1.0);
  (*param).temp.room            = (data_t)config_getdouble(ini, "temperature:room", -1.0);
  (*param).temp.max             = (data_t)config_getdouble(ini, "temperature:max", -1.0);
  (*param).temp.coefficient     = (data_t)config_getdouble(ini, "temperature:coefficient", -1.0);

  /* Search setup */
  (*param).search.threshold_slope     = (data_t)config_getdouble(ini, "search:threshold_slope", -1.0);
  (*param).search.threshold_amp       = (data_t)config_getdouble(ini, "search:threshold_amp", -1.0);
  (*param).search.peaks_real_number   = (index_t)config_getint(ini, "search:peaks.num.real", -1.0);
  (*param).search.peaks_array_number  = (index_t)config_getint(ini, "search:peaks.num.arr", -1.0);
  (*param).search.peak_search_number  = (index_t)config_getint(ini, "search:peaks.num", -1.0);

  /* Plot setup */
  (*param).plot.x_min             = (data_t)config_getdouble(ini, "plot:x.min", -1.0);
  (*param).plot.x_max             = (data_t)config_getdouble(ini, "plot:x.max", -1.0);
  (*param).plot.y_min             = (data_t)config_getdouble(ini, "plot:y.min", -1.0);
  (*param).plot.y_max             = (data_t)config_getdouble(ini, "plot:y.max", -1.0);
  (*param).plot.show_signal       = (index_t)config_getint(ini, "plot:show.signal", -1.0);
  (*param).plot.show_smooth       = (index_t)config_getint(ini, "plot:show.signal.smooth", -1.0);
  (*param).plot.show_derivative   = (index_t)config_getint(ini, "plot:show.signal.derivative", -1.0);
  (*param).plot.show_markers      = 0; /* deprecated */
  (*param).plot.show_temperature  = (index_t)config_getint(ini, "plot:show.experiment.temperature", -1.0);
  (*param).plot.show_vs_smooth    = (index_t)config_getint(ini, "plot:show.experiment.smooth", -1.0);
  (*param).plot.show_vs_noise     = (index_t)config_getint(ini, "plot:show.experiment.noise", -1.0);

  return 0;
}

static void
config_parameters_set_default(void)
{
    FILE *ini ;

    if ((ini=fopen("config.ini", "w"))==NULL) {
        fprintf(stderr, "config: cannot create config.ini\n");
        return ;
    }

    fprintf(ini,
    "[Points]\n"
    "number          = 15000;        // total number of points\n"
    "\n"
    "[Peaks]\n"
    "number          = 4;            // number of peaks to generate\n"
    "timeshift       = 500;          // parameter for exp_gaussian function\n"
    "[0].amplitude   = 1.0;          // amplitude of peak 1 (a.u.)\n"
    "[0].position    = 1511.0;       // position of peak 1 (nm)\n"
    "[0].width       = 1.0;          // FWHM of peak 1 (nm)\n"
    "\n"
    "[1].amplitude   = 0.8;          // amplitude of peak 2 (a.u.)\n"
    "[1].position    = 1523.0;       // position of peak 2 (nm)\n"
    "[1].width       = 1.0;          // FWHM of peak 2 (nm)\n"
    "\n"
    "[2].amplitude   = 0.6;          // amplitude of peak 3 (a.u.)\n"
    "[2].position    = 1536.0;       // position of peak 3 (nm)\n"
    "[2].width       = 1.0;          // FWHM of peak 3 (nm)\n"
    "\n"
    "[3].amplitude   = 0.4;          // amplitude of peak 4 (a.u.)\n"
    "[3].position    = 1551.0;       // position of peak 4 (nm)\n"
    "[3].width       = 1.0;          // FWHM of peak 4 (nm)\n"
    "\n"
    "[Noise]\n"
    "amplitude       = 0.15;         // noise amplitude (a.u.)\n"
    "color           = 3;            // 0-random; 1-white; 2-brown; 3-violet; 4-pink (unfinished); 5-blue (unfinished)\n"
    "\n"
    "[Smooth]\n"
    "width           = 100;          // smooth width (points)\n"
    "level           = 3;            // number of smooth operations to apply\n"
    "\n"
    "[Search]\n"
    "peaks.num       = 1;            // peak of interest (sensor) number to show on graph\n"
    "threshold_slope = 0.000001;     // first derivative slope threshold\n"
    "threshold_amp   = 0.35;         // signal's peak amplitude threshold\n"
    "peaks.num.real  = 4;            // number of perak to search for\n"
    "peaks.num.arr   = 10;           // maximal array size for peak searching\n"
    "\n"
    "[Generation]\n"
    "number          = 100;          // number of signal generations (mesurements in experiment)\n"
    "frequency       = 5;            // frequency of mesurements\n"
    "\n"
    "[Temperature]\n"
    "apply           = 1;            // calculate temperature drift\n"
    "tick            = 20;           // number of mesurement to change temperature\n"
    "room            = 20;           // room temperature\n"
    "max             = 1350;         // maxumum temperature in experiment\n"
    "coefficient     = 0.02;         // coefficient [nm/C]\n"
    "\n"
    "[Plot]\n"
    "x.min           = 1500;         // x window limit (left)\n"
    "x.max           = 1600;         // x window limit (right)\n"
    "y.min           = 0.0;          // y window limit (bottom)\n"
    "y.max           = 1.4;          // y window limit (top)\n"
    "show.signal     = 1;             // show plot of real generated signal\n"
    "show.signal.smooth     = 0;      // show plot of smoothed generated signal\n"
    "show.signal.derivative = 0;      // show plot of fisrt derivative of generated signal\n"
    "show.experiment.temperature = 0; // show plot of temperature change on sensors\n"
    "show.experiment.smooth      = 0; // show plot of temperature vs smooth (1 - order, 2 - width)\n"
    "show.experiment.noise       = 1; // show plot of temperature vs noise (SNR)\n"
    "\n"
    );
    fclose(ini);
}

static void
data_clear(data_t * x, index_t size)
{
  for (index_t i = 0; i < size; i++) x[i] = 0.0;
}

static void
data_convert_to_lambda(data_t * lambda, data_t lambda_begin, data_t lambda_end, index_t size)
{
  // data_t lambda[size];
  index_t i;
  for (i = 0; i < size; i++)
  {
    lambda[i] = lambda_begin + ( i * ((lambda_end - lambda_begin)/size));
  }
}

static void
data_arrays_set(Signal_Parameters conf, Points *data, Points *data_temp,
                Points *temp_gen, Points *temp_sens_1, Points *temp_sens_2, Points *temp_sens_3, Points *temp_sens_4)
{
  (*data).x = (*data).y = NULL;
  (*data).x = (data_t *)MEM_malloc_arrayN(conf.n_points + 1, sizeof(data_t), "data_arrays_set: data.x");
  (*data).y = (data_t *)MEM_malloc_arrayN(conf.n_points + 1, sizeof(data_t), "data_arrays_set: data.y");
  data_convert_to_lambda((*data).x, conf.plot.x_min, conf.plot.x_max, conf.n_points);

  (*data_temp).x = (*data_temp).y = NULL;
  (*data_temp).x = (data_t *)MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: data_temp.x");
  (*data_temp).y = (data_t *)MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: data_temp.y");

  (*temp_gen).y = NULL;
  (*temp_gen).y = (data_t *)MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_gen.y");

  (*temp_sens_1).x = (*temp_sens_2).x = (*temp_sens_3).x = (*temp_sens_4).x = NULL;
  (*temp_sens_1).y = (*temp_sens_2).y = (*temp_sens_3).y = (*temp_sens_4).y = NULL;
  (*temp_sens_1).y = (data_t *)MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_sens_1.y");
  (*temp_sens_2).y = (data_t *)MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_sens_2.y");
  (*temp_sens_3).y = (data_t *)MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_sens_3.y");
  (*temp_sens_4).y = (data_t *)MEM_malloc_arrayN(conf.generation_max + 1, sizeof(data_t), "data_arrays_set: temp_sens_4.y");
  
  (*temp_gen).y[0] = (*temp_sens_1).y[0] = (*temp_sens_2).y[0] =(* temp_sens_3).y[0] = (*temp_sens_4).y[0] = conf.temp.room;
}

static void
graphics_set(gnuplot_ctrl *win[], Signal_Parameters conf)
{
  if (conf.plot.show_signal || conf.plot.show_smooth )
  {
    win[0] = gnuplot_init();
    gnuplot_cmd(win[0], "set term qt size 2000, 400");
    gnuplot_cmd(win[0], "set grid");
    gnuplot_cmd(win[0], "set xrange [%g:%g]", conf.plot.x_min, conf.plot.x_max);
    gnuplot_cmd(win[0], "set yrange [%g:%g]", conf.plot.y_min, conf.plot.y_max);
    gnuplot_cmd(win[0], "set label \"Frequency: %d Hz\" at 40,1.4", conf.generation_frequency);
    gnuplot_setstyle(win[0], "lines") ;
    gnuplot_set_xlabel(win[0], "Wavelength, nm");
    gnuplot_set_ylabel(win[0], "Amplitude, a.u.");
  }

  if (conf.plot.show_derivative)
  {
    win[1] = gnuplot_init();
    gnuplot_cmd(win[1], "set term qt size 1000, 400");
    gnuplot_cmd(win[1], "set grid");
    gnuplot_cmd(win[1], "set xrange [%g:%g]", conf.plot.x_min, conf.plot.x_max);
    gnuplot_cmd(win[1], "set yrange [%g:%g]", -0.015, 0.015);
  }

  if (conf.plot.show_temperature)
  {
    win[2] = gnuplot_init();
    gnuplot_cmd(win[2], "set term qt size 1000, 400");
    gnuplot_cmd(win[2], "set grid");
    gnuplot_cmd(win[2], "set xrange [%d:%d]", 0, conf.generation_max);
    gnuplot_cmd(win[2], "set yrange [%g:%g]", 0.0, conf.temp.max);
    gnuplot_setstyle(win[2], "lines") ;
    gnuplot_set_xlabel(win[2], "Interrogation, tick");
    gnuplot_set_ylabel(win[2], "Temperature, ^oC");
  }

  if (conf.plot.show_vs_smooth)
  {
    win[3] = gnuplot_init();
    gnuplot_cmd(win[3], "set term qt size 1000, 400");
    gnuplot_cmd(win[3], "set grid");
    gnuplot_setstyle(win[3], "points") ;
    if (conf.plot.show_vs_smooth == 1)
      gnuplot_set_xlabel(win[3], "Smooth order");
    else if (conf.plot.show_vs_smooth == 2)
      gnuplot_set_xlabel(win[3], "Smooth width");
    else if (conf.plot.show_vs_noise == 1)
      gnuplot_set_xlabel(win[3], "Smooth width");
    gnuplot_set_ylabel(win[3], "T_{sens} -- T_{gen}, ^oC");
  }

  if (conf.plot.show_vs_noise)
  {
    win[4] = gnuplot_init();
    gnuplot_cmd(win[4], "set term qt size 1000, 400");
    gnuplot_cmd(win[4], "set grid");
    gnuplot_setstyle(win[4], "points") ;
    gnuplot_set_xlabel(win[4], "Signal to noise ration (SNR)");
    gnuplot_set_ylabel(win[4], "T_{sens} -- T_{gen}, ^oC");
  }

  return;
}

static void
graphics_reset(gnuplot_ctrl *win[], Signal_Parameters conf)
{
  if ((conf.plot.show_signal || conf.plot.show_smooth)
                                 & (win[0] != NULL)) gnuplot_resetplot(win[0]);
  if (conf.plot.show_derivative  & (win[1] != NULL)) gnuplot_resetplot(win[1]);
  if (conf.plot.show_temperature & (win[2] != NULL)) gnuplot_resetplot(win[2]);
  if ((conf.plot.show_vs_smooth == 1 || conf.plot.show_vs_smooth == 2)
                                 & (win[3] != NULL)) gnuplot_resetplot(win[3]);
  if ((conf.plot.show_vs_noise == 1)
                                 & (win[4] != NULL)) gnuplot_resetplot(win[4]);
}

void
show_statistics(Signal_Parameters conf, Signal_Statistics stat, Points temp_gen, Points temp_sens_1, Points temp_sens_2, Points temp_sens_3, Points temp_sens_4)
{
  index_t i = 0;
  printf("\n");
  printf(BLUE("STATISTUS:")" Generation time:\t%f sec\n", stat.generation_time); // the last generation time
  printf(BLUE("STATISTUS:")" Max search freq:\t%f kHz\n", (1/stat.peak_search_time)/1000); // the last generation time
  printf(BLUE("STATISTUS:")" Number of drops:\t%d\n", stat.n_drops);
  if (conf.noise.amplitude > 0)
  {
    if (conf.noise.color == 1) printf(BLUE("STATISTUS:")" Noise color    :\t1 - white\n");
    if (conf.noise.color == 2) printf(BLUE("STATISTUS:")" Noise color    :\t2 - brown\n");
    if (conf.noise.color == 3) printf(BLUE("STATISTUS:")" Noise color    :\t3 - violet\n");
    if (conf.noise.color == 4) printf(BLUE("STATISTUS:")" Noise color    :\t4 - pink\n");
    if (conf.noise.color == 5) printf(BLUE("STATISTUS:")" Noise color    :\t5 - blue\n");
  }

  if(conf.temp.apply)
    printf(BLUE("STATISTUS:")" Rise of T[step]:\t%f C\n", stat.delta_temp);
  else
    printf(BLUE("STATISTUS:")" Rise of T[step]:\t- (no temperature rise)\n");
  printf(BLUE("STATISTUS:")" Sensor number #:\t%d\n", conf.search.peak_search_number);

  switch (conf.search.peak_search_number)
  {
    case 1:
      printf(BLUE("STATISTUS:")" End temperature:\t%f (generator: %f)\n", temp_sens_1.y[conf.generation_max], temp_gen.y[conf.generation_max]);
      break;
    case 2:
      printf(BLUE("STATISTUS:")" End temperature:\t%f (generator: %f)\n", temp_sens_2.y[conf.generation_max], temp_gen.y[conf.generation_max]);
      break;
    case 3:
      printf(BLUE("STATISTUS:")" End temperature:\t%f (generator: %f)\n", temp_sens_3.y[conf.generation_max], temp_gen.y[conf.generation_max]);
      break;
    case 4:
      printf(BLUE("STATISTUS:")" End temperature:\t%f (generator: %f)\n", temp_sens_4.y[conf.generation_max], temp_gen.y[conf.generation_max]);
      break;
  }

  data_t diff[conf.generation_max];
  switch (conf.search.peak_search_number)
  {
    case 1:
      for (i = 0; i <= conf.generation_max; i++) diff[i] = temp_sens_1.y[i] - temp_gen.y[i];
      break;
    case 2:
      for (i = 0; i <= conf.generation_max; i++) diff[i] = temp_sens_2.y[i] - temp_gen.y[i];
      break;
    case 3:
      for (i = 0; i <= conf.generation_max; i++) diff[i] = temp_sens_3.y[i] - temp_gen.y[i];
      break;
    case 4:
      for (i = 0; i <= conf.generation_max; i++) diff[i] = temp_sens_4.y[i] - temp_gen.y[i];
      break;
  }
  printf(BLUE("STATISTUS:")" Max temp deviation:\t[%f, %f]\n", max(diff, conf.generation_max + 1), min(diff, conf.generation_max + 1));
  printf(BLUE("STATISTUS:")" Min temp deviation:\t%f\n", min_abs(diff, conf.generation_max + 1));
}

static void
show_generator_info(Signal_Parameters conf, Signal_Statistics stat, index_t n_step)
{
  index_t i = 0;

  printf("\n" GREEN("GENERATOR:") " Temperature changed! New positions [ ");
  for (i = 0; i < conf.n_peaks; i++)
  {
    printf(CYAN("%f"), conf.peak[i].position);
    if (i != conf.n_peaks - 1) printf(", ");
  }
  printf(" ]");

  printf("\n" GREEN("GENERATOR:") " Temperature changed! The new value [ ");
  for (i = 0; i < conf.n_peaks; i++)
  {
    printf(CYAN("%f"), conf.temp.room + stat.delta_temp * n_step);
    if (i != conf.n_peaks - 1) printf(", ");
  }

  printf(" ]\n");
}

static void
show_psearch_info(Signal_Parameters conf, index_t i_gen, index_t i_sens, index_t n_peaks,
                  Points temp_sens_1, Points temp_sens_2, Points temp_sens_3, Points temp_sens_4)
{
  data_t value = 0.f;

  if (i_sens == 0) printf("[ ");
  /* Show value */
  // printf("%f", data.x[(index_t)peaks.peak[i].position]);

  /* Show difference */
  // data_t diff = data.x[peaks.peak[i].position] - data_ref.x[peaks_ref.peak[i].position];
  // printf((diff >= 0) ? " %f" : "%f", diff);

  /* Show temperature */
  switch (i_sens)
  {
    case 0:
      value = temp_sens_1.y[i_gen];
      break;
    case 1:
      value = temp_sens_2.y[i_gen];
      break;
    case 2:
      value = temp_sens_3.y[i_gen];
      break;
    case 3:
      value = temp_sens_4.y[i_gen];
      break;
    default:
      break;
  }

  // format output data string
  printf((i_sens != conf.search.peaks_real_number - 1) ? "%f, " : "%f", value);

  // show the note about found and real number of peaks
  if (i_sens == conf.search.peaks_real_number - 1)
  {
    if (n_peaks == conf.search.peaks_real_number)
    {
      printf(" ] -> "GREEN("[%d of %d]\n"), conf.search.peaks_real_number, n_peaks);
    }
    else if (n_peaks < conf.search.peaks_real_number)
    {
      printf(" ] -> "YELLOW("[%d of %d]\n"), conf.search.peaks_real_number, n_peaks);
    }
    else
    {
      printf(" ] -> "RED("[%d of %d]\n"), conf.search.peaks_real_number, n_peaks);
    }
  }
}

void
free_gnuplot(Signal_Parameters conf, gnuplot_ctrl **win)
{
    // Cleanup:
  if (conf.plot.show_signal || conf.plot.show_smooth )
    gnuplot_close(win[0]);
  if (conf.plot.show_derivative)
    gnuplot_close(win[1]);
  if (conf.plot.show_temperature)
    gnuplot_close(win[2]);
  if (conf.plot.show_vs_smooth)
    gnuplot_close(win[3]);
  if (conf.plot.show_vs_noise)
    gnuplot_close(win[4]);
}

static double
exp_broaden(const index_t size, double * in, double * out, const double t) {
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

static double
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

int
test_signal(const char * conf_name)
{
  index_t i = 0; // general iterator

  /* Parameters setup (config) */
  dictionary *ini = config_load(conf_name);
  if (ini == NULL)
  {
      fprintf(stderr, "cannot parse file: %s\n", conf_name);
      return -1;
  }

  Signal_Parameters conf;
  config_parameters_set(&conf, ini);

  /* Array for generated data */
  Points data, data_temp;
  Points temp_gen, temp_sens_1, temp_sens_2, temp_sens_3, temp_sens_4;
  data_arrays_set(conf, &data, &data_temp, &temp_gen, &temp_sens_1, &temp_sens_2, &temp_sens_3, &temp_sens_4);

  Signal_Statistics stat;

  /* Setup graphics */
  index_t max_number_of_plots = 5;
  gnuplot_ctrl *win[max_number_of_plots];
  for (i = 0; i < max_number_of_plots; i++)
  {
    win[i] = NULL;
  }
  graphics_set(win, conf);

  /* Generate signal and find peaks */
  index_t i_gen = 0; // generations iterator
  data_t *dy_dx = NULL;
  stat.n_drops = 0; // number of droped generations because of too many peaks (more than `peaks_real_number`)

  if (conf.plot.show_derivative)
  {
    dy_dx = MEM_malloc_arrayN(conf.n_points, sizeof(data_t), "test_signal: dy_dx"); // derivative
  }

  init_rnd();

  /* Generate main signal */
  Peaks peaks;
  peaks.peak = MEM_malloc_arrayN(conf.search.peaks_array_number, sizeof(Peak), "test_signal: peaks.peak array");

  if (conf.temp.apply)
  {
    stat.delta_temp = (conf.temp.max - conf.temp.room)/(conf.generation_max/conf.temp.tick);
  }

  /* Setup experiments */
  index_t smooth_level = 1;
  data_t x[conf.generation_max];
  data_t y[conf.generation_max];
  for (i = 0; i < conf.generation_max; i++)
  {
    x[i] = 0.0;
    y[i] = 0.0;
  }
  if (conf.plot.show_vs_smooth == 2)
  {
    conf.smooth.width = 25;
  }
  data_t noise_amplitude = 0.f, noise_step = 0.05;

  index_t n_step = 0;
  for (i_gen = 0; i_gen <= conf.generation_max; i_gen++)
  {
    /*
     * ======================================
     * Clear data from the previous iteration
     * ======================================
     */

    /* Clean plot for next generation */
    graphics_reset(win, conf);

    /* Clear data (set to zero) */
    data_clear(data.y, conf.n_points);

    /*
     * ======================================
     * Generate new signal
     * ======================================
     */

    /* Save previously generated signal peak positions
     * (ideal from generator and found by searcher)
     */
    data_t peak_position_ideal[conf.search.peaks_real_number];
    index_t peak_position_found[conf.search.peaks_real_number];
    for (i = 0; i < conf.search.peaks_real_number; i++)
    {
      peak_position_ideal[i] = conf.peak[i].position;
      peak_position_found[i] = peaks.peak[i].position;
    }

    /* ==================================================
     * ==== Experiment 1: simulate temperature change ===
     * ================================================== */
    /* Change peak positions according to the new temperature value */
    if ((conf.temp.apply) & (fmod(i_gen, conf.temp.tick) <= 1.0e-14))
    {
      for (i = 0; i < conf.n_peaks; i++)
      {
        conf.peak[i].position = peak_position_ideal[i] + (stat.delta_temp * conf.temp.coefficient);
      }
      show_generator_info(conf, stat, n_step);
      n_step = n_step + 1;
    }

    /* ==================================================
     * ==== Experiment 3: simulate noise level change ===
     * ================================================== */
    index_t noise_tick = 12;
    if (conf.plot.show_vs_noise == 1) // change noise amplitude
    {
      if (fmod(i_gen, noise_tick) <= 1.0e-14)
      {
        noise_amplitude += noise_step;
        printf(MAGENTA("PSEARCHER:")" Noise amplitude changed to %f\n", noise_amplitude);
      }
      conf.noise.amplitude = noise_amplitude;

      /* set x values */
      x[i_gen] = conf.noise.amplitude;

      /* set y values */
      for (i = 0; i < i_gen + 1; i++)
      {
        y[i] = temp_sens_1.y[i] - temp_gen.y[i];
      }

      gnuplot_cmd(win[4], "set xrange [%g:%g]", 0.0, 0.5);
      gnuplot_cmd(win[4], "set yrange [%g:%g]", -20.0, 20.0);
      gnuplot_plot_xy(win[4], x, y, i_gen + 1, "dT vs Noise amplitude");
    }
    else if ((i_gen > 0) & (conf.plot.show_vs_noise == 2)) // change noise color
    {
      /* TODO: code */
    }

    /* SIGNAL GENERATOR */
    stat.generation_time = signal_generate_exp(&data, conf.n_peaks, conf.peak, conf.noise, conf.n_points);
    if ((i_gen > 0) & conf.plot.show_signal & (win[0] != NULL))
    {
      gnuplot_plot_xy(win[0], data.x, data.y, conf.n_points, "Signal");
    }

    /*
     * ======================================
     * Find peaks
     * ======================================
     */

    /* ===================================================
     * ==== Experiment 2: evaluate smooth level change ===
     * =================================================== */
    /* Smooth signal */
    index_t smooth_tick = 15;
    if ((i_gen > 0) & (conf.plot.show_vs_smooth == 1)) // change smooth order
    {
      if (fmod(i_gen, smooth_tick) <= 1.0e-14)
      {
        ++smooth_level;
        printf(MAGENTA("PSEARCHER:")" Smooth order changed to %d\n", smooth_level);
      }
      for (i = 0; i < smooth_level; i++)
      {
        smooth(data.y, conf.n_points, conf.smooth.width);
      }

      /* set x values */
      x[i_gen] = smooth_level;

      /* set y values */
      for (i = 0; i < i_gen + 1; i++)
      {
        y[i] = temp_sens_1.y[i] - temp_gen.y[i] ;
      }

      gnuplot_cmd(win[3], "set xrange [%d:%d]", 0, 8);
      gnuplot_cmd(win[3], "set yrange [%g:%g]", -20.0, 20.0);
      gnuplot_plot_xy(win[3], x, y, i_gen + 1, "dT vs Smooth order");
    }
    else if ((i_gen > 0) & (conf.plot.show_vs_smooth == 2)) // change smooth width
    {
      if (fmod(i_gen, smooth_tick) <= 1.0e-14)
      {
        conf.smooth.width += 25;
        printf(MAGENTA("PSEARCHER:")" Smooth width changed to %d\n", conf.smooth.width);
      }
      for (i = 0; i < conf.smooth.level; i++)
      {
        smooth(data.y, conf.n_points, conf.smooth.width);
      }

      /* set x values */
      x[i_gen] = conf.smooth.width;

      /* set y values */
      for (i = 0; i < i_gen + 1; i++)
      {
        y[i] = temp_sens_1.y[i] - temp_gen.y[i];
      }

      gnuplot_cmd(win[3], "set xrange [%d:%d]", 0, 200);
      gnuplot_cmd(win[3], "set yrange [%g:%g]", -20.0, 40.0);
      gnuplot_plot_xy(win[3], x, y, i_gen + 1, "dT vs Smooth width");
    }
    else
    {
      for (i = 0; i < conf.smooth.level; i++)
      {
        smooth(data.y, conf.n_points, conf.smooth.width);
      }
    }

    /* Show plot of smoothed signal */
    if (conf.plot.show_smooth & (win[0] != NULL))
    {
      gnuplot_plot_xy(win[0], data.x, data.y, conf.n_points, "Signal (smooth)");
    }

    /* Find and show derivative */
    if (conf.plot.show_derivative & (win[1] != NULL))
    {
      data_clear(dy_dx, conf.n_points);
      deriv(conf.n_points, data.y, dy_dx);
      gnuplot_plot_xy(win[1], data.x, dy_dx, conf.n_points, "dy/dLambda");
    }

    /* Find peaks */
    peaks.peak = MEM_reallocN(peaks.peak, sizeof(Peak) * conf.search.peaks_array_number);
    stat.peak_search_time = findpeaks(data.y, &peaks, &conf);

    /*
     * =======================================
     * Get temperature values and show results
     * =======================================
     */

    if (i_gen > 0)
    {
      temp_gen.y[i_gen] = temp_gen.y[i_gen - 1] + (conf.peak[0].position - peak_position_ideal[0])/conf.temp.coefficient;
      // if (peaks.total_number >= conf.search.peaks_real_number)
      // {
        temp_sens_1.y[i_gen] = temp_sens_1.y[i_gen - 1] + (data.x[(index_t)peaks.peak[0].position] - data.x[peak_position_found[0]])/conf.temp.coefficient;
        temp_sens_2.y[i_gen] = temp_sens_2.y[i_gen - 1] + (data.x[(index_t)peaks.peak[1].position] - data.x[peak_position_found[1]])/conf.temp.coefficient;
        temp_sens_3.y[i_gen] = temp_sens_3.y[i_gen - 1] + (data.x[(index_t)peaks.peak[2].position] - data.x[peak_position_found[2]])/conf.temp.coefficient;
        temp_sens_4.y[i_gen] = temp_sens_4.y[i_gen - 1] + (data.x[(index_t)peaks.peak[3].position] - data.x[peak_position_found[3]])/conf.temp.coefficient;

        printf(MAGENTA("PSEARCHER:")" Found %d peak(s) in %f sec at ", peaks.total_number, stat.peak_search_time);
        for (index_t i_sens = 0; i_sens < conf.search.peaks_real_number; i_sens++)
        {
          show_psearch_info(conf, i_gen, i_sens, peaks.total_number, temp_sens_1, temp_sens_2, temp_sens_3, temp_sens_4);

          // Plot markers in peak positions
          if ((conf.plot.show_signal || conf.plot.show_smooth || conf.plot.show_derivative) & conf.plot.show_markers & (win[0] != NULL))
          {
            data_t marker_x[2] = { data.x[(index_t)peaks.peak[i_sens].position], data.x[(index_t)peaks.peak[i_sens].position] };
            data_t marker_y[2] = { conf.plot.y_min, conf.plot.y_max };
            gnuplot_plot_xy(win[0], marker_x, marker_y, 2, "Peak marker");
          }
        }
      // }
      // else
      // {
      //   temp_sens_1.y[i_gen] = temp_sens_1.y[i_gen - 1];
      //   temp_sens_2.y[i_gen] = temp_sens_2.y[i_gen - 1];
      //   temp_sens_3.y[i_gen] = temp_sens_3.y[i_gen - 1];
      //   temp_sens_4.y[i_gen] = temp_sens_4.y[i_gen - 1];

      //   printf(MAGENTA("PSEARCHER:")" Wrong number of peaks. Found %d, should be %d. "RED("Drop!")"\n", peaks.total_number, conf.search.peaks_real_number);
      //   ++stat.n_drops;
      // }
    }

    /* Temperature */
    if (conf.temp.apply & conf.plot.show_temperature)
    {
      gnuplot_cmd(win[2], "set yrange [%d:%g]", 0, (conf.temp.max + 700));
      data_temp.x[i_gen] = i_gen;
      switch (conf.search.peak_search_number)
      {
        case 1:
          gnuplot_plot_xy(win[2], data_temp.x, temp_sens_1.y, i_gen + 1, "T (sensor 1)");
          break;
        case 2:
          gnuplot_plot_xy(win[2], data_temp.x, temp_sens_2.y, i_gen + 1, "T (sensor 2)");
          break;
        case 3:
          gnuplot_plot_xy(win[2], data_temp.x, temp_sens_3.y, i_gen + 1, "T (sensor 3)");
          break;
        case 4:
          gnuplot_plot_xy(win[2], data_temp.x, temp_sens_4.y, i_gen + 1, "T (sensor 4)");
          break;
      }
      gnuplot_plot_xy(win[2], data_temp.x, temp_gen.y, i_gen + 1, "T (generator)");
    }
    /* Isolate desired segment for curve fitting */
    // index_t points_segment_size = 100; // number of points
    // data_t points_segment_center = 300.0; // data center value
    // Points data_segment;
    // data_segment.x = (data_t *)malloc(sizeof(data_t) * points_segment_size);
    // data_segment.y = (data_t *)malloc(sizeof(data_t) * points_segment_size);
    // // data_window_get(points, n_points, 200, points_segment_size, segment);
    // index_t n1 = val2ind(data.x, conf.n_points, points_segment_center - (points_segment_size / 2));
    // // index_t n2 = val2ind(data.x, n_points, points_segment_center + (points_segment_size / 2));
    // for (index_t ii = 0; ii < points_segment_size; ii++)
    // {
    //   data_segment.x[ii] = data.x[n1];
    //   data_segment.y[ii] = data.y[n1];
    //   n1 = n1 + 1;
    // }
    // gnuplot_plot_points(win, &data_segment, points_segment_size, "Segment");

    /* Simulate mesurements frequency */
    if (conf.generation_max != 1)
    {
      usleep(1000000/conf.generation_frequency);
    }
  }

  /* Statistics */
  show_statistics(conf, stat, temp_gen, temp_sens_1, temp_sens_2, temp_sens_3, temp_sens_4);

  /* Do not close window untill we press any key */
  if (conf.plot.show_signal || conf.plot.show_smooth || conf.plot.show_derivative || conf.plot.show_temperature)
  {
    printf("\nPress ENTER to continue\n"); while (getchar()!='\n'){}
  }

  /*
   * ======================================
   * Clear data
   * ======================================
   */

  MEM_freeN(conf.peak);

  MEM_freeN(data.x);
  MEM_freeN(data.y);

  MEM_freeN(data_temp.x);
  MEM_freeN(data_temp.y);
  
  MEM_freeN(temp_gen.y);
  MEM_freeN(temp_sens_1.y);
  MEM_freeN(temp_sens_2.y);
  MEM_freeN(temp_sens_3.y);
  MEM_freeN(temp_sens_4.y);

  if (conf.plot.show_derivative)
  {
    MEM_freeN(dy_dx);
  }

  MEM_freeN(peaks.peak);

  config_freedict(ini);
  free_gnuplot(conf, win);

  return 0;
}

int
main(int argc ENSEN_UNUSED, const char ** argv ENSEN_UNUSED)
{
# ifdef MEM_DEBUG_APPLY
  MEM_use_guarded_allocator(); /* SLOW, use only for debugging */
# endif
  MEM_init_memleak_detection();
  MEM_enable_fail_on_memleak();

  clearscreen();
  const char * conf_file_name = "config.ini";
  if (fopen(conf_file_name, "r") == NULL) {
    config_parameters_set_default();
    printf("Cannot find configuration file: %s. Created the new one!\n", conf_file_name);
  }
  test_signal("config.ini");

# ifdef MEM_DEBUG_APPLY
  printf("Used %ld kB of memory \n", MEM_get_peak_memory()/1024);
# endif

  return 0;
}
