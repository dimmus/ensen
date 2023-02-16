#include <unistd.h> // usleep()
#include <stdlib.h> // clearscr()
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <complex.h>

#include <fftw3.h>

extern int usleep (__useconds_t __useconds); // from <unistd.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef ENABLE_NLS
# include <libintl.h>
#endif

#ifdef UI_GNUPLOT
  #include "ui_gnuplot.h"
#endif

#ifdef UI_NUKLEAR
  #define NK_IMPLEMENTATION
  #include "ui_nuklear.h"
#endif

#ifdef UI_SDL
  #include "ui_sdl_llist.h"
  #include "ui_sdl_plot.h"
#endif

#include "ensen_private.h"

#include "ensen_signal.h"
#include "ensen_math.h"
#include "ensen_config.h"
#include "ensen_config_dictionary.h"

#include <gsl/gsl_statistics.h>
#include <gsl/gsl_fit.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft.h>
#include <gsl/gsl_fft_complex.h>

#include "fftw_utils.h"

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])
#define PLOT(data, from, to, title)       \
  for (index_t ii = from; ii <= to; ii++) \
  {                                       \
    data_points.x[ii] = ii;               \
    data_points.y[ii] = data;             \
  }                                       \
  gnuplot_plot_points(win, &data_points, to, title); \

// static void clearscreen(void);
// static int parameters_set(Signal_Parameters * param, dictionary *ini);
// static void create_default_config_file(void);
// static void clear_data(data_t * x, index_t size);
// static void convert_to_lambda(data_t * lambda, data_t lambda_begin, data_t lambda_end, index_t size);
// static void calibrate(Points *data, Peaks * peaks_ref, Signal_Parameters * conf, index_t show);
int test_signal(const char * conf_name);

double exp_gaussian(const index_t size, double * in, double * out, double pos, double wid, double timeconstant);

/// @brief Zero pads input and convolutes result by an exponential decay
/// of time constant "t" by multiplying Fourier transforms and inverse
/// transforming the result.
/// @param size Size of input
/// @param input Input data array 
/// @param output Output data array
/// @param t Time constant
/// @return Time of exacution (in sec.)
double exp_broaden(const index_t size, double * input, double * output, const double t);

// tmp (was for testing) - DELETE
int test_signal_peaksearch(void);
double test_gsl_fft(void);
int test_gsl_fft_small(void);
int test_signal_random(void);
int peakfit_WIP(void);

static void
clearscreen(void)
{
  // system("cls||clear"); // win and linux
  printf("\e[1;1H\e[2J");
}

