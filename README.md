SCPI parser library
===========

[SCPI](http://en.wikipedia.org/wiki/Standard_Commands_for_Programmable_Instruments) Parser library aims to provide parsing ability of SCPI commands on instrument side. All commands are defined by their patterns eg: "STATus:QUEStionable:EVENt?".

Source code is published with open source Simplified BSD license.

Command pattern definition
-----------
A command pattern is defined using the well known command representation from SCPI instruments. A pattern is case insensitive but uses lower and upper case letters to show the short and long form of the command.

    Pattern "SYSTem" matches strings "SYST", "syst", "SyStEm", "system", ...

A command pattern is divided by colon ":" to show command hierarchy

    Pattern "SYSTem:VERsion?" mathes strings "SYST:version?", "system:ver?", "SYST:VER?", ...

The SCPI standard also uses brackets "[]" to define optional parts of a command.

    Pattern "SYSTem:ERRor[:NEXT]?" mathes "SYST:ERR?", "system:err?" and also "system:error:next?", ...


Command callback
-----------
Command callback is defined as a function with a context parameter, e.g.:

```c
    int DMM_MeasureVoltageDcQ(scpi_context_t * context)
```

The "Q" at the end of the function name indicates that this function is a Query function (command with "?").

The command callback can use predefined functions to parse input parameters and to write output.

Reading input parameters is done by using the functions `SCPI_ParamInt`, `SCPI_ParamDouble`, `SCPI_ParamString` and `SCPI_ParamNumber`.

Writing output is done by using the functions `SCPI_ResultInt`, `SCPI_ResultDouble`, `SCPI_ResultString`, `SCPI_ResultText`. You can write multiple output variables. They are automaticcaly separated by comma ",".

Source code organisation
------------

Source codes are divided into a few files to provide better portability to other systems.

- *libscpi/src/parser.c* - provides the core parser library
- *libscpi/src/error.c* - provides basic error handling (error queue of the instrument)
- *libscpi/src/ieee488.c* - provides basic implementation of IEEE488.2 mandatory commands
- *libscpi/src/minimal.c* - provides basic implementation of SCPI mandatory commands
- *libscpi/src/utils.c* - provides string handling routines and conversion routines
- *libscpi/src/units.c* - provides handling of special numners (DEF, MIN, MAX, ...) and units
- *libscpi/src/fifo.c* - provides basic implementation of error queue FIFO
- *libscpi/src/debug.c* - provides debug functions

- *examples/test-parser* - is the basic non-interactive demo of the parser
- *examples/test-interactive* - is the basic interactive demo of the parser
- *examples/test-tcp* - is the basic interactive tcp server (port 5025)
- *examples/common* - common examples commands


Implementation to your instrument
-------------
First of all you need to fill the structure of SCPI command definitions

```c	
scpi_command_t scpi_commands[] = {
	{ .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
	{ .pattern = "*RST", .callback = SCPI_CoreRst,},
	{ .pattern = "MEASure:VOLTage:DC?", .callback = DMM_MeasureVoltageDcQ,},
	SCPI_CMD_LIST_END
};
```

Then you need to initialize the interface callbacks structure. If you don't want to provide some callbacks, just initialize it as `NULL`. The write callback is mandatory and is used to output data from the library.

```c
scpi_interface_t scpi_interface = {
	.write = myWrite,
	.error = NULL,
	.reset = NULL, /* Called from SCPI_CoreRst */
	.test = NULL, /* Called from SCPI_CoreTstQ */
	.control = NULL,
};
```

An important library component is the command buffer. The maximum size is up to you and should be larger than the largest possible command. 

```c
#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
```

The last structure is the scpi context used in the parser library.

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
	.special_numbers = scpi_special_numbers_def,
};
```

All these structures should be global variables of the C file or allocated by function like malloc. It is a common mistake to create these structures inside a function as local variables of this function. This will not work. If you don't know why, you should read something about [function stack.](http://stackoverflow.com/questions/4824342/returning-a-local-variable-from-function-in-c).


Now we are ready to initialize SCPI context. It is possible to use more SCPI contexts and share some configurations (command list, registers, units list, error callback...)

```c
SCPI_Init(&scpi_context);
```

A test implementation of function myWrite, which outputs everything to stdout, could be

```c	
size_t myWrite(scpi_t * context, const char * data, size_t len) {
	(void) context;
	return fwrite(data, 1, len, stdout);
}
```

An interactive demo can beimplemented using this loop

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

Command callback is defined as a function with return value of type `scpi_result_t` and one parameter - scpi context

```c
	scpi_result_t DMM_MeasureVoltageDcQ(scpi_t * context)
```

Command callback should return `SCPI_RES_OK` if everything goes well.

You can read command parameters and write command results. There are several functions to do this.

Every time you call a function to read a command parameter, it shifts pointers to the next parameter. You can't read specified parameter directly by its index - e.g. 

```c
	// pseudocode
	param3 = read_param(3); // this is not possible

	read_param();           // discard first parameter
	read_param();           // discard second parameter
	param3 = read_param();  // read third parameter
```

If you discard some parameters, there is no way to recover them.

These are the functions, you can use to read command parameters
 - `SCPI_ParamInt` - read signed 32bit integer value (dec or hex with 0x prefix)
 - `SCPI_ParamDouble` - read double value
 - `SCPI_ParamNumber` - read double value with or without units or represented by special number (DEF, MIN, MAX, ...). This function is more universal then SCPI_ParamDouble.
 - `SCPI_ParamText` - read text value - may be encapsuled in ""
 - `SCPI_ParamString` - read unspecified parameter not encapsulated in ""
 - `SCPI_ParamBool` - read boolean value (ON, OFF, 0, 1)
 - `SCPI_ParamChoice` - read enumeration value eg. (BUS, IMMediate, EXTernal) defined by parameter

These are the functions, you can use to write command results
 - `SCPI_ResultInt` - write integer value
 - `SCPI_ResultDouble` - write double value
 - `SCPI_ResultText` - write text value encapsulated in ""
 - `SCPI_ResultString` - directly write string value
 - `SCPI_ResultBool` - write boolean value

You can use the function `SCPI_NumberToStr` to convert number with units to textual representation and then use `SCPI_ResultString` to write this to the user.