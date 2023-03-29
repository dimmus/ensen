ensen_lib_header_src = [
  'ensen_private.h',
]
ensen_lib_src = []

ensen_ext_deps = [intl, m]

subdir('mem')
subdir('config')
subdir('math')
subdir('signal')
subdir('ui')


libensen = library('ensen', ensen_lib_src,
  dependencies : [ensen_deps, ensen_ext_deps],
  include_directories : config_dir + [include_directories(join_paths('..','..'))],
  install : true,
  c_args : ensen_cargs,
  version : meson.project_version()
)

ensen_lib = declare_dependency(
  include_directories : [ include_directories('.')],
  link_with : libensen,
  dependencies : ensen_deps
)

install_headers(ensen_lib_header_src,
  install_dir : dir_include,
)