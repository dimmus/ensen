#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#ifdef ENABLE_NLS
# include <libintl.h>
#endif

#include "ensen_utils.h"
#include "ensen_conf.h"
#include "ensen_data.h"
#include "ensen_show.h"
#include "ensen_plot.h"
#include "ensen_exp.h"

#include "mem/ensen_mem_guarded.h"
#include "str/safe_lib.h"

#include "ensen.h"

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
  Points data, data_temp;
  Points temp_gen, temp_sens_1, temp_sens_2, temp_sens_3, temp_sens_4;
  Signal_Statistics stat;
  
  config_parameters_set(&conf, ini);
  PointsArray data_arrays = { 
                              &data,
                              &data_temp, 
                              &temp_gen, 
                              &temp_sens_1, 
                              &temp_sens_2, 
                              &temp_sens_3, 
                              &temp_sens_4 
  };
  data_arrays_set_new(conf, data_arrays);

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

  // derivative
  dy_dx = (conf.plot.show_derivative) ? MEM_malloc_arrayN(conf.n_points, sizeof(data_t), "test_signal: dy_dx") : NULL;

  init_rnd();

  /* Generate main signal */
  Peaks peaks;
  peaks.peak = MEM_malloc_arrayN(conf.search.peaks_array_number, sizeof(Peak), "test_signal: peaks.peak array");

  stat.delta_temp = (conf.temp.apply) ? (conf.temp.max - conf.temp.room)/(conf.generation_max/conf.temp.tick) : 0.f;

  /* Setup experiments */
  index_t smooth_level = 1;
  data_t x[conf.generation_max];
  data_t y[conf.generation_max];
  data_t noise_amplitude = 0.f;
  data_t noise_step = 0.05;
  conf.smooth.width = (conf.plot.show_vs_smooth == 2) ? 25 : 100;

  data_clear(x, conf.generation_max);

  index_t n_step = 0;
  for (i_gen = 0; i_gen <= conf.generation_max; i_gen++)
  {
    /*
     * ======================================
     * Clear data from the previous iteration
     * ======================================
     */

    graphics_reset(win, conf); /* Clean plot for next generation */
    data_clear(data.y, conf.n_points); /* Clear data (set to zero) */

    /*
     * ======================================
     * Generate new signal
     * ======================================
     */

    /* 
     * Save previously generated signal peak positions
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
     * ==== Experiment 2: simulate noise level change ===
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
    else if (conf.plot.show_vs_noise == 2) // change noise color
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
     * ==== Experiment 3: evaluate smooth level change ===
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
main(int argc __UNUSED__, const char ** argv __UNUSED__)
{
# ifdef MEM_DEBUG_APPLY
  MEM_use_guarded_allocator(); /* SLOW, use only for debugging */
# endif
  MEM_init_memleak_detection();
  MEM_enable_fail_on_memleak();

  clearscreen();
  
  const char * conf_file_name = "config.ini";
  FILE *f = fopen(conf_file_name, "r");
  if (f == NULL) {
    config_parameters_set_default();
    printf("Cannot find configuration file: %s. Created the new one!\n", conf_file_name);
  }

  test_signal("config.ini");
  
  if (f)
  {
    fclose(f);
  }

# ifdef MEM_DEBUG_APPLY
  printf("Used %ld kB of memory \n", MEM_get_peak_memory()/1024);
# endif

  return 0;
}
