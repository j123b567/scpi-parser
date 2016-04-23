---
title: Basic instrument
order: 10
---

Look inside [`examples/common/scpi-def.c`](https://github.com/j123b567/scpi-parser/blob/master/examples/common/scpi-def.c). Here is a basic instrument implementation. You can take it as a template for future development.

Command patterns and callbacks
---

First of all you need to fill structure of SCPI command definitions

```c	
scpi_command_t scpi_commands[] = {
	{ .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
	{ .pattern = "*RST", .callback = SCPI_CoreRst,},
	{ .pattern = "MEASure:VOLTage:DC?", .callback = DMM_MeasureVoltageDcQ,},
	SCPI_CMD_LIST_END
};
```

Output callbacks
---

Than you need to initialize interface callbacks structure. If you don't want to provide some callbacks, just initialize them as `NULL`. `write` callback is mandatory and is used to output data from the library.

```c
scpi_interface_t scpi_interface = {
	.write = myWrite,
	.error = NULL,
	.reset = NULL,
	.srq = NULL,
};
```

Test implementation of function myWrite, which outputs everything to stdout, can be

```c    
size_t myWrite(scpi_context_t * context, const char * data, size_t len) {
    (void) context;
    return fwrite(data, 1, len, stdout);
}
```

Handling input
---

In your code, you should feed [`SCPI_Input`]({{ site.baseurl }}/api/SCPI_Input) function. Input command does not need to be complete in one call of this function. It is designed to buffer full command and after it is complete it calls parser.

You can force parsing of buffered command by calling this function with zero length.

If you have another buffering function or you always have full command in the buffer, you don't need to use intermediate buffer and you can directly call [`SCPI_Parse`]({{ site.baseurl }}/api/SCPI_Parse) with your application specific buffer

If you need to use build in buffering, you need to specify input buffer. Maximum size is up to you and it should be larger than any possible largest command.

```c
#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
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

There is also need to allocate error queue. This can be done by
```c
#define SCPI_ERROR_QUEUE_SIZE 17
int16_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];
```

SCPI parser context
---
The last structure is scpi context used in parser library. So, compose everything together by init function. Before any usage of the library, initialization must be performed by [`SCPI_Init`]({{ site.baseurl }}/api/SCPI_Init).

```c
scpi_t scpi_context;
SCPI_Init(&scpi_context,
    scpi_commands,
    &scpi_interface,
    scpi_units_def,
    SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
    scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
    scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);
```

All these structures should be global variables of the c file or allocated by function like `malloc`. It is common mistake to create these structures inside a function as local variables of this function. This will not work. If you don't know why, you should read something about [function stack.](http://stackoverflow.com/questions/4824342/returning-a-local-variable-from-function-in-c).

It is possible to use more SCPI contexts and share some configurations (command list, registers, units list, error callback...), but be aware that the library is not thread safe.

