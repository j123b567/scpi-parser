---
title: Version history
layout: page
order: 100
---

Version v2.1 2018-08-24
---

* LabWindows/CVI example
* Resolve bug with empty string


Version v2.0 2015-06-24
---

* better AVR support
* implementation of traversal of header tree - it is possible to write `CONF:VOLT:DC 1; AC 2` instead of `CONF:VOLT:DC 1; CONF:VOLT:AC 2`
* resolve issue with multiple commands with result separated by `;`
* add support for Multiple Identical Capabilities - e.g. `OUT:FREQ`, `OUT2:FREQ` by pattern `OUTput#:FREQuency`

Version v2.0_beta1 2015-01-18
---

* parsing more compliant with SCPI-1999
* support all parameter types defined in the spec - separate them and identifie them
* support for Arbitrary program data
* support for tagging command patterns (useful for common handler)
* support for extending any parameter type using SCPI_Parameter
* general support for number or text value (e.g. 1, 6, DEF, INF) not limited to one array of special numbers
* support for optional command headers (Richard.hmm)


Version v1.0 released 2013-06-24
---

* support basic command pattern matching (no optional keywoards)
* support basic data types (no expressions, no nondecimal numbers, no arbitrary program data, ...)
* last version before refactoring of the parser and before extending parameter handling

