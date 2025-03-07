vxi11_dep = dependency('vxi11', required: false)
tirpc_dep = dependency('libtirpc', required: false)

if vxi11_dep.found() and tirpc_dep.found()
  executable('test-vxi11',
    [ 'main.c', 'vxi11_xdr.c' ],
    dependencies: [
      libscpi_static_dep,
      libexample_common_dep,
      tirpc_dep, vxi11_dep,
    ]
  )
endif
