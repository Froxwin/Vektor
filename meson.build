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

gtk = dependency('gtk4', required: true)
epoxy = dependency('epoxy')

src = files(
  'src/main.c',
  'src/core/matrix.c',
  'src/core/primitives.c',
  'src/core/raster.c',
  'src/ui/uicontroller.c',
  'src/ui/vektorcanvas.c',
  'src/application/applicationstate.c'
)

executable(
  'vektor',
  src,
  dependencies: [gtk,epoxy],
  link_args: ['-lm'],
  install: true,
)