static int
parameters_set(Signal_Parameters * param, dictionary *ini)
{
  // config_dump(ini, stderr);
  
  /* Peaks setup */
  (*param).n_points = (index_t)config_getint(ini, "points:number", -1);
  (*param).n_peaks = (index_t)config_getint(ini, "peaks:number", -1);

  (*param).peak = malloc(sizeof(Peak) * 4);

  (*param).peak[0].amplitude = (data_t)config_getdouble(ini, "peaks:[0].amplitude", -1.0);
  (*param).peak[0].position = (data_t)config_getdouble(ini, "peaks:[0].position", -1.0);
  (*param).peak[0].width = (data_t)config_getdouble(ini, "peaks:[0].width", -1.0);
  
  (*param).peak[1].amplitude = (data_t)config_getdouble(ini, "peaks:[1].amplitude", -1.0);
  (*param).peak[1].position = (data_t)config_getdouble(ini, "peaks:[1].position", -1.0);
  (*param).peak[1].width = (data_t)config_getdouble(ini, "peaks:[1].width", -1.0);
  
  (*param).peak[2].amplitude = (data_t)config_getdouble(ini, "peaks:[2].amplitude", -1.0);
  (*param).peak[2].position = (data_t)config_getdouble(ini, "peaks:[2].position", -1.0);
  (*param).peak[2].width = (data_t)config_getdouble(ini, "peaks:[2].width", -1.0);

  (*param).peak[3].amplitude = (data_t)config_getdouble(ini, "peaks:[3].amplitude", -1.0);
  (*param).peak[3].position = (data_t)config_getdouble(ini, "peaks:[3].position", -1.0);
  (*param).peak[3].width = (data_t)config_getdouble(ini, "peaks:[3].width", -1.0);

  /* Noise setup */
  (*param).noise.amplitude = (data_t)config_getdouble(ini, "noise:amplitude", -1.0);

  /* Smooth setup */
  (*param).smooth_width = (int)config_getint(ini, "smooth:width", -1);

  /* Generation setup */
  (*param).generation_max = (index_t)config_getint(ini, "generation:number", -1.0);
  (*param).generation_frequency = (index_t)config_getint(ini, "generation:frequency", -1.0); // Hz

  /* Temperature setup */
  (*param).temp.tick = (index_t)config_getint(ini, "temperature:tick", -1.0);
  (*param).temp.room = (data_t)config_getdouble(ini, "temperature:room", -1.0);
  (*param).temp.max = (data_t)config_getdouble(ini, "temperature:max", -1.0);

  /* Search setup */
  (*param).search.threshold_slope = (data_t)config_getdouble(ini, "search:threshold_slope", -1.0);
  (*param).search.threshold_amp = (data_t)config_getdouble(ini, "search:threshold_amp", -1.0);
  (*param).search.peaks_real_number = (index_t)config_getint(ini, "search:peaks.num.real", -1.0);
  (*param).search.peaks_array_number = (index_t)config_getint(ini, "search:peaks.num.arr", -1.0);

  /* Plot setup */
  (*param).plot.x_min = (data_t)config_getdouble(ini, "plot:x.min", -1.0);
  (*param).plot.x_max = (data_t)config_getdouble(ini, "plot:x.max", -1.0);
  (*param).plot.y_min = (data_t)config_getdouble(ini, "plot:y.min", -1.0);
  (*param).plot.y_max = (data_t)config_getdouble(ini, "plot:y.max", -1.0);
  (*param).plot.show_signal = (index_t)config_getint(ini, "plot:show.signal", -1.0);
  (*param).plot.show_smooth = (index_t)config_getint(ini, "plot:show.smooth", -1.0);
  (*param).plot.show_derivative = (index_t)config_getint(ini, "plot:show.derivative", -1.0);
  (*param).plot.show_markers = (index_t)config_getint(ini, "plot:show.markers", -1.0);
  (*param).plot.show_temperature = (index_t)config_getint(ini, "plot:show.temp", -1.0);

  return 0;
}

static void 
create_default_config_file(void)
{
    FILE *ini ;

    if ((ini=fopen("config.ini", "w"))==NULL) {
        fprintf(stderr, "config: cannot create config.ini\n");
        return ;
    }

    fprintf(ini,
    "[Points]\n"
    "number          = 1000;\n"
    "\n"
    "[Peaks]\n"
    "number          = 4;\n"
    "[0].amplitude   = 1.0;\n"
    "[0].position    = 1520.0;\n"
    "[0].width       = 10.0;\n"
    "\n"
    "[1].amplitude   = 0.8;\n"
    "[1].position    = 1540.0;\n"
    "[1].width       = 10.0;\n"
    "\n"
    "[2].amplitude   = 0.6;\n"
    "[2].position    = 1560.0;\n"
    "[2].width       = 10.0;\n"
    "\n"
    "[3].amplitude   = 0.4;\n"
    "[3].position    = 1580.0;\n"
    "[3].width       = 10.0;\n"
    "\n"
    "[Noise]\n"
    "amplitude       = 0.2;\n"
    "\n"
    "[Smooth]\n"
    "width           = 50;\n"
    "\n"
    "[Search]\n"
    "threshold_slope = 0.0001;\n"
    "threshold_amp   = 0.32;\n"
    "\n"
    "[Generation]\n"
    "number          = 10;\n"
    "frequency       = 5;\n"
    "\n"
    "[Plot]\n"
    "x.min           = 0.0;\n"
    "x.max           = 1000;\n"
    "y.min           = 0.0;\n"
    "y.max           = 1.6;\n"
    "show.signal     = 1;\n"
    "show.smooth     = 1;\n"
    "show.derivative = 0;\n"
    "show.markers    = 1;\n"
    "\n"
    );
    fclose(ini);
}

static void
clear_data(data_t * x, index_t size)
{
  for (index_t i = 0; i < size; i++) x[i] = 0.0;
}

static void
convert_to_lambda(data_t * lambda, data_t lambda_begin, data_t lambda_end, index_t size)
{
  // data_t lambda[size]; 
  index_t i; 
  for (i = 0; i < size; i++)
  {
    lambda[i] = lambda_begin + ( i * ((lambda_end - lambda_begin)/size)); 
  }
}

