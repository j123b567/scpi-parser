---
title: Command callback
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

Specifying `mandatory` parameter will introduce SCPI Error -109 "Missing parameter"

These are the functions, you can use to read parameters
 - `SCPI_Parameter` - read parameter to scpi_parameter_t. This structure contains pointer to buffer and type of the parameter (program mnemonic, hex number, ....)
 - `SCPI_ParamInt` - read signed 32bit integer value (dec or hex with 0x prefix)
 - `SCPI_ParamDouble` - read double value
 - `SCPI_ParamNumber` - read double value with or without units or represented by special number (DEF, MIN, MAX, ...). This function is more universal then SCPI_ParamDouble.
 - `SCPI_ParamCopyText` - read text value - must be encapsuled in ""
 - `SCPI_ParamBool` - read boolean value (0, 1, on, off)
 - `SCPI_ParamChoice` - read value from predefined constants
 

These are the functions, you can use to write results
 - `SCPI_ResultInt` - write integer value
 - `SCPI_ResultDouble` - write double value
 - `SCPI_ResultText` - write text value encapsulated in ""
 - `SCPI_ResultMnemonic` - directly write string value
 - `SCPI_ResultArbitraryBlock` - result arbitrary data
 - `SCPI_ResultIntBase` - write integer in special base
 - `SCPI_ResultBool` - write boolean value

You can use function `SCPI_NumberToStr` to convert number with units to textual representation and then use `SCPI_ResultMnemonic` to write this to the user.

You can use `SCPI_Parameter` in conjuction with `SCPI_ParamIsNumber`, `SCPI_ParamToInt`, `SCPI_ParamToDouble`, `SCPI_ParamToChoice` in your own parameter type handlers.

`SCPI_ParamNumber` is now more universal. It can handle number with units, it can handle special numbers like `DEF`, `INF`, ... These special numbers are now defined in parameter and not in context. It is possible to define more general usage with different special numbers for different commands.
