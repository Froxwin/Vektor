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

src = files(
  'src/main.c',
  'src/matrix.c',
  'src/ui/uicontroller.c'
)

executable(
  'vektor',
  src,
  dependencies: [gtk],
  link_args: ['-lm'],
  install: true,
)

