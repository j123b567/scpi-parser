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

|---------------------+---------------------------------------------|-----------------------|
| Feature             | Pattern example                             | Related API           |
|---------------------+---------------------------------------------|-----------------------|
| Short and long form | `MEASure` means `MEAS` or `MEASURE` command | [`SCPI_Match()`]({{ site.baseurl }}/api/SCPI_Match) |
| Common command      | `*CLS`                                      |                       |
| Compound command    | `CONFigure:VOLTage`                         |                       |
| Query command       | `MEASure:VOLTage?`, `*IDN?`                 |                       |
| Optional keywords   | `MEASure:VOLTage[:DC]?`                     |                       |
| Numeric keyword suffix<br>Multiple identical capabilities | `OUTput#:FREQuency` | [`SCPI_CommandNumbers()`]({{ site.baseurl }}/api/SCPI_CommandNumbers) |
|---------------------+---------------------------------------------|-----------------------|

**Supported parameter types**

|---------------------+----------------------|--------------------|
| Type                | Example              | Related API        |
|---------------------+----------------------|--------------------|
| Decimal             | `10`, `10.5`         | [`SCPI_ParamDouble()`]({{ site.baseurl }}/api/SCPI_ParamDouble), [`SCPI_ParamToDouble()`]({{ site.baseurl }}/api/SCPI_ParamToDouble) |
| Decimal with suffix | `-5.5 V`, `1.5 KOHM` | [`SCPI_ParamNumber()`]({{ site.baseurl }}/api/SCPI_ParamNumber) |
| Hexadecimal         | `#HFF`               | [`SCPI_ParamUInt32()`]({{ site.baseurl }}/api/SCPI_ParamUInt32), [`SCPI_ParamToUInt32()`]({{ site.baseurl }}/api/SCPI_ParamToUInt32) |
| Octal               | `#Q77`               | [`SCPI_ParamUInt32()`]({{ site.baseurl }}/api/SCPI_ParamUInt32), [`SCPI_ParamToUInt32()`]({{ site.baseurl }}/api/SCPI_ParamToUInt32) |
| Binary              | `#B11`               | [`SCPI_ParamUInt32()`]({{ site.baseurl }}/api/SCPI_ParamUInt32), [`SCPI_ParamToUInt32()`]({{ site.baseurl }}/api/SCPI_ParamToUInt32) |
| String              | `"text"`, `'text'`   | [`SCPI_ParamCopyText()`]({{ site.baseurl }}/api/SCPI_ParamCopyText) |
| Arbitrary block     | `#12AB`              | [`SCPI_ParamArbitraryBlock()`]({{ site.baseurl }}/api/SCPI_ParamArbitraryBlock) |
| Program expression  | `(1)`                | [`SCPI_Parameter()`]({{ site.baseurl }}/api/SCPI_Parameter), [`SCPI_ParamCharacters()`]({{ site.baseurl }}/api/SCPI_ParamCharacters) |
| Numeric list        | `(1,2:50,80)`        | [`SCPI_ExprNumericListEntry()`]({{ site.baseurl }}/api/SCPI_ExprNumericListEntry) |
| Channel list        | `(@1!2:3!4,5!6)`     | [`SCPI_ExprChannelListEntry()`]({{ site.baseurl }}/api/SCPI_ExprChannelListEntry) |
| Character data      | `MINimum`, `DEFault`, `INFinity` | [`SCPI_ParamChoice()`]({{ site.baseurl }}/api/SCPI_ParamChoice), [`SCPI_ParamNumber()`]({{ site.baseurl }}/api/SCPI_ParamNumber) |
|---------------------+----------------------|--------------------|

