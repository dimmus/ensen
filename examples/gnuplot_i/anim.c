#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

#include "ensen_private.h" // for ENSEN_UNUSED
#include "ui_gnuplot.h"

#define SLEEP_LGTH  1


int main(int argc ENSEN_UNUSED, char *argv[] ENSEN_UNUSED) 
{
    gnuplot_ctrl    *   h1;
    double              phase ;

    printf("*** example of gnuplot control through C ***\n") ;
    h1 = gnuplot_init() ;

    for (phase=0.1 ; phase<10 ; phase +=0.1) {
        gnuplot_resetplot(h1) ;
        gnuplot_cmd(h1, "plot sin(x+%g)", phase) ;
        sleep(1);
    }
    
    for (phase=10 ; phase>=0.1 ; phase -=0.1) {
        gnuplot_resetplot(h1) ;
        gnuplot_cmd(h1, "plot sin(x+%g)", phase) ;
        sleep(1);
    }
    
    gnuplot_close(h1) ;
    
    return 0 ;
}

