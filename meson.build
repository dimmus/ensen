project('ensen', 'c',
        version       : '0.0.99',
        meson_version : '>= 0.53.0',
    		default_options: [
          'c_std=c99',
          'warning_level=2',
          'buildtype=debug',
        ],
)

v_array = meson.project_version().split('.')
v_maj = v_array[0]
v_min = v_array[1]
v_mic = v_array[2]

##### binaries
cc = meson.get_compiler('c')
host_os = host_machine.system()

ensen_cflags = []
ensen_cflags_try = [
  '-Wshadow',
  '-Wstrict-prototypes',
  '-Werror=missing-prototypes',
  '-Werror=pointer-arith',
  '-Wno-missing-field-initializers']

foreach cf: ensen_cflags_try
  if cc.has_argument(cf) == true
    ensen_cflags += cf
  endif
endforeach
add_global_arguments(ensen_cflags, language: 'c')

deps_os = declare_dependency(link_args : ['-lm -lfftw3'] )

##### dir locations
dir_prefix = get_option('prefix')
dir_include = join_paths(dir_prefix, get_option('includedir'))
dir_pkginclude = join_paths(dir_include, meson.project_name())
dir_bin = join_paths(dir_prefix, get_option('bindir'))
dir_lib = join_paths(dir_prefix, get_option('libdir'))
dir_data = join_paths(dir_prefix, get_option('datadir'))
dir_pkgdata = join_paths(dir_data, meson.project_name())
dir_locale = join_paths(dir_prefix, get_option('localedir'))

###### dependencies
efl_req = '>= 1.26.3'
ensen_deps = [
  dependency('eina', version : efl_req),
  dependency('gsl'),
  dependency('fftw3')
]

# configuration
config_dir = [include_directories('.')]
config_h = configuration_data()
config_h.set_quoted('PACKAGE_NAME', meson.project_name())

if get_option('ui-backend') == 'gnuplot'
  config_h.set('UI_GNUPLOT', '1')
  find_program('gnuplot', native: true)
elif get_option('ui-backend') == 'sdl'
  config_h.set('UI_SDL', '1')
  ensen_deps += [dependency('SDL2'), dependency('SDL2_ttf')]
elif get_option('ui-backend') == 'python'
  config_h.set('UI_PYTHON', '1')
  ensen_deps += [dependency('python3')]
elif get_option('ui-backend') == 'plplot'
  config_h.set('UI_PLPLOT', '1')
  ensen_deps += [dependency('plplot')]
elif get_option('ui-backend') == 'nuklear'
  config_h.set('UI_NUKLEAR', '1')
  ensen_deps += [dependency('nuklear')]
elif get_option('ui-backend') == 'efl'
  config_h.set('UI_EFL', '1')
  ensen_deps += [dependency('efl', version : efl_req)]
endif

pkgconfig = import('pkgconfig')

ensen_cargs = [
  '-D_POSIX_C_SOURCE=200809L',
  '-D_XOPEN_SOURCE=500',
  '-D_XOPEN_SOURCE_EXTENDED', # usleep()
  '-D_USE_MISC', # usleep()
]

##### translation
build_nls = false
deps_nls = []
if get_option('nls')
  intl_dep = cc.find_library('intl', required: false)
  if intl_dep.found() and cc.has_header('libintl.h')
    deps_nls += cc.find_library('intl', required: true)
    ensen_cargs += '-DENABLE_NLS'
    build_nls = true
  endif
endif

##### documentation
build_doc = false
scdoc = dependency('scdoc', native: true, required: get_option('man-pages'), version: '>=1.9.0')
if scdoc.found()
	build_doc = true
endif

subdir('src')

if build_doc
	subdir('doc')
endif

if build_nls
	subdir('po')
endif

if get_option('examples')
    subdir('examples')
endif

# Use config_h after all subdirs have set values
configure_file(output: 'config.h', configuration: config_h)

# Output
summary({'OS': host_machine.system(),
         'NLS': build_nls ? 'yes' : 'no'
        }, section: 'Configuration Options Summary:')

summary({'prefix': dir_prefix,
         'bindir': dir_bin,
         'libdir': dir_lib,
        #  'incdir': dir_include,
         'pkgincdir': dir_pkginclude,
        #  'datadir': dir_data,
         'pkgdatadir': dir_pkgdata,
         'localedir': dir_locale,
        }, section: 'Directories:')

summary({'compilation': 'ninja -C build',
         'installation': 'sudo ninja -C build install',
        }, section: 'Compilation')
