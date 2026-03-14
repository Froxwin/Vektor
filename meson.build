project(
  'vektor',
  'c',
  version: '0.0.0',
  meson_version: '>=0.63',
  default_options: [
    'c_std=c2x',
    'warning_level=3',
    'buildtype=debug',
  ],
)

c_args = meson.get_compiler('c').get_supported_arguments([
  '-Wno-unused-variable',
  '-Wno-unused-parameter',
  '-Wno-pedantic'
  ])
add_project_arguments(c_args, language: 'c')

gtk = dependency('gtk4', required: true, include_type: 'system')
epoxy = dependency('epoxy', include_type: 'system')

src = files(
  'src/main.c',
  'src/core/matrix.c',
  'src/core/primitives.c',
  'src/core/raster.c',
  'src/core/modifier.c',
  'src/ui/uicontroller.c',
  'src/ui/vektorcanvas.c',
  'src/ui/widgets/colorwheel.c',
  'src/application/applicationstate.c'
)

executable(
  'vektor',
  src,
  dependencies: [gtk, epoxy],
  link_args: ['-lm'],
  install: true,
)

