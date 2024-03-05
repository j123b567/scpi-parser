project('scpi-parser', [ 'c', 'cpp' ],
    version : '2.2',
    license : 'BSD',
    meson_version : '>= 0.60',
    default_options : [
      'warning_level=3',
      'buildtype=debugoptimized',
    ]
  )


c_args = []
cpp_args = []

cc = meson.get_compiler('c')

install_data(files(['LICENSE', 'README.md']))

subdir('libscpi')
subdir('examples')
