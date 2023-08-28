libexample_common = static_library('common', 'scpi-def.c',
  include_directories : [ inc, include_directories('.') ],
)

libexample_common_dep = declare_dependency(
  include_directories : [ inc, include_directories('.') ],
  link_with : libexample_common,
)
