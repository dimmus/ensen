#ifndef ENSEN_UI_H
#define ENSEN_UI_H

#ifdef UI_GNUPLOT
  #include "ui/gnuplot/ensen_ui_gnuplot.h"
#endif

#ifdef UI_NUKLEAR
  #define NK_IMPLEMENTATION
  #include "ui_nuklear.h"
#endif

#ifdef UI_SDL
  #include "ui_sdl_llist.h"
  #include "ui_sdl_plot.h"
#endif

#endif