/* static void
calibrate(Points * data, Peaks * peaks_ref, Signal_Parameters * conf, index_t show)
{
  Noise ns;
  ns.amplitude = 0.0; // set noise to zero when search reference peaks
  
  signal_generate(data, (*conf).n_peaks, (*conf).peak, ns, (*conf).n_points);
  
  (*peaks_ref).peak = malloc(sizeof(Peak) * (*conf).search.peaks_array_number);
  findpeaks((*data).y, peaks_ref, conf);
  if (show)
  {
    index_t i = 0;
    for (i = 0; i < (*conf).search.peaks_real_number; i++)
    {
      if (i == 0) printf("Reference peak values are [ ");      
      printf("%f", (*data).x[(*peaks_ref).peak[i].position]);
      if (i != (*conf).search.peaks_real_number - 1) printf(", "); 
      if (i == (*conf).search.peaks_real_number - 1) printf(" ]\n");
    }
  }

  return;
}
 */
static void
graphics_set(gnuplot_ctrl *win[], Signal_Parameters * conf)
{
  if ((*conf).plot.show_signal || (*conf).plot.show_smooth )
  {
    win[0] = gnuplot_init();
    gnuplot_cmd(win[0], "set term qt size 2000, 400");
    gnuplot_cmd(win[0], "set grid");
    gnuplot_cmd(win[0], "set xrange [%g:%g]", (*conf).plot.x_min, (*conf).plot.x_max);
    gnuplot_cmd(win[0], "set yrange [%g:%g]", (*conf).plot.y_min, (*conf).plot.y_max);
    gnuplot_cmd(win[0], "set label \"Frequency: %d Hz\" at 40,1.4", (*conf).generation_frequency);
    gnuplot_setstyle(win[0], "lines") ;
    gnuplot_set_xlabel(win[0], "Wavelength, nm");
    gnuplot_set_ylabel(win[0], "Amplitude, a.u.");
  }

  if ((*conf).plot.show_derivative)
  {
    win[1] = gnuplot_init();
    gnuplot_cmd(win[1], "set term qt size 1000, 400");
    gnuplot_cmd(win[1], "set grid");
    gnuplot_cmd(win[1], "set xrange [%g:%g]", (*conf).plot.x_min, (*conf).plot.x_max);
    gnuplot_cmd(win[1], "set yrange [%g:%g]", -0.015, 0.015);
  }

  if ((*conf).plot.show_temperature)
  {
    win[2] = gnuplot_init();
    gnuplot_cmd(win[2], "set term qt size 1000, 400");
    gnuplot_cmd(win[2], "set grid");
    gnuplot_cmd(win[2], "set xrange [%d:%d]", 0, (*conf).generation_max);
    gnuplot_cmd(win[2], "set yrange [%g:%g]", 0.0, (*conf).temp.max);
    gnuplot_setstyle(win[2], "lines") ;
    gnuplot_set_xlabel(win[2], "Generation, tick");
    gnuplot_set_ylabel(win[2], "Temperature, C");
  }

  return;
}

