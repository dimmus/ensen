project('ensen', ['c','cpp'],
        version       : '0.1.119',
        meson_version : '>= 0.59.0',
        default_options: [
                'c_std=c99',
                'cpp_std=c++20', # use in guarded memory allocator
                'warning_level=2',
                'buildtype=minsize', # "plain", "debug", "debugoptimized", "release", "minsize", "custom"
                'optimization=s',  # "plain", "0", "g", "1", "2", "3", "s"
                'debug=false'
              ],
)

proj     = meson.project_name()
ver      = meson.project_version()

ensen_version = ver.split('.')

config_h = configuration_data()

if ensen_version[2] == '99'
  git_version = '0'
  git         = find_program('git', required: false)
  if git.found() == true
    git_cmd = run_command(git.path(), 'rev-list', '--count', 'HEAD')
    if git_cmd.returncode() == 0
      git_version = git_cmd.stdout().strip()
    endif
  endif
  ensen_version_rev = '.'.join([ver, git_version])
  release = 'dev-@0@.@1@.@2@'.format(ensen_version[0], ensen_version[1], ensen_version[2])
else
  ensen_version_rev = ver
  release = '@0@.@1@.@2@'.format(ensen_version[0], ensen_version[1], ensen_version[2])
  config_h.set('ENSEN_RELEASE_BUILD', '1')
endif

##### binaries
cc = meson.get_compiler('c')
host_os = host_machine.system()

ensen_dev_cflags = []
ensen_cflags_try = [
  '-W',
  '-Wshadow',
  '-Wstrict-prototypes',
  '-Wmissing-prototypes',
  '-Wunused-parameter',
  '-Wsign-compare',
  '-Wpointer-arith',
  '-Wno-missing-field-initializers',
  '-Wfloat-equal',
  '-Wuninitialized',
  '-Wundef',
  '-Wendif-labels',
  '-Wcast-align',
  '-Wformat=2',
  '-Wimplicit-fallthrough=2',
  '-Winit-self',
  '-Wlogical-op',
  '-Wmissing-include-dirs',
  '-Wold-style-definition',
  '-Woverflow',
  '-Wstrict-aliasing=2',
  '-Wstrict-prototypes',
  '-fstrict-aliasing',
  '-Wno-format-y2k',
  '-Wno-variadic-macros', # no warning: ISO C does not permit named variadic macros (for C++ use warning_level=2)
  '-Wno-strict-aliasing', # no warning: dereferencing type-punned pointer will break strict-aliasing rules
  '-Wno-format-nonliteral', # no warning: format not a string literal, argument types not checked (lib/strsafe: snprintf_s)
  '-Wno-implicit-fallthrough', # switch off warnings in lib/mem_s/mem_primitives_lib.c
]

foreach cf: ensen_cflags_try
  if cc.has_argument(cf) == true
    ensen_dev_cflags += cf
  endif
endforeach
add_global_arguments(ensen_dev_cflags, language: 'c')

deps_os = declare_dependency(link_args : ['-lm'])

m = cc.find_library('m')

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
  # dependency('eina', version : efl_req),
  dependency('gsl'),
  dependency('fftw3'),
]

cov = find_program('lcov')
if (cov.found())
  #
endif

# configuration
config_dir = [include_directories('.')]
config_h.set_quoted('PACKAGE_NAME', meson.project_name())
config_h.set('HAVE_CONFIG_H', '1')

config_h.set_quoted('PACKAGE', meson.project_name())
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())
config_h.set_quoted('VERSION', meson.project_version())

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

if get_option('memguard')
  config_h.set('MEM_DEBUG_APPLY', '1')
endif

if get_option('debug-conf')
  config_h.set('DEBUG_CONF', '1')
endif

if get_option('random') == 'std'
  config_h.set('RANDOM_STD', '1')
elif get_option('random') == 'mt'
  config_h.set('RANDOM_MT', '1')
endif

cpu_sse3 = true
config_h.set10('BUILD_MMX', true)
config_h.set10('BUILD_SSE3', true)
native_arch_opt_c_args = [
                            '-msse3'
                         ]

host_os = host_machine.system()
if host_os == 'linux'
   config_h.set('_GNU_SOURCE', 1)
   config_h.set('__EXTENSIONS__', 1)
   config_h.set('_POSIX_PTHREAD_SEMANTICS', 1)
   config_h.set('_TANDEM_SOURCE', 1)
   config_h.set('_ALL_SOURCE', 1)
   config_h.set('_POSIX_SOURCE', 1)
   config_h.set('_POSIX_1_SOURCE', 1)
endif

pkgconfig = import('pkgconfig')

ensen_cargs = [
  '-D_POSIX_C_SOURCE=200809L',
  '-D_XOPEN_SOURCE=500',
]

##### documentation
# build_doc = false
# scdoc = dependency('scdoc', native: true, required: get_option('docs'), version: '>=1.9.0')
# if scdoc.found()
# 	build_doc = true
# endif
# if build_doc
# 	subdir('doc')
# endif
doxygen = find_program('doxygen', required : false)

if doxygen.found()
  subdir('doc')
endif

subdir('po')
subdir('src')

if get_option('tests')
  check = dependency('check')

  if (check.version() == '0.15.1')
    error('There is a bug in check@0.15.1 which does not allow us to be compiled with it. Please downgrade / upgrade or disable tests')
  endif

  test_env = environment()

  if get_option('b_sanitize') == 'address'
    test_env.set('ASAN_OPTIONS', 'detect_leaks=0:detect_odr_violation=0')
  endif

  subdir(join_paths('src', 'tests'))
endif

if get_option('examples')
    subdir('examples')
endif

# Use config_h after all subdirs have set values
configure_file(output       : 'config.h',
               configuration: config_h,
               install_dir  : dir_include)

dir_pkgconfig = join_paths(dir_lib, 'pkgconfig')
pkgconfig.generate(name             : proj,
                   description      : 'Multimode FBG-based temperature sensing ',
                   filebase         : proj,
                   subdirs          : proj,
                  #  requires         : requires_e,
                   version          : ensen_version_rev,
                   libraries_private: '-lm',
                   install_dir      : dir_pkgconfig,
                   variables        : [ 'exec_prefix=${prefix}',
                                        'datarootdir=@0@'.format(dir_data),
                                        'datadir=${datarootdir}',
                                        # 'modules=@0@/enlightenment/modules'.format(dir_lib),
                                        'pkgdatadir=${datarootdir}/' + proj + '/data',
                                        # 'themes=${pkgdatadir}/themes',
                                        # 'module_arch=@0@'.format(module_arch),
                                        # 'backgrounds=${pkgdatadir}/backgrounds',
                                        'release=@0@'.format(release),
                                        # 'wayland=@0@'.format(wayland)
                                      ]
                  )

# Output
summary({'OS': host_os,
         'Release:': release,
         'Version:': ensen_version_rev,
        #  'NLS': build_nls ? 'yes' : 'no'
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
