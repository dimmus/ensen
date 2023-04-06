#include <stdio.h>
#include <stdlib.h>
#include <time.h> /* srand48() */
#include <sys/time.h>

#include "ensen_math_random.h"

#define MT_N 624
#define MT_M 397

static unsigned long mt[MT_N];
static int mti = MT_N + 1;

void mt_init(unsigned long seed)
{
  mt[0] = seed;
  for (mti = 1; mti < MT_N; mti++)
  {
    mt[mti] = 1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti;
  }
}

unsigned long
mt_rand(void)
{
  unsigned long y;
  static unsigned long mag01[2] = {0x0UL, 0x9908b0dfUL};
  if (mti >= MT_N)
  {
    int kk;
    if (mti == MT_N + 1)
    {
      mt_init(5489UL);
    }
    for (kk = 0; kk < MT_N - MT_M; kk++)
    {
      y = (mt[kk] & 0x80000000UL) | (mt[kk + 1] & 0x7fffffffUL);
      mt[kk] = mt[kk + MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (; kk < MT_N - 1; kk++)
    {
      y = (mt[kk] & 0x80000000UL) | (mt[kk + 1] & 0x7fffffffUL);
      mt[kk] = mt[kk + (MT_M - MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (mt[MT_N - 1] & 0x80000000UL) | (mt[0] & 0x7fffffffUL);
    mt[MT_N - 1] = mt[MT_M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
    mti = 0;
  }
  y = mt[mti++];
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);
  return y;
}

#ifdef RANDOM_MT
data_t rnd(void)
{
  return (data_t)mt_rand() / (data_t)_SC_ULONG_MAX;
}
#else
data_t
rnd(void)
{
  return (data_t)rand() / (data_t)RAND_MAX;
}
#endif

void
init_rnd(void)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec);
  return;
}

void
random_seed(index_t algorithm)
{
  switch (algorithm)
  {
  case 1:
    srand(time(NULL) ^ (getpid() << 16));
    break;
  case 2:
    srand48(time(NULL));
    break;
  default:
    srand(time(NULL) ^ (getpid() << 16));
    break;
  }  

}

int 
random_range(int lower, int upper)
{
	return (rand() % (upper - lower + 1)) + lower;
}

int 
random_range_uniform(int lower, int upper) {
    double myRand = rand()/(1.0 + RAND_MAX); 
    int range = upper - lower + 1;
    int myRand_scaled = (myRand * range) + lower;
    return myRand_scaled;
}

data_t 
random_range_zero_one(void)
{
    return (data_t)drand48();
}

data_t 
random_range_pm_one(void)
{
    return ((double)rand()/RAND_MAX)*2 - 1;
    /* return (data_t)mrand48()/RAND_MAX; */
}