int 
test_signal(const char * conf_name)
{
  /* Parameters setup (config) */
  dictionary *ini = config_load(conf_name);
  if (ini == NULL)
  {
      fprintf(stderr, "cannot parse file: %s\n", conf_name);
      return -1;
  }

  Signal_Parameters conf;
  parameters_set(&conf, ini);

  /* Array for generated data */
  Points data, data_temp; // data_ref
  data_temp.x = data_temp.y = NULL;
  data.x = (data_t *)malloc(sizeof(data_t) * conf.n_points);
  data.y = (data_t *)malloc(sizeof(data_t) * conf.n_points);
  convert_to_lambda(data.x, conf.plot.x_min, conf.plot.x_max, conf.n_points);

  // data_ref.x = data.x;
  // data_ref.y = (data_t *)malloc(sizeof(data_t) * conf.n_points);
  
  if (conf.plot.show_temperature)
  {
    data_temp.x = (data_t *)malloc(sizeof(data_t) * conf.generation_max);
    data_temp.y = (data_t *)malloc(sizeof(data_t) * conf.generation_max);
  }

  Points data_temp1, data_temp2, data_temp3, data_temp4;
  data_temp1.x = data_temp2.x = data_temp3.x = data_temp4.x = NULL;
  data_temp1.y = data_temp2.y = data_temp3.y = data_temp4.y = NULL;
  if (conf.plot.show_temperature)
  {
    data_temp1.y = (data_t *)malloc(sizeof(data_t) * conf.generation_max);
    data_temp2.y = (data_t *)malloc(sizeof(data_t) * conf.generation_max);
    data_temp3.y = (data_t *)malloc(sizeof(data_t) * conf.generation_max);
    data_temp4.y = (data_t *)malloc(sizeof(data_t) * conf.generation_max);
  }

  /* Setup graphics */
  gnuplot_ctrl *win[3];
  win[0] = win[1] = win[2] = NULL;
  graphics_set(win, &conf);

  /* Generate signal and find peaks */
  index_t i_gen = 0; // generations iterator
  index_t i     = 0; // general iterator
  data_t *dy_dx = malloc(sizeof(data_t) * conf.n_points); // derivative
  index_t n_drops = 0; // number of droped generations becouse of to many peaks (more than `peaks_real_number`)

  random_seed();
  
  /* Generate reference (calibration) signal */
  // Peaks peaks_ref;
  // peaks_ref.peak = malloc(sizeof(Peak) * conf.search.peaks_array_number);
  // calibrate(&data_ref, &peaks_ref, &conf, 1);

  /* Generate main signal */
  Peaks peaks;
  peaks.peak = malloc(sizeof(Peak) * conf.search.peaks_array_number);
  data_t time = 0.0;
  // index_t step_n = 0;
  
  index_t apply_temp = 1;
  data_t delta_temp;
  if (apply_temp) delta_temp = (conf.temp.max - conf.temp.room)/(conf.generation_max/conf.temp.tick);
  
  for (i_gen = 0; i_gen <= conf.generation_max; i_gen++)
  {
    /* Clean plot for next generation */
    if ((conf.plot.show_signal || conf.plot.show_smooth) & (win[0] != NULL)) gnuplot_resetplot(win[0]);
    if (conf.plot.show_derivative & (win[1] != NULL)) gnuplot_resetplot(win[1]);
    if (conf.plot.show_temperature & (win[2] != NULL)) gnuplot_resetplot(win[2]);

    /* Clear data (set to zero) */
    clear_data(data.y, conf.n_points);

    /* Generate signal */   
    data_t pos_ideal[conf.search.peaks_real_number];
    for (i = 0; i < conf.search.peaks_real_number; i++) pos_ideal[i] = conf.peak[i].position;
    if ((apply_temp) & (fmod(i_gen + 1, conf.temp.tick) == 0))
    {
      printf("\n" GREEN("GENERATOR:") " Temperature changed! New positions [ ");
      for (i = 0; i < conf.n_peaks; i++)
      {
        conf.peak[i].position = conf.peak[i].position + (delta_temp * 0.02);
        printf(CYAN("%f"), conf.peak[i].position);
        if (i != conf.n_peaks - 1) printf(", ");
      }
      printf(" ]\n");
    }

    /* Save peaks data from previous generation to calculate temperature change */
    index_t pos[conf.search.peaks_real_number];
    for (i = 0; i < conf.search.peaks_real_number; i++) pos[i] = peaks.peak[i].position;
    
    signal_generate(&data, conf.n_peaks, conf.peak, conf.noise, conf.n_points);
    if (conf.plot.show_signal & (win[0] != NULL)) gnuplot_plot_xy(win[0], data.x, data.y, conf.n_points, "Signal");  
    
    /* Smooth signal */
    smooth(data.y, conf.n_points, conf.smooth_width);
    smooth(data.y, conf.n_points, conf.smooth_width);
    smooth(data.y, conf.n_points, conf.smooth_width);
    // smooth(data.y, conf.n_points, conf.smooth_width);
    // smooth(data.y, conf.n_points, conf.smooth_width);
    if (conf.plot.show_smooth & (win[0] != NULL)) gnuplot_plot_xy(win[0], data.x, data.y, conf.n_points, "Signal (smooth)");

    /* Find and show derivative */
    if (conf.plot.show_derivative & (win[1] != NULL))
    {
      clear_data(dy_dx, conf.n_points);
      deriv(conf.n_points, data.y, dy_dx);
      gnuplot_plot_xy(win[1], data.x, dy_dx, conf.n_points, "dy/dLambda");
    }

    /* Find peaks */
    peaks.peak = realloc(peaks.peak, sizeof(Peak) * conf.search.peaks_array_number);
    time = findpeaks(data.y, &peaks, &conf);
    if (peaks.total_number == conf.search.peaks_real_number)
    {
      printf(MAGENTA("PSEARCHER:")" Found %d peak(s) in %f sec at ", peaks.total_number, time);
      for (i = 0; i < peaks.total_number; i++)
      {
        if (i == 0) printf("[ ");      
        /* Show value */
        printf("%f", data.x[(index_t)peaks.peak[i].position]);
        /* Show difference */
        // data_t diff = data.x[peaks.peak[i].position] - data_ref.x[peaks_ref.peak[i].position];
        // printf((diff >= 0) ? " %f" : "%f", diff);
        /* Show temperature */
        // data_t diff = (data.x[peaks.peak[i].position] - data_ref.x[peaks_ref.peak[i].position])/0.02;
        // printf((diff >= 0) ? " %f" : "%f", diff);
        
        if (i != peaks.total_number - 1) printf(", "); 
        if ((i == peaks.total_number - 1) & (conf.search.peaks_real_number == peaks.total_number)) printf(" ]\n");
        
        // Plot markers in peak positions
        if ((conf.plot.show_signal || conf.plot.show_smooth || conf.plot.show_derivative) & conf.plot.show_markers & (win[0] != NULL))
        {
          data_t marker_x[2] = { data.x[(index_t)peaks.peak[i].position], data.x[(index_t)peaks.peak[i].position] };
          data_t marker_y[2] = { conf.plot.y_min, conf.plot.y_max };
          gnuplot_plot_xy(win[0], marker_x, marker_y, 2, "Peak marker");
        }
      }
    }
    else
    {
      printf(MAGENTA("PSEARCHER:")" Wrong number of peaks. Found %d, should be %d. "RED("Drop!")"\n", peaks.total_number, conf.search.peaks_real_number);
      ++n_drops;
    }

    /* Temperature */
    if ((conf.plot.show_temperature) & (i_gen > 0))
    {
      gnuplot_cmd(win[2], "set yrange [%d:%g]", 0, (conf.temp.max + 700));
      data_temp.x[i_gen] = i_gen;
      // for (i = 0; i < peaks.total_number; i++)
      // {
      //   data_temp.y[i_gen] = ((100 * (i + 1)) + (100 * i)) + (data.x[peaks.peak[i].position] - data_ref.x[peaks_ref.peak[i].position])/0.02;
      //   gnuplot_plot_xy(win[2], data_temp.x, data_temp.y, i_gen + 1, "Temperature deviation");
      // }   

      data_temp1.y[i_gen] = data_temp1.y[i_gen - 1] + (data.x[(index_t)peaks.peak[0].position] - data.x[pos[0]])/0.02;
      data_temp2.y[i_gen] = data_temp2.y[i_gen - 1] + (conf.peak[0].position - pos_ideal[0])/0.02;
      // data_temp2.y[i_gen] = data_temp1.y[i_gen - 1] + (data.x[peaks.peak[1].position] - data.x[pos[1]])/0.02;
      // data_temp3.y[i_gen] = data_temp1.y[i_gen - 1] + (data.x[peaks.peak[2].position] - data.x[pos[2]])/0.02;
      // data_temp4.y[i_gen] = data_temp1.y[i_gen - 1] + (data.x[peaks.peak[3].position] - data.x[pos[3]])/0.02;
      gnuplot_plot_xy(win[2], data_temp.x, data_temp1.y, i_gen + 1, "T1");
      gnuplot_plot_xy(win[2], data_temp.x, data_temp2.y, i_gen + 1, "T generator");
      // gnuplot_plot_xy(win[2], data_temp.x, data_temp3.y, i_gen + 1, "T3 deviation");
      // gnuplot_plot_xy(win[2], data_temp.x, data_temp4.y, i_gen + 1, "T4 deviation");
      // printf("GRAPHICUS: Position: old = %f vs new = %f\n", data.x[pos[0]], data.x[(index_t)peaks.peak[0].position]);
    } 
    else if (i_gen == 0)
    {
      data_temp.x[0] = 0;
      data_temp1.y[0] = data_temp2.y[0] = data_temp3.y[0] = data_temp4.y[0] = conf.temp.room;
      gnuplot_plot_xy(win[2], data_temp.x, data_temp1.y, i_gen + 1, "T1");
      gnuplot_plot_xy(win[2], data_temp.x, data_temp2.y, i_gen + 1, "T generator");
      // gnuplot_plot_xy(win[2], data_temp.x, data_temp3.y, i_gen + 1, "T3 deviation");
      // gnuplot_plot_xy(win[2], data_temp.x, data_temp4.y, i_gen + 1, "T4 deviation");
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

    /* Works only without smooth. VERY unstable results. */
    /* PeakSearch ps; */
    // ps.sigmas = (data_t)config_getdouble(ini, "search:sigma", -1.0); // 0.15
    // ps.win_size = (index_t)config_getdouble(ini, "search:window", -1.0); // 40
    // ps.data_s = conf.n_points; //sizeof(d) / sizeof(data_t);
    // ps.data_v = (data_t*) malloc(ps.data_s * sizeof(data_t));
    // index_t *peaks_arr = (index_t*) malloc(sizeof(index_t));
    // search_data_peaks(&ps, &peaks_arr, data.y, n_points);
    
    /* Simulate mesurements frequency */
    if (conf.generation_max != 1) usleep(1000000/conf.generation_frequency); 
  }

  printf("\n");
  printf(BLUE("STATISTUS:")" Rise of T[step]: %f\n", delta_temp); 
  printf(BLUE("STATISTUS:")" Number of drops: %d\n", n_drops);
  printf(BLUE("STATISTUS:")" Max temperature: %f\n", data_temp1.y[conf.generation_max]);

  /* Do not close window untill we press any key */
  if (conf.plot.show_signal || conf.plot.show_smooth || conf.plot.show_derivative || conf.plot.show_temperature)
  {
    printf("\n");
    printf("Press ENTER to continue\n"); while (getchar()!='\n'){}
  }

  // Cleanup:
  if (conf.plot.show_signal || conf.plot.show_smooth ) gnuplot_close(win[0]);
  if (conf.plot.show_derivative) gnuplot_close(win[1]);
  if (conf.plot.show_temperature)
  {
    gnuplot_close(win[2]);
    // free(data_temp.x); free(data_temp.y);
  }
  free(conf.peak);
  free(data.x); free(data.y);
  // free(data_ref.x); free(data_ref.y);
  config_freedict(ini);

  return 0;
}

double 
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
  for (index_t p = (size/2+2); p <= (n-(size/2)+1); p++)
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
  double start_time = get_run_time();
  // Exponentially-convoluted gaussian(x,pos,wid) = gaussian peak centered on pos, half-width=wid
  // x may be scalar, vector, or matrix, pos and wid both scalar
  double yy[size];
  double arg = 0.0;
  for (index_t i = 0; i < size; i++)
  {
    arg = (in[i] - pos)/(0.60056120439323 * wid);
    yy[i] = exp(-(arg * arg)); 
  }
  exp_broaden(size, yy, out, timeconstant);

  double end_time = get_run_time();
  return end_time - start_time;
}

