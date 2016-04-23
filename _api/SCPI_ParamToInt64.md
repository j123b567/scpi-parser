---
title: SCPI_ParamToInt64()
category: parameters_ex
---

```c
scpi_bool_t
SCPI_ParamToInt64(
    scpi_t * context,
    scpi_parameter_t * parameter,
    int64_t * value);
```

Usage example
---

```c
scpi_bool_t res;
scpi_parameter_t param1;
int64_t value = 0;

res = SCPI_Parameter(context, &param1, FALSE);

if (res) {
    // Is parameter a number without suffix?
    if (SCPI_ParamIsNumber(&param1, FALSE) {
        // Convert parameter to int. Result is in value.
        SCPI_ParamToInt64(context, &param1, &value);
    }
}
```