panels_list += cappletname
desktop = 'gnome-@0@-panel.desktop'.format(cappletname)

desktop_in = configure_file(
  input: desktop + '.in.in',
  output: desktop + '.in',
  configuration: desktop_conf
)

i18n.merge_file(
  desktop,
  type: 'desktop',
  input: desktop_in,
  output: desktop,
  po_dir: po_dir,
  install: true,
  install_dir: control_center_desktopdir
)

datadir = get_option('datadir')

resource_data = files(
  'security-framework.ui',
  'resources/control-center-image.svg',
  'resources/control-center-small-image.png',
  'resources/ghub-image.svg',
  'resources/ghub-small-image.png',
  'resources/gcontroller-image.svg',
  'resources/gcontroller-small-image.png',
  'resources/gauth-image.svg',
  'resources/gauth-small-image.png',
  'resources/apps-image.png',
  'resources/apps-small-image.png',
  'resources/gagent-image.svg',
  'resources/gagent-small-image.png',
  'resources/gpms-image.svg',
  'resources/gpms-small-image.png',
  'resources/sound-wave.gif',
  'resources/sound-idle.png',
  'resources/lsf.png',
  'resources/policy-reload.wav'
)

common_sources = []
common_sources += gnome.compile_resources(
  'cc-@0@-resources'.format(cappletname),
  cappletname + '.gresource.xml',
  c_name: 'cc_' + cappletname.underscorify(),
  dependencies: resource_data,
  export: true
)

sources = files(
  'cc-security-framework-panel.c',
)

deps = common_deps + [
  libxml_dep,
  lsf_api_dep,
  lsf_json_dep
]

cflags += [
  '-DGNOMELOCALEDIR="@0@"'.format(control_center_localedir),
  '-DDATADIR="@0@"'.format(control_center_datadir),
]

sources += common_sources + files('cc-security-framework-panel.c')
panels_libs += static_library(
  cappletname,
  sources: sources + resources,
  include_directories: [ top_inc, common_inc ],
  dependencies: deps,
  c_args: cflags
)
