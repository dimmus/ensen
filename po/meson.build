dep_intl = []

if get_option('nls')
  i18n = import('i18n')
  add_project_arguments('-DGETTEXT_PACKAGE="ensen"', language:'c')
  i18n.gettext(meson.project_name(),
             args: [
                '--directory=' + meson.project_source_root(),
                '--keyword=_',
                '--keyword=d_:1',
                '--keyword=P_:1,2',
                '--keyword=dP_:1,2',
                '--keyword=N_',
                '--keyword=NP_:1,2',
                '--from-code=UTF-8',
                '--foreign-user'
              ])
  config_h.set('HAVE_GETTEXT', '1')
  dep_intl = cc.find_library('intl', required : false)
endif

# maintainer shortcut for updating po stuff
run_target ('po',
  command: ['ninja', '-C', meson.project_build_root(),
    meson.project_name() + '-pot',
    meson.project_name() + '-update-po']
)