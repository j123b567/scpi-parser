#  'scpi.g'
sources = files([
  'error.c',
  'expression.c',
  'fifo.c',
  'ieee488.c',
  'lexer.c',
  'minimal.c',
  'parser.c',
  'units.c',
  'utils.c',
])

inc_private = include_directories( '.' )

libscpi = both_libraries('scpi',
  sources,
  soversion: meson.project_version(),
  include_directories: [ inc, inc_private ],
  install: true,
)

libscpi_dynamic_dep = declare_dependency(
  include_directories: inc,
  link_with: libscpi.get_shared_lib(),
)

libscpi_static_dep = declare_dependency(
  include_directories: inc,
  link_with: libscpi.get_static_lib(),
)

pkg = import('pkgconfig')
pkg.generate(libscpi,
  name: 'libscpi',
  description : 'SCPI parser library',
)
