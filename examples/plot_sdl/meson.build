example_plot_sdl_src = []

example_plot_sdl_src += files([
   'plot_sdl.c',
])

example_plot_sdl_bin = executable('runplot_sdl', example_plot_sdl_src,
   c_args : [ ensen_cargs,
             '-DHAVE_CONFIG_H',
            ],
   include_directories  : [ config_dir, '.', '../../src/include' ],
   dependencies         : [ ensen_lib, ensen_deps ],
   install              : false
)

