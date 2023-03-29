test_math_src = [
  'test_math.c',
  'test_math.h',
  'random_color.c',
]

test_math_bin = executable('test_math',
  test_math_src,
  dependencies: [ensen_deps, check, m, ensen_lib],
  link_with: libensen,
  c_args : [
    '-DTESTS_BUILD_DIR="'+meson.current_build_dir()+'"',
    '-DTESTS_SRC_DIR="'+meson.current_source_dir()+'"']
)

test('test_math', test_math_bin,
  env : test_env
)