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
<tr><td>Short and long form<td><code>MEASure</code> means <code>MEAS</code> or <code>MEASURE</code> command</tr>
<tr><td>Common command<td><code>*CLS</code></td>
<tr><td>Compound command<td><code>CONFigure:VOLTage</code><tr>
<tr><td>Query command<td><code>MEASure:VOLTage?</code>, <code>*IDN?</code></tr>
<tr><td>Optional keywords<td><code>MEASure:VOLTage[:DC]?</code></tr>
<tr><td>Numeric keyword suffix<br>Multiple identical capabilities<td><code>OUTput#:FREQunecy</code></tr>
</table>

**Supported parameter types**
<table>
<tr><th>Type<th>Example</tr>
<tr><td>Decimal<td><code>10</code>, <code>10.5</code></tr>
<tr><td>Decimal with suffix<td><code>-5.5 V</code>, <code>1.5 KOHM</code></tr>
<tr><td>Hexadecimal<td><code>#HFF</code></tr>
<tr><td>Octal<td><code>#Q77</code></tr>
<tr><td>Binary<td><code>#B11</code></tr>
<tr><td>String<td><code>"text"</code>, <code>'text'</code></tr>
<tr><td>Arbitrary block<td><code>#12AB</code></tr>
<tr><td>Program expression<td><code>(1)</code></tr>
<tr><td>Numeric list<td><code>(1,2:50,80)</code></tr>
<tr><td>Channel list<td><code>(@1!2:3!4,5!6)</code></tr>
<tr><td>Character data<td><code>MINimum</code>, <code>DEFault</code>, <code>INFinity</code></tr>
</table>


[![travis build](https://travis-ci.org/j123b567/scpi-parser.svg?branch=master)](https://travis-ci.org/j123b567/scpi-parser) [![Coverage Status](https://coveralls.io/repos/j123b567/scpi-parser/badge.svg?branch=master&service=github)](https://coveralls.io/github/j123b567/scpi-parser?branch=master)
