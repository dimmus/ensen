doc_target = []

tar = find_program('tar')

doc_target += custom_target('doxygen',
  command: [doxygen, join_paths(meson.current_build_dir(), 'Doxyfile')],
  output: ['empty'],
  build_by_default: false
)

compress_target = custom_target('package_doc_tar',
  command: [tar, '-C', meson.build_root(), '--xz',  '-cf', 'efl-'+meson.project_version()+'-doc.tar.xz', 'html', 'man'],
  output: 'ensen-'+meson.project_version()+'-doc.tar.xz',
  depends: [doc_target],
  build_by_default: false
)

run_target('doc',
  command: ['echo', 'Documentation built successfully'],
  depends: compress_target,
)

doxyfile_conf = configuration_data()
doxyfile_conf.merge_from(config_h)
doxyfile_conf.set('srcdir', meson.current_source_dir())
doxyfile_conf.set('top_builddir', meson.build_root())
doxyfile_conf.set('top_srcdir', meson.source_root())

configure_file(
  input: 'Doxyfile.in',
  output: 'Doxyfile',
  configuration: doxyfile_conf,
)
