---
title: SCPI_ParamIsNumber()
---

```c
scpi_bool_t
SCPI_ParamIsNumber(
    scpi_parameter_t * parameter,
    scpi_bool_t suffixAllowed);
```

This function is for testing, if parameter is numeric type or not.

If `suffixAllowed` is `TRUE` and number is with suffix e.g. `10.5 V`, it will also return `TRUE`, otherwise, it will return `FALSE` in this situation.

It is possible to use functions [`SCPI_ParamToInt`]({{ site.basedir }}/api/scpi_paramtoint), [`SCPI_ParamToUnsignedInt`]({{ site.basedir }}/api/scpi_paramtounsignedint) or [`SCPI_ParamToDouble`]({{ site.basedir }}/api/scpi_paramtodouble) to convert the parameter into number.

Usage example
---

```c
scpi_bool_t res;
scpi_parameter_t param1;
uint32_t value = 0;

res = SCPI_Parameter(context, &param1, FALSE);

if (res) {
    // Is parameter a number without suffix?
    if (SCPI_ParamIsNumber(&param1, FALSE) {
        // Convert parameter to unsigned int. Result is in value.
        SCPI_ParamToUnsignedInt(context, &param1, &value);
    }
}
```