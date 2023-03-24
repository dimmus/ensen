#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ensen_private.h" // for __UNUSED__
#include "ui_gnuplot.h"

int main(int argc __UNUSED__, char * argv[] __UNUSED__)
{
	gnuplot_ctrl * g = gnuplot_init();

	gnuplot_cmd(g, "set terminal png");
	gnuplot_cmd(g, "set output \"sine.png\"");
	gnuplot_plot_equation(g, "sin(x)", "Sine wave");
	gnuplot_close(g);

	return 0 ;
}