int
main(int argc __UNUSED__, const char ** argv __UNUSED__)
{
  clearscreen();
  const char * conf_file_name = "config.ini";
  if (fopen(conf_file_name, "r") == NULL) {
    create_default_config_file();
    printf("Cannot find configuration file: %s. Created the new one!\n", conf_file_name);
  } 
  test_signal("config.ini"); 

  /*
  index_t size = 100;

  Points data_points;
  data_points.x = malloc(sizeof(data_t) * size);
  data_points.y = malloc(sizeof(data_t) * size);
  
  // Plot
  gnuplot_ctrl *win = gnuplot_init();
  gnuplot_cmd(win, "set grid");
  gnuplot_setstyle(win, "lines");

  double in[size], out[size];

  // Input data
  index_t j = 0;
  for (j = 0; j < size; ++j) {
    // in[j] = ((j >= 40) & (j <= 60)) ? 1.0 : 0.0;
    in[j] = j;
  }
  // PLOT(in[ii], 0, size, "Input data");

  // double time = exp_broaden(size, in, out, -5);
  double time = exp_gaussian(size, in, out, 40, 10, 10);
  PLOT(out[ii], 0, size, "Output data");

  double d[size];
  deriv(size, out, d);
  // PLOT(d[ii], 0, size, "Derivative of exp");
  index_t peak = findpeak(size, out);
  printf("Peak position: %d\n", peak);

  // Plot markers in peak positions
  double marker_x[2] = { peak, peak };
  double marker_y[2] = { 0, 1 };
  gnuplot_plot_xy(win, marker_x, marker_y, 2, "Peak position");

  printf("Time: %f\n", time);
  printf("press ENTER to continue\n"); while (getchar()!='\n'){}
  
  gnuplot_close(win);
  free(data_points.x);
  free(data_points.y);
  win = NULL;
  data_points.x = NULL;
  data_points.y = NULL;
  */
  return 0;
}

