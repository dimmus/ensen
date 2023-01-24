example_math_fft_src = []

example_math_fft_src += files([
   'app.c',
   'main.c',
])

example_math_fft_bin = executable('runfft', example_math_fft_src,
   c_args : [ ensen_cargs,
             '-DHAVE_CONFIG_H',
            ],
   include_directories  : [ config_dir, '.', '../../src/include' ],
   dependencies         : [ ensen_lib, ensen_deps ],
   install              : false
)

