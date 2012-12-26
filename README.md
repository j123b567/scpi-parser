SCPI parser library
===========

[SCPI](http://en.wikipedia.org/wiki/Standard_Commands_for_Programmable_Instruments) Parser library aims to provide parsing ability of SCPI commands on instrument side. All commands are defined by its patterns eg: "STATus:QUEStionable:EVENt?".

Source codes are published with open source Simplified BSD license.

Command pattern definition
-----------
Command pattern is defined by well known representation from SCPI instruments. Pattern is case insensitive but uses lower and upper case letters to show short and long form of the command.

    Pattern "SYSTem" matches strings "SYST", "syst", "SyStEm", "system", ...

Command pattern is devided by colon ":" to show command hierarchy

    Pattern "SYSTem:VERsion?" mathes strings "SYST:version?", "system:ver?", "SYST:VER?", ...

SCPI standard also uses brackets "[]" to define unnecesery parts of command. This behaviour is not implemented yet.

    Pattern "SYSTem:ERRor[:NEXT]?" should match "SYST:ERR?", "system:err?" and also "system:error:next?", ...

In current implementation, you should write two patterns to implement this behaviour

    Pattern "SYSTem:ERRor?" and "SYSTem:ERRor:NEXT?"


Command callback
-----------
Command callbac is defined as function with context parameter, e.g.:

```c
    int DMM_MeasureVoltageDcQ(scpi_context_t * context)
```

The "Q" at the end of the function name indicates, that this function is Query function (command with "?").

The command callback can use predefined function to parse input parameters and to write output.

Reading input parameter is done by functions `SCPI_ParamInt`, `SCPI_ParamDouble`, `SCPI_ParamString` adn `SCPI_ParamNumber`.

Writing output is done by functions `SCPI_ResultInt`, `SCPI_ResultDouble`, `SCPI_ResultString`, `SCPI_ResultText`. You can write multiple output variables. They are automaticcaly separated by coma ",".

Source code organisation
------------

Source codes are devided into few files to provide better portability to other systems.

- *libscpi/parser.c* - provides the core parser library
- *libscpi/error.c* - provides basic error handling (error queue of the instrument)
- *libscpi/ieee488.c* - provides basic implementation of IEEE488.2 mandatory commands
- *libscpi/minimal.c* - provides basic implementation of SCPI mandatory commands
- *libscpi/utils.c* - provides string handling routines and conversion routines
- *libscpi/units.c* - provides handling of special numners (DEF, MIN, MAX, ...) and units
- *libscpi/fifo.c* - provides basic implementation of error queue FIFO
- *libscpi/debug.c* - provides debug functions

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
	.special_numbers = scpi_special_numbers_def,
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

These are the functions, you can use to read parameters
 - `SCPI_ParamInt` - read signed 32bit integer value (dec or hex with 0x prefix)
 - `SCPI_ParamDouble` - read double value
 - `SCPI_ParamNumber` - read double value with or without units or represented by special number (DEF, MIN, MAX, ...). This function is more universal then SCPI_ParamDouble.
 - `SCPI_ParamText` - read text value - may be encapsuled in ""
 - `SCPI_ParamString` - read unspecified parameter not encapsulated in ""

These are the functions, you can use to write results
 - `SCPI_ResultInt` - write integer value
 - `SCPI_ResultDouble` - write double value
 - `SCPI_ResultText` - write text value encapsulated in ""
 - `SCPI_ResultString` - directly write string value

You can use function `SCPI_NumberToStr` to convert number with units to textual representation and then use `SCPI_ResultString` to write this to the user.