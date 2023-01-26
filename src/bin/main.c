#include <unistd.h> // usleep()
#include <stdlib.h> // clearscr()
#include <stdio.h>

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

#include "Ensen.h"

#include "ensen_signal.h"
#include "ensen_math.h"
#include "ensen_config.h"
#include "ensen_config_dictionary.h"

#include "signal_generator.h"
#include "signal_fit.h"
#include "signal_peaksearch.h"

int test_signal_peaksearch(void);
int test_signal_gaussian(const char * conf_name);
int test_signal_random(void);
int test_math_random_range(void);
int test_math_random_mt19937(void);
int test_fit(void);
static void clearscreen(void);
static void create_default_config_file(void);

int 
test_signal_peaksearch(void)
{
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

  return 0;
}

int 
test_signal_gaussian(const char * conf_name)
{
  // Setup config
  dictionary *ini;
  ini = config_load(conf_name);
  if (ini == NULL) 
  {
      fprintf(stderr, "cannot parse file: %s\n", conf_name);
      return -1 ;
  }
  // Show parsed config in terminal
  // config_dump(ini, stderr);

  // Setup signal parameters
  index_t n_points = (index_t)config_getint(ini, "points:number", -1);
  Point (*points)[] = malloc(sizeof(double) * 2 * n_points);
 
  index_t n_peaks = (index_t)config_getint(ini, "peaks:number", -1);;
  Peak peaks[n_peaks];

  peaks[0].amplitude = (data_t)config_getdouble(ini, "peaks:[0].amplitude", -1.0);
  peaks[0].position = (data_t)config_getdouble(ini, "peaks:[0].position", -1.0);
  peaks[0].width = (data_t)config_getdouble(ini, "peaks:[0].width", -1.0);
  
  peaks[1].amplitude = (data_t)config_getdouble(ini, "peaks:[1].amplitude", -1.0);
  peaks[1].position = (data_t)config_getdouble(ini, "peaks:[1].position", -1.0);
  peaks[1].width = (data_t)config_getdouble(ini, "peaks:[1].width", -1.0);
  
  peaks[2].amplitude = (data_t)config_getdouble(ini, "peaks:[2].amplitude", -1.0);
  peaks[2].position = (data_t)config_getdouble(ini, "peaks:[2].position", -1.0);
  peaks[2].width = (data_t)config_getdouble(ini, "peaks:[2].width", -1.0);

  peaks[3].amplitude = (data_t)config_getdouble(ini, "peaks:[3].amplitude", -1.0);
  peaks[3].position = (data_t)config_getdouble(ini, "peaks:[3].position", -1.0);
  peaks[3].width = (data_t)config_getdouble(ini, "peaks:[3].width", -1.0);

  Noise noise;
  noise.amplitude = config_getdouble(ini, "noise:amplitude", -1.0);

  // Peak search
  // Result array, initialized to minimum length (will expand automatically)
  index_t *peaks_arr = (index_t*) malloc(sizeof(index_t));

  PeakSearch ps;
  ps.sigmas = (data_t)config_getdouble(ini, "search:sigma", -1.0); // 0.15
  ps.win_size = (index_t)config_getdouble(ini, "search:window", -1.0); // 40
  ps.data_s = n_points; //sizeof(d) / sizeof(data_t);
  ps.data_v = (data_t*) malloc(ps.data_s * sizeof(data_t));

  // Setup plot parameters
  index_t gen_max = (index_t)config_getint(ini, "plot:generations", -1.0);
  index_t gen_frequency = (index_t)config_getint(ini, "plot:frequency", -1.0);; // Hz

  double plot_lim_y1 = config_getdouble(ini, "plot:y.min", -1.0);
  double plot_lim_y2 = config_getdouble(ini, "plot:y.max", -1.0);
  
  gnuplot_ctrl *win = gnuplot_init();
  gnuplot_cmd(win, "set term qt size 1000, 400");
  gnuplot_cmd(win, "set grid");
  gnuplot_cmd(win, "set xrange [0.0:%d]", n_points);
  gnuplot_cmd(win, "set yrange [%g:%g]", plot_lim_y1, plot_lim_y2);
  gnuplot_cmd(win, "set label \"Frequency: %d Hz\" at 40,1.4", gen_frequency);
  gnuplot_setstyle(win, "lines") ;
  gnuplot_set_xlabel(win, "Wavelength, nm");
  gnuplot_set_ylabel(win, "Amplitude, a.u.");

  // Generate and plot
  index_t gen_i = 0;
  random_seed();
  for (gen_i = 0; gen_i < gen_max; gen_i++)
  {
    // Clear initial values
    index_t i = 0;
    for (i = 0; i < n_points; i++) (*points)[i].x = (*points)[i].y = 0.0;

    // Generate signal
    signal_generate(points, n_peaks, peaks, noise, n_points);

    // Plot signal
    gnuplot_resetplot(win);
    // gnuplot_plot_points(win, points, n_points, "Signal");
    // signal_fit(points, n_points, 2);
    fsmooth(points, n_points, 50);
    fsmooth(points, n_points, 50);
    gnuplot_plot_points(win, points, n_points, "Signal (smooth)");

    // Plot markers in peak positions
    // for (index_t m = 0; m < n_peaks; m++)
    // {
    //   double marker_x[2] = { peaks[m].position, peaks[m].position };
    //   double marker_y[2] = { plot_lim_y1, plot_lim_y2 };
    //   gnuplot_plot_xy(win, marker_x, marker_y, 2, "Marker");
    // }

    index_t n;
    for (n = 0; n < n_points; n++)
    {
      ps.data_v[n] = (*points)[n].y;
    }

    // Run the search on ps and put results in peaks
    index_t count = search_peaks(&ps, &peaks_arr);

    // Print results
    printf("Overall st.dev: %f. Found %d peaks\n", ps.stdev, count);
    index_t k;
    for (k=0; k<count; k++) {
      printf("peak %d: point %d, value %f\n", k, peaks_arr[k], ps.data_v[peaks_arr[k]]);
    }
    
    // Simulate mesurements frequency  
    usleep(1000000/gen_frequency); 
  }

  // Do not close window untill we press any key
  printf("press ENTER to continue\n"); while (getchar()!='\n'){}

  // Cleanup:
  gnuplot_close(win);
  free(points);
  free(peaks_arr);
  config_freedict(ini);

  return 0;
}

