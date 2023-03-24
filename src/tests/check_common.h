#ifndef CHECK_COMMON_H
#define CHECK_COMMON_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h> /* getenv */
#include <stdio.h> /* fprintf, fputs */
#include <string.h> /* strcmp */
#include <strings.h>
#include <unistd.h> /* execvp */
#include <errno.h> /* errno */
#include <sys/time.h>

#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#include <check.h>

# ifdef __GNUC__

#  if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#   define EINA_UNUSED __attribute__ ((__unused__))
#  else
#   define EINA_UNUSED
#  endif
# else
#  define EINA_UNUSED
# endif

#define ENABLE_TIMING_INFO

/* Uncomment this line to print a warning line to stdout for every expected error received.
   Otherwise, expected errors are just silently ignored, greatly reducing the log size.
*/
//#define PRINT_EXPECTED_ERROR

typedef struct _Dimmus_Test_Case Dimmus_Test_Case;
struct _Dimmus_Test_Case
{
   const char *test_case;
   void (*build)(TCase *tc);
};

static void
_dimmus_tests_list(const Dimmus_Test_Case *etc)
{
   const Dimmus_Test_Case *itr = etc;
      fputs("Available Test Cases:\n", stderr);
   for (; itr->test_case; itr++)
      fprintf(stderr, "\t%s\n", itr->test_case);
}

EINA_UNUSED static int
_dimmus_test_option_disp(int argc, char **argv, const Dimmus_Test_Case *etc)
{
   int i;

   for (i = 1; i < argc; i++)
     {
        if ((strcmp(argv[i], "-h") == 0) ||
            (strcmp(argv[i], "--help") == 0))
          {
             fprintf(stderr, "Usage: %s [options] [test_case1 .. [test_caseN]]\n",
                     argv[0]);
             fprintf(stderr, " -l\t--list\t\tList all tests case.\n");
             fprintf(stderr, " \t--valgrind\tRun the tests under valgrind.\n");
             fprintf(stderr, "\nNote that CK_RUN_CASE=test_case does also filter which tests are run\n");
             return 0;
          }
        else if ((strcmp(argv[i], "-l") == 0) ||
                 (strcmp(argv[i], "--list") == 0))
          {
             _dimmus_tests_list(etc);
             return 0;
          }
        else if (strcmp(argv[i], "--valgrind") == 0)
          {
             const char ** nav = (const char **) alloca(sizeof(char*) * (argc + 3));
             int j, k;

             nav[0] = "valgrind";
             nav[1] = "--trace-children=yes";
             for (j = 0, k = 2; j < argc; j++)
               {
                  if (i == j) continue ; // Remove --valgrind
                  nav[k++] = argv[j];
               }

             nav[k] = NULL;
             execvp("valgrind", (char**) nav);
             fprintf(stderr, "Failed to execute valgrind due to '%s'\n", strerror(errno));
             return 0;
          }
     }

   return 1;
}

static int
_dimmus_test_use(int argc, const char **argv, const char *test_case)
{
   if (argc < 1)
     return 1;

   for (; argc > 0; argc--, argv++)
     if (strcasecmp(test_case, *argv) == 0)
       return 1;
   return 0;
}

#ifdef ENABLE_TIMING_INFO
EINA_UNUSED static double _timing_start_time;

static int
_timing_enabled(void)
{
   const char *lc = getenv("TIMING_ENABLED");
   return !!lc;
}

static double
_timing_time_get(void)
{
   struct timeval timev;

   gettimeofday(&timev, NULL);
   return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

EINA_UNUSED static void
_timing_start(void)
{
   if (_timing_enabled())
     _timing_start_time = _timing_time_get();
}

EINA_UNUSED static void
_timing_end(void)
{
   double diff;
   static int thres_set = 0;
   static double thres = 0;

   if (!_timing_enabled()) return;
   if (!thres_set)
     {
        const char *env = getenv("TIME_DIFF_THRESHOLD");

        if (env)
          {
             thres = strtod(env, NULL);
             if (thres > 0)
               thres_set = 1;
          }
        if (!thres_set)
          thres = 0.0001;
        thres_set = 1;
     }
   diff = _timing_time_get() - _timing_start_time;

   if (diff > thres)
     printf("TIME %s: %.5g\n", tcase_name(), diff);
}

# define DIMMUS_START_TEST(TEST_NAME) \
  START_TEST(TEST_NAME) \
  { \
  _timing_start();

# define DIMMUS_END_TEST \
  _timing_end(); \
  } \
  END_TEST

#else
# define DIMMUS_START_TEST(TEST_NAME) START_TEST(TEST_NAME)
# define DIMMUS_END_TEST END_TEST

#endif

static char srunner_xml_buf[4096];

static int
_dimmus_suite_run_end(SRunner *sr, const char *name)
{
   int failed_count;

   if (name)
     {
        char *n = strdup(name);
        char *p;

        for (p = n; p[0]; p++)
          {
             switch (p[0])
               {
                case ' ':
                case '/':
                case '\\':
                case ';':
                  p[0] = '_';
                  break;
               }
          }
        snprintf(srunner_xml_buf, sizeof(srunner_xml_buf), "./check-results-%s.xml", n);
        srunner_set_xml(sr, srunner_xml_buf);
        free(n);
     }
   else
     srunner_set_xml(sr, "./check-results.xml");
   srunner_run_all(sr, CK_ENV);
   failed_count = srunner_ntests_failed(sr);
   srunner_free(sr);
   return failed_count;
}

EINA_UNUSED static int
_dimmus_suite_build_and_run(int argc, const char **argv, const char *suite_name, const Dimmus_Test_Case *etc, SFun init, SFun shutdown)
{
   Suite *s;
   SRunner *sr;
   TCase *tc;
   int i, failed_count = 0;

#ifdef ENABLE_TIMING_INFO
   double tstart = 0.0;
   int timing;

   timing = _timing_enabled();
   if (timing)
     tstart = _timing_time_get();
#endif

   fflush(stdout);
   s = suite_create(suite_name);
   sr = srunner_create(s);

   for (i = 0; etc[i].test_case; ++i)
     {
        if (!_dimmus_test_use(argc, argv, etc[i].test_case))
           continue;
        tc = tcase_create(etc[i].test_case);
        if (init || shutdown)
          tcase_add_checked_fixture(tc, init, shutdown);

        etc[i].build(tc);
        suite_add_tcase(s, tc);
     }

     failed_count = _dimmus_suite_run_end(sr, NULL);

#ifdef ENABLE_TIMING_INFO
   if (timing)
     {
        printf("SUITE TIME(%u) %s: %.5g\n", getpid(), suite_name, _timing_time_get() - tstart);
        fflush(stdout);
     }
#endif
   return failed_count;
}

#define SUITE_INIT(NAME) static void _##NAME##_suite_init(void)
#define SUITE_INIT_FN(NAME) _##NAME##_suite_init
#define SUITE_SHUTDOWN(NAME) static void _##NAME##_suite_shutdown(void)
#define SUITE_SHUTDOWN_FN(NAME) _##NAME##_suite_shutdown

#endif
