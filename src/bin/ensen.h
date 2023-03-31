#ifndef ENSEN_H
#define ENSEN_H

#define PLOT(data, from, to, title)       \
  for (index_t ii = from; ii <= to; ii++) \
  {                                       \
    data_points.x[ii] = ii;               \
    data_points.y[ii] = data;             \
  }                                       \
  gnuplot_plot_points(win, &data_points, to, title); \

int test_signal(const char * conf_name);

#endif