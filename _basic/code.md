---
layout: page
title: Source code organisation
order: 5
---

Source codes are devided into few files to provide better portability to other systems.

- *libscpi/src/parser.c* - provides the core parser library
- *libscpi/src/lexer.c* - provides identification of keywoards and data types
- *libscpi/src/error.c* - provides basic error handling (error queue of the instrument)
- *libscpi/src/ieee488.c* - provides basic implementation of IEEE488.2 mandatory commands
- *libscpi/src/minimal.c* - provides basic implementation of SCPI mandatory commands
- *libscpi/src/utils.c* - provides string handling routines and conversion routines
- *libscpi/src/units.c* - provides handling of special numners (DEF, MIN, MAX, ...) and units
- *libscpi/src/fifo.c* - provides basic implementation of error queue FIFO
- *libscpi/src/debug.c* - provides debug functions

- *libscpi/test/* - Unit test for the library

- *examples/test-parser* - is the basic non-interactive demo of the parser
- *examples/test-interactive* - is the basic interactive demo of the parser
- *examples/test-tcp* - is the basic interactive tcp server (port 5025)
- *examples/common* - common examples commands