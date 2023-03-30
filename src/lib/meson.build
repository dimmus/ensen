ensen_lib_src = []

subdir('config')
subdir('math')
subdir('signal')
subdir('ui')
if get_option('memguard')
    subdir('mem')
endif

libensen = library('ensen', ensen_lib_src,
  c_args : [ ensen_cargs,
             '-DHAVE_CONFIG_H'
           ],
  dependencies : ensen_deps,
  include_directories : [config_dir, '../include'],
  install : true,
  version : meson.project_version()
)

ensen_lib = declare_dependency(
  include_directories : [ include_directories('../include')],
  link_with : libensen,
  dependencies : ensen_deps
)