static void
clearscreen(void)
{
  // system("cls||clear"); // win and linux
  printf("\e[1;1H\e[2J");
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
  double lambda[rsp.n_points]; 
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
  double (*s)[] = malloc(sizeof(double) * rsp.n_points);

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
test_math_random_range(void)
{
  random_seed();
  for(index_t i = 0; i < 40; i++) 
  {
    // printf("%d ", random_range(-1, 1));
    // printf("%d ", random_range_uniform(-2, 2));

    printf("%f ", random_zero_one());
    // printf("%f ", random_pm_one()); 
  }

  return 0;
}

int
test_math_random_mt19937(void)
{ 
  unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
  random_mt19937ar_cok_init_by_array(init, length);

  int i; 
  for (i=0; i<100; i++) 
  {
    // printf("%10.8f ", random_mt19937ar_cok_real1());
    printf("%10.8f ", random_pm_one());
    // printf("%10.8f ", random_zero_one(2));
    if (i%7==4) printf("\n");
  }

  return 0;
}

int
test_fit(void)
{
  index_t n_points = 11;
  double x[] = {1,2,4,3,5,9,6,4,5,3,1};
  printf("%f \n", min(x,n_points));
  printf("%d \n", val2int(x,n_points,6));
  // TODO: unfinished

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
    "\n"
    "[Points]\n"
    "number = 1000;\n"
    "\n"
    "[Peaks]\n"
    "number = 4;\n"
    "[0].amplitude = 1.0;\n"
    "[0].position = 200.0;\n"
    "[0].width = 100.0;\n"   
    "[1].amplitude = 0.8;\n"
    "[1].position = 400.0;\n"
    "[1].width = 100.0;\n"
    "[2].amplitude = 0.6;\n"
    "[2].position = 600.0;\n"
    "[2].width = 100.0;\n"
    "[3].amplitude = 0.4;\n"
    "[3].position = 800.0;\n"
    "[3].width = 100.0;\n"
    "\n"
    "[Noise]\n"
    "amplitude = 0.2;\n"
    "\n"
    "[Search]\n"
    "sigma = 0.15;\n"
    "window = 40;\n"
    "\n"
    "[Plot]\n"
    "generations = 1;\n"
    "frequency = 1;\n"
    "y.min = 0.0;\n"
    "y.max = 1.6;\n"
    "\n");
    fclose(ini);
}

int
main(int argc __UNUSED__, const char * argv[] __UNUSED__)
{
  int status;

  clearscreen();

  if (argc<2) {
      create_default_config_file();
      status = test_signal_gaussian("config.ini"); 
  } else {
      status = test_signal_gaussian(argv[1]); 
  }

  // status = test_signal_peaksearch();
  // status = test_signal_random(); 
  // status = test_math_random_range();
  // status = test_math_random_mt19937();
  // status = test_fit();

  return status;
}