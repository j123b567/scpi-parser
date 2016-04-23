---
title: SCPI_ParamToUInt64()
category: parameters_ex
---

```c
scpi_bool_t
SCPI_ParamToUInt64(
    scpi_t * context,
    scpi_parameter_t * parameter,
    uint64_t * value);
```

Usage example
---

```c
scpi_bool_t res;
scpi_parameter_t param1;
uint64_t value = 0;

res = SCPI_Parameter(context, &param1, FALSE);

if (res) {
    // Is parameter a number without suffix?
    if (SCPI_ParamIsNumber(&param1, FALSE) {
        // Convert parameter to unsigned  int. Result is in value.
        SCPI_ParamToUInt64(context, &param1, &value);
    }
}
```