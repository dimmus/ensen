#ifndef ENSEN_PLOT_H
#define ENSEN_PLOT_H

#include "signal/ensen_signal.h"
#include "ui/ensen_ui.h"

void graphics_set(gnuplot_ctrl *win[], const Signal_Parameters conf);
void graphics_reset(gnuplot_ctrl *win[], const Signal_Parameters conf);
void free_gnuplot(Signal_Parameters conf, gnuplot_ctrl **win);

#endif