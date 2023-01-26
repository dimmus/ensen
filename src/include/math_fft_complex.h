#ifndef MATH_FFT_COMPLEX_H
#define MATH_FFT_COMPLEX_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct complex_f {
    float re;
    float im;
} complex_f;

typedef struct complex_d {
    double re;
    double im;
} complex_d;

#define complex_mul_re(a_re, a_im, b_re, b_im)  (a_re * b_re - a_im * b_im)
#define complex_mul_im(a_re, a_im, b_re, b_im)  (a_re * b_im + a_im * b_re)

#ifdef __cplusplus
}
#endif

#endif
