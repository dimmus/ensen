#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

#include "Ensen.h" // for __UNUSED__
#include "ui_gnuplot.h"

#define SLEEP_LGTH  1


int main(int argc __UNUSED__, char *argv[] __UNUSED__) 
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

