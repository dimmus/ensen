example_gnuplot_anim_bin = executable('anim', 'anim.c',
   c_args : [ ensen_cargs,
             '-DHAVE_CONFIG_H',
            ],
   include_directories  : [ config_dir, '.', '../../src/include' ],
   dependencies         : [ ensen_lib, ensen_deps ],
   install              : false
)

example_gnuplot_example_bin = executable('example', 'example.c',
   c_args : [ ensen_cargs,
             '-DHAVE_CONFIG_H',
            ],
   include_directories  : [ config_dir, '.', '../../src/include' ],
   dependencies         : [ ensen_lib, ensen_deps ],
   install              : false
)

example_gnuplot_sinepng_bin = executable('sinepng', 'sinepng.c',
   c_args : [ ensen_cargs,
             '-DHAVE_CONFIG_H',
            ],
   include_directories  : [ config_dir, '.', '../../src/include' ],
   dependencies         : [ ensen_lib, ensen_deps ],
   install              : false
)

