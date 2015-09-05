---
title: Basic instrument
---

Look inside `examples/common/scpi-def.c`. Here is a basic instrument implementation. You can take it as a template for future development.

First of all you need to fill structure of SCPI command definitions

```c	
scpi_command_t scpi_commands[] = {
	{ .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
	{ .pattern = "*RST", .callback = SCPI_CoreRst,},
	{ .pattern = "MEASure:VOLTage:DC?", .callback = DMM_MeasureVoltageDcQ,},
	SCPI_CMD_LIST_END
};
```

Than you need to initialize interface callbacks structure. If you don't want to provide some callbacks, just initialize them as `NULL`. `write` callback is mandatory and is used to output data from the library.

```c
scpi_interface_t scpi_interface = {
	.write = myWrite,
	.error = NULL,
	.reset = NULL,
	.srq = NULL,
};
```

Important thing is command buffer. Maximum size is up to you and it should be larger than any possible largest command.

```c
#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
```

The last structure is scpi context used in parser library. So, compose everything together.

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
#define SMALL_BUFFER_LEN 10
char smbuffer[SMALL_BUFFER_LEN];
while(1) {
	fgets(smbuffer, SMALL_BUFFER_LEN, stdin);
	SCPI_Input(&scpi_context, smbuffer, strlen(smbuffer));
}
```
