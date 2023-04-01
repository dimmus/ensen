#include "test_math.h"
#include "math/random/ensen_math_random_noise.h"

/* DIMMUS_START_TEST (noise_color_test_white_range)
{
    float noise = genWhiteNoise();
    if (noise < 0 || noise > 1)
    {
        ck_abort_msg("genWhiteNoise failure: generated value not in range [0,1]");
    }
}
DIMMUS_END_TEST */

DIMMUS_START_TEST (noise_color_test_white_rnd)
{
    init_rnd();
    data_t wn1 = genWhiteNoise();
    data_t wn2 = genWhiteNoise();
    if (wn1 - wn2 <= 1.0e-14) /* solved warning: comparing floating-point with ‘==’ or ‘!=’ is unsafe */
    {
        ck_abort_msg("genWhiteNoise failure: generated two equal values (not a random)");
    }
}
DIMMUS_END_TEST


void random_color_test(TCase *tc)
{
//    tcase_add_test(tc, noise_color_test_white_range); // return values not in range [0,1)
   tcase_add_test(tc, noise_color_test_white_rnd);
//    tcase_add_test(tc, noise_color_test_violet);
//    tcase_add_test(tc, noise_color_test_brown);
//    tcase_add_test(tc, noise_color_test_pink);
//    tcase_add_test(tc, noise_color_test_blue);
}