/* Tests and WIP below*/

int 
test_signal_peaksearch(void)
{
  /*
  data_t d[] = {
    1,1,2,1,2,30,32,8,2,1,
    1,1,2,1,2, 2, 1,3,1,1,
    1,1,2,1,2,33,32,8,2,1,
    1,1,2,1,2, 2, 1,3,1,1};

  PeakSearch ps;
  ps.sigmas = 1.0;
  ps.win_size = 5;
  ps.data_s = sizeof(d) / sizeof(data_t);
  ps.data_v = (data_t*) malloc(ps.data_s * sizeof(data_t));
  ps.data_v = d;

  // Result array, initialized to minimum length (will expand automatically)
  index_t *peaks = (index_t*) malloc(sizeof(index_t));

  // Run the search on ps and put results in peaks
  index_t count = search_peaks(&ps, &peaks);

  // Printout:
  printf("Overall st.dev: %f. Found %d peaks\n", ps.stdev, count);
  index_t i;
  for (i=0; i<count; i++) {
    printf("peak %d: point %d, value %f\n", i, peaks[i], ps.data_v[peaks[i]]);
  }
  
  // Cleanup:
  // free(ps.data_v); // Commented due to error: munmap_chunk(): invalid pointer
  free(peaks);
  */
  return 0;
}

