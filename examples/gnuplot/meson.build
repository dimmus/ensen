example_gnuplot_src = []

example_gnuplot_src += files([
   'main.c',
])

example_gnuplot_bin = executable('run_gnuplot', example_gnuplot_src,
   c_args : [ ensen_cargs,
             '-DHAVE_CONFIG_H',
            ],
   include_directories  : [ config_dir, '.', '../../src/include' ],
   dependencies         : [ ensen_lib, ensen_deps ],
   install              : false
)

