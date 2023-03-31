#include "ensen_plot.h"

void
graphics_set(gnuplot_ctrl *win[], const Signal_Parameters conf)
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

void
graphics_reset(gnuplot_ctrl *win[], const Signal_Parameters conf)
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