double
test_gsl_fft(void)
{
  double start_time = get_run_time();
  // Data
  const unsigned int size = 100;
  const index_t size_double = size * 2;
  double data[2*(size_double)];

  Points data_points;
  data_points.x = malloc(sizeof(data_t) * size_double);
  data_points.y = malloc(sizeof(data_t) * size_double);
  
  // Plot
  gnuplot_ctrl *win = gnuplot_init();
  gnuplot_cmd(win, "set grid");
  gnuplot_setstyle(win, "lines");

  index_t i = 0, j = 0, k = 0; // iterators
  
  gsl_fft_complex_wavetable *wavetable;
  gsl_fft_complex_workspace *workspace;

  wavetable = gsl_fft_complex_wavetable_alloc(size_double);
  workspace = gsl_fft_complex_workspace_alloc(size_double);
  
  // Original doubled signal
  for (i = 0; i < size_double; i++)
  {
    REAL(data,i) = ((i >= (40 + size/2)) & (i <= (60 + size/2))) ? 1.0 : 0.0; 
    IMAG(data,i) = 0.0;
  }
  // PLOT(REAL(data, ii), 0, size_double, "Original (doubled) signal");

  // Signal after FFT
  gsl_fft_complex_forward(data, 1, size_double, wavetable, workspace);
  // PLOT(REAL(data, ii), 0, size_double, "FFT(data)");

  // Exponential signal
  double t = -5;
  double a[size_double];
  double sum = 0;
  for (j = 0; j < size_double; j++)
  {
    a[j] = exp(-(j+1)/t);
    sum += a[j];
  }
  // PLOT(a[ii], 0, size_double, "a=exp");

  // FFT of exponential signal
  double data_a[2 * size_double];
  for (i = 0; i < size_double; i++)
  {
    REAL(data_a, i) = a[i]; 
    IMAG(data_a, i) = 0.0;
  }
  gsl_fft_complex_forward(data_a, 1, size_double, wavetable, workspace);
  // PLOT(REAL(data, ii), 0, size_double, "FFT(a)");

  // Multiplied FFT
  double fa[2 * size_double];
  for (k = 0; k < size_double * 2; k++)
  {
    fa[k] = data[k] * data_a[k];
  }
  PLOT(REAL(fa, ii), 0, size_double, "FFT(data) * FFT(a)");

  // IFFT
  gsl_fft_complex_inverse(fa, 1, size_double, wavetable, workspace);
  // PLOT(REAL(fa, ii) / sum, 0, size_double, "IFFT");

  // Compress data
  // data_points.x = realloc(data_points.x, sizeof(data_t) * size);
  // data_points.y = realloc(data_points.y, sizeof(data_t) * size);

  // index_t ii = 0;
  // for (index_t p = (size/2+2); p <= (size_double-(size/2)+1); p++)
  // {
  //   data_points.x[ii] = ii;
  //   data_points.y[ii] = REAL(fa, p)/sum;
  //   ii = ii + 1;
  // }
  // gnuplot_plot_points(win, &data_points, size, "Result");

  double end_time = get_run_time();
  printf("press ENTER to continue\n"); while (getchar()!='\n'){}

  gsl_fft_complex_wavetable_free(wavetable);
  gsl_fft_complex_workspace_free(workspace);  
  gnuplot_close(win);
  free(data_points.x);
  free(data_points.y);

  return end_time - start_time;
}

