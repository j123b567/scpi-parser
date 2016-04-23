---
layout: default
title: About
---

About SCPI parser
----


[SCPI](http://en.wikipedia.org/wiki/Standard_Commands_for_Programmable_Instruments) Parser library aims to provide parsing ability of SCPI commands on **instrument side**. All commands are defined by its patterns eg: `"STATus:QUEStionable:EVENt?"`.

Source codes are published with open source Simplified BSD license.

SCPI parser library is based on these standards

* [SCPI-99](http://www.ivifoundation.org/docs/scpi-99.pdf)
* [IEEE 488.2-2004](http://dx.doi.org/10.1109/IEEESTD.2004.95390)


**SCPI version compliance**

| SCPI version | v1999.0 |


**Supported command patterns**

|---------------------+---------------------------------------------|
| Feature             | Pattern example                             |
|---------------------+---------------------------------------------|
| Short and long form | `MEASure` means `MEAS` or `MEASURE` command |
| Short and long form | `MEASure` means `MEAS` or `MEASURE` command |
| Common command      | `*CLS`                                      |
| Compound command    | `CONFigure:VOLTage`                         |
| Query command       | `MEASure:VOLTage?`, `*IDN?`                 |
| Optional keywords   | `MEASure:VOLTage[:DC]?`                     |
| Numeric keyword suffix<br>Multiple identical capabilities | `OUTput#:FREQunecy` |
|---------------------+---------------------------------------------|

**Supported parameter types**

|---------------------+----------------------|
| Type                | Example              |
|---------------------+----------------------|
| Decimal             | `10`, `10.5`         |
| Decimal with suffix | `-5.5 V`, `1.5 KOHM` |
| Hexadecimal         | `#HFF`               |
| Octal               | `#Q77`               |
| Binary              | `#B11`               |
| String              | `"text"`, `'text'`   |
| Arbitrary block     | `#12AB`              |
| Program expression  | `(1)`                |
| Numeric list        | `(1,2:50,80)`        |
| Channel list        | `(@1!2:3!4,5!6)`     |
| Character data      | `MINimum`, `DEFault`, `INFinity` |
|---------------------+----------------------|

