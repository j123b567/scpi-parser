---
title: SCPI_Init()
category: parser
order: 10
---

```c
void SCPI_Init(scpi_t * context,
    const scpi_command_t * commands,
    scpi_interface_t * interface,
    const scpi_unit_def_t * units,
    const char * idn1, const char * idn2, const char * idn3, const char * idn4,
    char * input_buffer, size_t input_buffer_length,
    scpi_error_t * error_queue_data, int16_t error_queue_size);
```

Function to initialise the parser. All parameters must be prealocated before calling this function. It just registers these values to context.