int
test_gsl_fft_small(void)
{
  index_t max = 5;
  gsl_fft_complex_wavetable *wavetable;
  gsl_fft_complex_workspace *workspace;

  wavetable = gsl_fft_complex_wavetable_alloc(max);
  workspace = gsl_fft_complex_workspace_alloc(max);

  double data[2 * max];

  // Setup input data
  for (index_t i = 0; i < max; i++)
  {
    REAL(data,i) = i + 1; // 1 2 3 4 5
    IMAG(data,i) = 0.0;
  }

  // FFT
  gsl_fft_complex_forward(data, 1, max, wavetable, workspace);
  for (index_t i = 0; i < max; i++)
  {
    printf("%f ", REAL(data,i)); // 15.000000 -2.500000 -2.500000 -2.500000 -2.500000
  }
  printf("\n");
  
  // IFFT
  gsl_fft_complex_inverse(data, 1, max, wavetable, workspace);
  for (index_t i = 0; i < max; i++)
  {
    printf("%f ", REAL(data,i)); // 1.000000 2.000000 3.000000 4.000000 5.000000
  }
  printf("\n");

  gsl_fft_complex_wavetable_free(wavetable);
  gsl_fft_complex_workspace_free(workspace); 

  return 0;
}

int 
test_signal_random(void)
{
  // init
  random_seed();
  
  gnuplot_ctrl *win = gnuplot_init();

  Random_Signal_Parameters rsp;
  rsp.n_points = 1000;
  rsp.desired_mean = 25.0;
  rsp.desired_std_deviation = 10.0;
  rsp.n_bases = 50;
  rsp.max_frequency = 10.0;
  rsp.noise_percentage = 5.0;

  // setup wavelength
  data_t lambda[rsp.n_points]; 
  float lambda_begin = 1500;
  float lambda_end = 1600;
  
  index_t i; 
  for (i = 0; i < rsp.n_points; i++)
  {
    lambda[i] = (double)(lambda_begin + ( i * ((lambda_end - lambda_begin)/rsp.n_points))); 
  }

  index_t gen_max = 1000;
  index_t gen_frequency = 5; // Hz

  // allocate memory for array of size=n_points with double values   
  data_t (*s)[] = malloc(sizeof(double) * rsp.n_points);

  index_t j;
  for (j = 0; j < gen_max; j++)
  {
    // generate signal
    random_signal_generate(rsp, s);

    // plot
    gnuplot_resetplot(win);
    gnuplot_cmd(win, "set term qt size 1000, 400");
    gnuplot_cmd(win, "set grid");
    gnuplot_setstyle(win, "lines") ;
    gnuplot_set_xlabel(win, "Wavelength, nm");
    gnuplot_set_ylabel(win, "Amplitude, a.u.");
    gnuplot_plot_xy(win, lambda, (*s), rsp.n_points, "Random signal generator");
    usleep(1000000/gen_frequency); // simulate mesurements frequency
  }

  // do not close window untill press ENTER
  printf("press ENTER to continue\n"); while (getchar()!='\n'){} 

  // free
  gnuplot_close(win);
  free(s);

  return 0;
}

int
peakfit_WIP(void)
{
  index_t n_points = 11;
  data_t x[] = {1.2, 2.2, 4.14, 3.6, 4.11, 9.8, 6.3, 4.4, 5.2, 3.6, 1.5};
  printf("%f \n", min(x, n_points));
  printf("%f \n", max(x, n_points));
  printf("%d \n", val2ind(x, n_points, 4.11));
  // TODO: unfinished

  return 0; 
}
