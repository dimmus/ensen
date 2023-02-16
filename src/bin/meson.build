ensen_bin_src = []

ensen_bin_src += files([
   'main.c',
])

ensen_bin = executable('ensen', ensen_bin_src,
   c_args : [ ensen_cargs, native_arch_opt_c_args,
             '-DHAVE_CONFIG_H',
            ],
   include_directories  : [ config_dir, '../include' ],
   dependencies         : [ ensen_lib, ensen_deps, deps_nls ],
   install              : false
)

