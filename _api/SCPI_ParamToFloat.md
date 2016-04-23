---
title: SCPI_ParamToFloat()
category: parameters_ex
---

```c
scpi_bool_t
SCPI_ParamToFloat(
    scpi_t * context,
    scpi_parameter_t * parameter,
    float * value);
```

Usage example
---

```c
scpi_bool_t res;
scpi_parameter_t param1;
float value = 0;

res = SCPI_Parameter(context, &param1, FALSE);

if (res) {
    // Is parameter a number without suffix?
    if (SCPI_ParamIsNumber(&param1, FALSE) {
        // Convert parameter to floa. Result is in value.
        SCPI_ParamToFloat(context, &param1, &value);
    }
}
```