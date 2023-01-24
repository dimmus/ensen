#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "Eina.h"

#ifdef __GNUC__

# if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#  define __UNUSED__ __attribute__ ((__unused__))
# else
#  define __UNUSED__
# endif

# endif


