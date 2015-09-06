SCPI parser library v2
===========

[Documentation](http://j123b567.github.io/scpi-parser)
--------

About
--------

[SCPI](http://en.wikipedia.org/wiki/Standard_Commands_for_Programmable_Instruments) Parser library aims to provide parsing ability of SCPI commands on **instrument side**. All commands are defined by its patterns eg: `"STATus:QUEStionable:EVENt?"`.

Source codes are published with open source Simplified BSD license.

SCPI parser library is based on these standards

* [SCPI-99](http://www.ivifoundation.org/docs/scpi-99.pdf)
* [IEEE 488.2-2004](http://dx.doi.org/10.1109/IEEESTD.2004.95390)


**SCPI version compliance**
<table>
<tr><td>SCPI version<td>v1999.0</tr>
</table>


**Supported command patterns**
<table>
<tr><th>Feature<th>Pattern example</tr>
<tr><td>Short and long form<td>`MEASure` means `MEAS` or `MEASURE` command</tr>
<tr><td>Common command<td>`*CLS`</td>
<tr><td>Compound command<td>`CONFigure:VOLTage`<tr>
<tr><td>Query command<td>`MEASure:VOLTage?`, `*IDN?`</tr>
<tr><td>Optional keywords<td>`MEASure:VOLTage[:DC]?`</tr>
<tr><td>Numeric keyword suffix<br>Multiple identical capabilities<td>`OUTput#:FREQunecy`</tr>
</table>

**Supported parameter types**
<table>
<tr><th>Type<th>Example</tr>
<tr><td>Decimal<td>`10`, `10.5`</tr>
<tr><td>Decimal with suffix<td>`-5.5 V`, `1.5 KOHM`</tr>
<tr><td>Hexadecimal<td>`#HFF`</tr>
<tr><td>Octal<td>`#Q77`</tr>
<tr><td>Binary<td>`#B11`</tr>
<tr><td>String<td>`"text"`, `'text'`</tr>
<tr><td>Arbitrary block<td>`#12AB`</tr>
<tr><td>Program expression<td>`(1)`</tr>
<tr><td>Character data<td>`MINimum`, `DEFault`, `INFinity`</tr>
</table>


[![travis build](https://travis-ci.org/j123b567/scpi-parser.svg?branch=master)](https://travis-ci.org/j123b567/scpi-parser)
