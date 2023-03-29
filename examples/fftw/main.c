#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <complex.h>
#include <fftw3.h>

#include "util.h"

#ifdef UI_GNUPLOT
  #include "ui_gnuplot.h"
#endif

#include "../../src/include/Ensen.h"

#include "../../src/include/ensen_private.h"

#include "../../src/include/ensen_signal.h"
#include "../../src/include/ensen_math.h"
#include "../../src/include/ensen_config.h"
#include "../../src/include/ensen_config_dictionary.h"

static void print_real(const float *r, int nx, bool io, float dscale);
static void print_complex(const fftwf_complex *z, int nx, bool io, float dscale);

void print_real(const float *r, int nx, bool io, float dscale) {
    printf(io ? "          ##### rin ####\n" : "          #### rout ####\n");
    printf("   ix         -real-\n");
    for (int ix = 0; ix < nx; ix++) {
        printf("%5d      %12.3e\n", ix, r[ix] / dscale);
    }
}

void print_complex(const fftwf_complex *z, int nx, bool io, float dscale) {
    printf(io ? "          ############ zin ##########\n" : "          ########### zout ##########\n");
    printf("   ix         -real-       -imag-\n");
    for (int ix = 0; ix < nx; ix++) {
        if (ix < (nx >> 1) + 1) {
            printf("%5d     (%12.3e,%12.3e)\n",
                ix,
                crealf(z[ix]) / dscale, cimagf(z[ix]) / dscale
            );
        }
        else {
            printf("%5d     (         ***,         ***)\n",
                ix
            );
        }
    }
}

int
test_exp_broaden_fftw(void)
{
  // FFT
  const unsigned int size = 100;
  const index_t size_double = size * 2;

  /* Memory Allocation */
  float *rin = (float *)fftwf_malloc(sizeof(float) * size_double);
  fftwf_complex *zout = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * ((size_double >> 1) + 1));
  if (rin == NULL || zout == NULL) {
      fprintf(stderr, "memory allocation error!!\n");
      return 1;
  }

  /* Plan Creation (out-of-place forward and backward FFT) */
  fftwf_plan planf = fftwf_plan_dft_r2c_1d(size_double, rin, zout, FFTW_ESTIMATE);
  fftwf_plan planb = fftwf_plan_dft_c2r_1d(size_double, zout, rin, FFTW_ESTIMATE);
  if (planf == NULL || planb == NULL) {
      fprintf(stderr, "plan creation error!!\n");
      return 1;
  }

  /* Input Initialization */
  printf(" ===INPUT===\n");
  for (index_t ix = 0; ix < size_double; ix++) {
      rin[ix] = ((ix >= (40 + size/2)) & (ix <= (60 + size/2))) ? 1.0 : 0.0;
  }
  print_real(rin, size_double, true, 1.0f);

  /* FFT Execution (forward) */
  printf(" ===FORWARD===\n");
  fftwf_execute(planf);
  print_complex(zout, size_double, false, 1.0f);

  /* FFT Execution (backward) */
  printf(" ===BACKWARD===\n");
  fftwf_execute(planb);
  print_real(rin, size_double, true, (float)size_double);

  /* Plan Destruction */
  fftwf_destroy_plan(planf);
  fftwf_destroy_plan(planb);

  /* Memory Free */
  fftwf_free(rin);
  fftwf_free(zout);

  double end_time = get_run_time();
  printf("press ENTER to continue\n"); while (getchar()!='\n'){}

  return 0;
}

int 
test_1d_c2r(int n) {
    double real, phi;

    int nCplx = n / 2 + 1;

    printf("n=%d nCplx=%d\n", n, nCplx);

    fftw_complex *in = fftw_alloc_complex(nCplx);
    double *ref_out = fftw_alloc_real(n);
    double *fftw_out = fftw_alloc_real(n);

    fftw_plan p = fftw_plan_dft_c2r_1d(n, in, fftw_out, FFTW_ESTIMATE);

    // fill the input array with random data
    fill_random_1d_cplx(nCplx, in);

    // compute the reference output
    for (int k = 0; k < n; ++k) {

        // start with DC component, which is purely real due to Hermitian symmetry
        ref_out[k] = creal(in[0]);

        int loopEnd = nCplx;

        // special case for even n
        if (n % 2 == 0) {
            // Nyquist element is purely real as well
            phi = 2.0 * M_PI * (nCplx - 1) * k / ((double) n);
            ref_out[k] += creal(in[nCplx - 1]) * cos(phi);

            loopEnd = nCplx - 1;
        }

        // middle elements are handled the same for even and odd n
        for (int j = 1; j < loopEnd; ++j) {
            phi = 2.0 * M_PI * j * k / ((double) n);

            real = creal(in[j]) * cos(phi) - cimag(in[j]) * sin(phi);
            ref_out[k] += 2.0 * real;
        }
    }

    // compute the DFT of in using FFTW
    fftw_execute(p);

    // compare reference output with FFTW output
    double eps = 1e-12;
    int status = compare_1d_real(n, ref_out, fftw_out, eps);

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(ref_out);
    fftw_free(fftw_out);

    return status;
}

int
main(int argc ENSEN_UNUSED, const char ** argv ENSEN_UNUSED)
{
  int status = 0;
  status += test_1d_c2r(32);
  status += test_1d_c2r(33);

  printf("press ENTER to continue\n"); while (getchar()!='\n'){}

  return 0;
}
