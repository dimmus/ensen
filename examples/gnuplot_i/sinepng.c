#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ensen_private.h" // for ENSEN_UNUSED
#include "ui_gnuplot.h"

int main(int argc ENSEN_UNUSED, char * argv[] ENSEN_UNUSED)
{
	gnuplot_ctrl * g = gnuplot_init();

	gnuplot_cmd(g, "set terminal png");
	gnuplot_cmd(g, "set output \"sine.png\"");
	gnuplot_plot_equation(g, "sin(x)", "Sine wave");
	gnuplot_close(g);

	return 0 ;
}

