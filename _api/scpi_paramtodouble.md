---
title: SCPI_ParamToDouble()
---

```c
scpi_bool_t
SCPI_ParamToDouble(
    scpi_t * context,
    scpi_parameter_t * parameter,
    double * value);
```

Usage example
---

```c
scpi_bool_t res;
scpi_parameter_t param1;
double value = 0;

res = SCPI_Parameter(context, &param1, FALSE);

if (res) {
    // Is parameter a number without suffix?
    if (SCPI_ParamIsNumber(&param1, FALSE) {
        // Convert parameter to double. Result is in value.
        SCPI_ParamToDouble(context, &param1, &value);
    }
}
```