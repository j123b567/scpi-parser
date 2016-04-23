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

|--------------+---------|
| SCPI version | v1999.0 |
|--------------+---------|


**Supported command patterns**
<table>
<tr><th>Feature</th><th>Pattern example</th></tr>
<tr><td>Short and long form</td><td>`MEASure` means `MEAS` or `MEASURE` command</td></tr>
<tr><td>Common command</td><td>`*CLS`</td></tr>
<tr><td>Compound command</td><td>`CONFigure:VOLTage`</td></tr>
<tr><td>Query command</td><td>`MEASure:VOLTage?`, `*IDN?`</td></tr>
<tr><td>Optional keywords</td><td>`MEASure:VOLTage[:DC]?`</td></tr>
<tr><td>Numeric keyword suffix<br>Multiple identical capabilities</td><td>`OUTput#:FREQunecy`</td></tr>
</table>

**Supported parameter types**
<table>
<tr><th>Type</th><th>Example</th></tr>
<tr><td>Decimal</td><td><code>10</code>, <code>10.5</code></td></tr>
<tr><td>Decimal with suffix</td><td><code>-5.5 V</code>, <code>1.5 KOHM</code></td></tr>
<tr><td>Hexadecimal</td><td><code>#HFF</code></td></tr>
<tr><td>Octal</td><td><code>#Q77</code></td></tr>
<tr><td>Binary</td><td><code>#B11</code></td></tr>
<tr><td>String</td><td><code>"text"</code>, <code>'text'</code></td></tr>
<tr><td>Arbitrary block</td><td><code>#12AB</code></td></tr>
<tr><td>Program expression</td><td><code>(1)</code></td></tr>
<tr><td>Numeric list</td><td><code>(1,2:50,80)</code></td></tr>
<tr><td>Channel list</td><td><code>(@1!2:3!4,5!6)</code></td></tr>
<tr><td>Character data</td><td><code>MINimum</code>, <code>DEFault</code>, <code>INFinity</code></td></tr>
</table>

