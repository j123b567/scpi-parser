libexample_commoncxx = static_library('common', 'scpi-def.cpp',
  include_directories : [ inc, include_directories('.') ],
)

libexample_commoncxx_dep = declare_dependency(
  include_directories : [ inc, include_directories('.') ],
  link_with : libexample_commoncxx,
)
