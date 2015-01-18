SCPI parser library
===========

[SCPI](http://en.wikipedia.org/wiki/Standard_Commands_for_Programmable_Instruments) Parser library aims to provide parsing ability of SCPI commands on instrument side. All commands are defined by its patterns eg: "STATus:QUEStionable:EVENt?".

Source codes are published with open source Simplified BSD license.


Usage
---------------
Download source package or clone repository
 - v2.0_beta1 - https://github.com/j123b567/scpi-parser/archive/v2.0_beta1.zip
 - v1.2 - https://github.com/j123b567/scpi-parser/archive/v1.2.zip
 - v1.1 - https://github.com/j123b567/scpi-parser/archive/v1.1.zip
 - v1.0 - https://github.com/j123b567/scpi-parser/archive/v1.0.zip
 - git clone https://github.com/j123b567/scpi-parser.git

Go to the `libscpi` directory

Build library, results are in `dist` folder

	make

You can test library functions by

	make test

You can use any of the examples in the `examples` directory


Version history
----------------
Version v2.0_beta1 2015-01-18
 - parsing more compliant with SCPI-1999
 - support all parameter types defined in the spec - separate them and identifie them
 - support for Arbitrary program data
 - support for tagging command patterns (useful for common handler)
 - support for extending any parameter type using SCPI_Parameter
 - general support for number or text value (e.g. 1, 6, DEF, INF) not limited to one array of special numbers
 - support for optional command headers (Richard.hmm)


Version v1.0 released 2013-06-24
 - support basic command pattern matching (no optional keywoards)
 - support basic data types (no expressions, no nondecimal numbers, no arbitrary program data, ...)
 - last version before refactoring of the parser and before extending parameter handling


Command pattern definition
-----------
Command pattern is defined by well known representation from SCPI instruments. Pattern is case insensitive but uses lower and upper case letters to show short and long form of the command.

    Pattern "SYSTem" matches strings "SYST", "syst", "SyStEm", "system", ...

Command pattern is devided by colon ":" to show command hierarchy

    Pattern "SYSTem:VERsion?" mathes strings "SYST:version?", "system:ver?", "SYST:VER?", ...

SCPI standard also uses brackets "[]" to define unnecesery parts of command.

    Pattern "SYSTem:ERRor[:NEXT]?" matches strings  "SYST:ERR?", "system:err?" and also "system:error:next?", ...


Command callback
-----------
Command callbac is defined as function with context parameter, e.g.:

```c
    int DMM_MeasureVoltageDcQ(scpi_context_t * context)
```

The "Q" at the end of the function name indicates, that this function is Query function (command with "?").

The command callback can use predefined function to parse input parameters and to write output.

Reading input parameter is done by functions `SCPI_ParamInt`, `SCPI_ParamDouble`, `SCPI_ParamString`, `SCPI_ParamNumber`, `SCPI_ParamArbitraryBlock`, `SCPI_ParamCopyText`, `SCPI_ParamBool` and `SCPI_ParamChoice`

Writing output is done by functions `SCPI_ResultInt`, `SCPI_ResultDouble`, `SCPI_ResultString`, `SCPI_ResultText`. You can write multiple output variables. They are automaticcaly separated by coma ",".

Source code organisation
------------

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


Implementation to your instrument
-------------
First of all you need to fill structure of SCPI command definitions

```c	
scpi_command_t scpi_commands[] = {
	{ .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
	{ .pattern = "*RST", .callback = SCPI_CoreRst,},
	{ .pattern = "MEASure:VOLTage:DC?", .callback = DMM_MeasureVoltageDcQ,},
	SCPI_CMD_LIST_END
};
```

Than you need to initialize interface callbacks structure. If you don't want to provide some callbacks, just initialize it as `NULL`. write callback is mandatory and is used to output data from the library.

```c
scpi_interface_t scpi_interface = {
	.write = myWrite,
	.error = NULL,
	.reset = NULL,
	.test = NULL,
	.srq = NULL,
};
```

Important thing is command buffer. Maximum size is up to you and it should be larger than any possible largest command. 

```c
#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
```

The last structure is scpi context used in parser library.

```c
scpi_t scpi_context = {
	.cmdlist = scpi_commands,
	.buffer = {
		.length = SCPI_INPUT_BUFFER_LENGTH,
		.data = scpi_input_buffer,
	},
	.interface = &scpi_interface,
	.registers = scpi_regs,
	.units = scpi_units_def,
};
```

All these structures should be global variables of the c file or allocated by function like malloc. It is common mistake to create these structures inside a function as local variables of this function. This will not work. If you don't know why, you should read something about [function stack.](http://stackoverflow.com/questions/4824342/returning-a-local-variable-from-function-in-c).


Now we are ready to initialize SCPI context. It is possible to use more SCPI contexts and share some configurations (command list, registers, units list, error callback...)

```c
SCPI_Init(&scpi_context);
```

Test implementation of function myWrite, which outputs everything to stdout, can be

```c	
size_t myWrite(scpi_context_t * context, const char * data, size_t len) {
	(void) context;
	return fwrite(data, 1, len, stdout);
}
```

Interactive demo can beimplemented using this loop

```c
#define SMALL_BUFFER_LEN
char smbuffer[SMALL_BUFFER_LEN];
while(1) {
	fgets(smbuffer, SMALL_BUFFER_LEN, stdin);
	SCPI_Input(&scpi_context, smbuffer, strlen(smbuffer));
}
```


Implementation of command callback
-------------

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

`SCPI_ParamNumber` is now more universal. It can handle number with units, it can handle special numbers like `DEF`, `INF`, ... These special numbers are now defined in parameter and not in context. It is possible to define more general usage with different special numbers for different commands,
