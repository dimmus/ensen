#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <stdio.h>

#include "signal_form_gaussian.h" 
 
float gaussian_old(void) 
{
    float v1,v2,s;
 
    do {
        v1 = 2.0 * ((float) rand()/RAND_MAX) - 1;
        v2 = 2.0 * ((float) rand()/RAND_MAX) - 1;
 
        s = v1*v1 + v2*v2;
        printf("%f \n", s);
    } while ( s >= 1.0 || s == 0.0);
 
    return (v1*sqrt(-2.0 * log(s) / s));
}

// gaussian(X,pos,wid) = gaussian peak centered on pos, half-width=wid
// Examples: 
// gaussian([0 1 2],1,2) gives result [0.5000    1.0000    0.5000]
// gaussian([1:100],50,20) displays gaussian band centered at 50 with width 20.
float 
gaussian(float x, float pos, float wid)
{
    float arg = (x-pos)/(0.60056120439323 * wid);
    return exp(-(arg * arg));   
}
