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

    int DMM_MeasureVoltageDcQ(scpi_context_t * context)

The "Q" at the end of the function name indicates, that this function is Query function (command with "?").

The command callback can use predefined function to parse input parameters and to write output.

Reading input parameter is done by functions "SCPI_ParamInt", "SCPI_ParamDouble", "SCPI_ParamString".

Writing output is done by functions "SCPI_ResultInt", "SCPI_ResultDouble", "SCPI_ResultString", "SCPI_ResultText". You can write multiple output variables. They are automaticcaly separated by coma ",".

Source code organisation
------------

Source codes are devided into few files to provide better portability to other systems.

- *scpi.c* - provides the core parser library
- *scpi_error.c* - provides basic error handling (error queue of the instrument)
- *scpi_ieee488.c* - provides basic implementation of IEEE488.2 mandatory commands
- *scpi_minimal.c* - provides basic implementation of SCPI mandatory commands
- *scpi_utils.c* - provides string handling routines and conversion routines

- *test-parser.c* - is the basic non-interactive demo of the parser

Implementation to your instrument
-------------
First of all you need to fill structure of SCPI command definitions
	
	scpi_command_t scpi_commands[] = {
		{ .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
		{ .pattern = "*RST", .callback = SCPI_CoreRst,},
		SCPI_CMD_LIST_END
	};

Than you need to initialize interface callbacks structure. If you don't want to provide some callbacks, just initialize it as NULL. write callback is mandatory and is used to output data from the library.

	scpi_interface_t scpi_interface = {
		.write = myWrite,
		.error = NULL,
		.reset = NULL,
		.test = NULL,
	};

Important thing is command buffer. Maximum size is up to you and it should be larger than any possible largest command. 

	#define SCPI_BUFFER_LENGTH 256
	char myBuffer[SCPI_BUFFER_LENGTH];

	scpi_buffer_t scpi_buffer = {
		.length = SCPI_BUFFER_LENGTH,
		.data = myBuffer,
	};

The last structure is scpi context used in parser library.

	scpi_t scpi_context;

All these structures should be global variables of the c file or allocated by function like malloc. It is common mistake to create these structures inside a function as local variables of this function. This will not work. If you don't know why, you should read something about [function stack.](http://stackoverflow.com/questions/4824342/returning-a-local-variable-from-function-in-c).


Now we are ready to connect everything together

	SCPI_Init(&scpi_context, scpi_commands, &scpi_buffer, &scpi_interface);

Test implementation of function myWrite, which outputs everything to stdout, can be
	
	size_t myWrite(scpi_context_t * context, const char * data, size_t len) {
		(void) context;
		return fwrite(data, 1, len, stdout);
	}

Interactive demo can beimplemented using this loop

	#define SMALL_BUFFER_LEN
	char smbuffer[SMALL_BUFFER_LEN];
	while(1) {
		fgets(smbuffer, SMALL_BUFFER_LEN, stdin);
		SCPI_Input(&scpi_context, smbuffer, strlen(smbuffer));
	}



