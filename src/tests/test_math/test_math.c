#include "test_math.h"
#include "math/random/ensen_math_random_noise.h"

static const Dimmus_Test_Case etc[] = {
  { "Color", random_color_test },
  { NULL, NULL }
};

SUITE_INIT(ensen) {
    init_rnd();
    mt_init(rand());
}

SUITE_SHUTDOWN(ensen) {
}

int
main(int argc, char **argv)
{
   int failed_count;

   if (!_dimmus_test_option_disp(argc, argv, etc))
     return 0;

   failed_count = _dimmus_suite_build_and_run(argc - 1, (const char **)argv + 1,
                                           "Random", etc, SUITE_INIT_FN(ensen), SUITE_SHUTDOWN_FN(ensen));

   return (failed_count == 0) ? 0 : 255;
}