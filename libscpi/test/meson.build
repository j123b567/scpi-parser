
cunit_dep = dependency('cunit', required: false)
if cunit_dep.found()
  deps = [ libscpi_static_dep, cunit_dep ]

  test('fifo',
    executable('fifo', 'test_fifo.c', dependencies: deps)
  )

  test('lexer_parser',
    executable('lexer_parser', 'test_lexer_parser.c', dependencies: deps)
  )

  test('parser',
    executable('parser', 'test_parser.c', dependencies: deps)
  )
 
  test('scpi_utils',
    executable('scpi_utils', 'test_scpi_utils.c', dependencies: deps)
  )
endif
