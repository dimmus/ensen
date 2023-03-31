#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "ensen_private.h" 
#include "ensen_signal_form_gaussian.h" 

// gaussian(X,pos,wid) = gaussian peak centered on pos, half-width=wid
// Examples: 
// gaussian([0 1 2],1,2) gives result [0.5000    1.0000    0.5000]
// gaussian([1:100],50,20) displays gaussian band centered at 50 with width 20.
data_t 
gaussian(data_t  x, data_t  pos, data_t  wid)
{
    data_t  arg = (x-pos)/(0.60056120439323 * wid);
    return exp(-(arg * arg));   
}
