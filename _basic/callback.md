---
title: Command callback
order: 20
---

Command callback is defined as function with result of type `scpi_result_t` and one parameter - scpi context

```c
	scpi_result_t DMM_MeasureVoltageDcQ(scpi_t * context)
```

Command callback should return `SCPI_RES_OK` if everything goes well.

You can read command parameters and write command results. There are several functions to do this.

Every time, you call function to read parameter, it shifts pointers to the next parameter. You can't read specified parameter directly by its index - e.g. 

```c
	// pseudocode
	param3 = read_param(3); // this is not possible

	read_param();           // discard first parameter
	read_param();           // discard second parameter
	param3 = read_param();  // read third parameter
```

If you discard some parameters, there is no way to recover them.

Specifying `mandatory` parameter will introduce SCPI Error -109 "Missing parameter" if parameter is missing.

These are the functions, you can use to read parameters

* [`SCPI_Parameter`]({{ site.baseurl }}/api/SCPI_Parameter) - read parameter to scpi_parameter_t. This structure contains pointer to buffer and type of the parameter (program mnemonic, hex number, ....)
* [`SCPI_ParamInt`]({{ site.baseurl }}/api/SCPI_ParamInt) - read signed 32bit integer value
* [`SCPI_ParamUnsignedInt`]({{ site.baseurl }}/api/SCPI_ParamUnsignedInt) - read unsigned 32bit integer value
* [`SCPI_ParamDouble`]({{ site.baseurl }}/api/SCPI_ParamDouble) - read double value
* [`SCPI_ParamNumber`]({{ site.baseurl }}/api/SCPI_ParamNumber) - read double value with or without units or represented by special number (DEF, MIN, MAX, ...). This function is more universal then SCPI_ParamDouble.
* [`SCPI_ParamCopyText`]({{ site.baseurl }}/api/SCPI_ParamCopyText) - read text value - must be encapsuled in ""
* [`SCPI_ParamBool`]({{ site.baseurl }}/api/SCPI_ParamBool) - read boolean value (0, 1, on, off)
* [`SCPI_ParamChoice`]({{ site.baseurl }}/api/SCPI_ParamChoice) - read value from predefined constants
* [`SCPI_ParamArbitraryBlock`]({{ site.baseurl }}/api/SCPI_ParamArbitraryBlock) - read arbitrary block
* [`SCPI_ParamCharacters`]({{ site.baseurl }}/api/SCPI_ParamCharacters) - read raw parameter characters
 
 
These are the functions, you can use to write results

* [`SCPI_ResultInt`]({{ site.baseurl }}/api/SCPI_ResultInt) - write integer value
* [`SCPI_ResultIntBase`]({{ site.baseurl }}/api/SCPI_ResultIntBase) - write integer in special base
* [`SCPI_ResultDouble`]({{ site.baseurl }}/api/SCPI_ResultDouble) - write double value
* [`SCPI_ResultText`]({{ site.baseurl }}/api/SCPI_ResultText) - write text value encapsulated in ""
* [`SCPI_ResultCharacters`]({{ site.baseurl }}/api/SCPI_ResultCharacters) - directly write binary data
* [`SCPI_ResultMnemonic`]({{ site.baseurl }}/api/SCPI_ResultMnemonic) - directly write string value
* [`SCPI_ResultArbitraryBlock`]({{ site.baseurl }}/api/SCPI_ResultArbitraryBlock) - result arbitrary data
* [`SCPI_ResultBool`]({{ site.baseurl }}/api/SCPI_ResultBool) - write boolean value

You can use function [`SCPI_NumberToStr`]({{ site.baseurl }}/api/SCPI_NumberToStr) to convert number with units to textual representation and then use [`SCPI_ResultMnemonic`]({{ site.baseurl }}/api/SCPI_ResultMnemonic) to write this to the user.

You can use [`SCPI_Parameter`]({{ site.baseurl }}/api/SCPI_Parameter) in conjuction with [`SCPI_ParamIsNumber`]({{ site.baseurl }}/api/SCPI_ParamIsNumber), [`SCPI_ParamToInt`]({{ site.baseurl }}/api/SCPI_ParamToInt), [`SCPI_ParamToDouble`]({{ site.baseurl }}/api/SCPI_ParamToDouble), [`SCPI_ParamToChoice`]({{ site.baseurl }}/api/SCPI_ParamToChoice) in your own parameter type handlers.

[`SCPI_ParamNumber`]({{ site.baseurl }}/api/SCPI_ParamNumber) is more universal. It can handle number with units, it can handle special numbers like `DEF`, `INF`, ... These special numbers are now defined in parameter and not in context. It is possible to define more general usage with different special numbers for different commands.
