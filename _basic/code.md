---
title: Source code organisation
order: 5
---

Source codes are devided into few files to provide better portability to other systems.

* `libscpi/src/` - main source code directory
  * `parser.c` - provides the core parser library
  * `lexer.c` - provides identification of keywoards and data types
  * `error.c` - provides basic error handling (error queue of the instrument)
  * `ieee488.c` - provides basic implementation of IEEE488.2 mandatory commands
  * `minimal.c` - provides basic implementation of SCPI mandatory commands
  * `utils.c` - provides string handling routines and conversion routines
  * `units.c` - provides handling of special numners (DEF, MIN, MAX, ...) and units
  * `fifo.c` - provides basic implementation of error queue FIFO
  * `debug.c` - provides debug functions
* `libscpi/inc/` - main include directory
  * `scpi/scpi.h` - main include file - only this one should be included
  * `scpi/config.h` - configuration file
* `libscpi/test/` - unit tests using CUnit
* `examples/` - example sources
  * `test-parser/` - is the basic non-interactive demo of the parser
  * `test-interactive/` - is the basic command line interactive demo of the parser
  * `test-tcp/` - is the basic interactive tcp server (port 5025)
  * `test-tcp-srq/` - ist the basic interactive tcp server with SRQ
  * `test-CVI_w_GUI/` - LabWindows/CVI example
  * `test-LwIP-netconn/` - Example for LwIP netconn API
  * `common/scpi-def.c` - Common implementation of instrument

