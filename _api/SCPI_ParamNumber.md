---
title: SCPI_ParamNumber()
category: parameters
order: 90
---

```c
scpi_bool_t
SCPI_ParamNumber(
    scpi_t * context,
    const scpi_choice_def_t * special,
    scpi_number_t * value,
    scpi_bool_t mandatory);
```

`SCPI_ParamNumber()` reads next parameter and stores its value in `value`. If there is no parameter or type of the parameter is not a number or mnemonic, `FALSE` is returnd.

If `mandatory` is `TRUE` and parameter is missing, SCPI error -109, "Missing parameter" is also generated for this.

If `mandatory` is `FALSE` and return of the function is also `FALSE` we should scheck also result of [`SCPI_ParamErrorOccurred()`]({{ site.baseurl }}/api/SCPI_ParamErrorOccurred). If the result is `FALSE` parameter is just missing.


Value description
---

`scpi_number_t` type has this fields

* `special` means, that the number is not a number, but MIN, MAX, DEFAULT, 
* `tag` is the index to `scpi_special_numbers_def` or other array of definitions and is valid only if `special==TRUE`. For `scpi_special_numbers_def`, there are set of defines `SCPI_NUM_MIN`, `SCPI_NUM_MAX`, ... but you can use your own list with your own values.
* `value` is value in base units and is valid only if `special==FALSE`
* `unit` is unit of `value` if available
* `base` is the base that the number had prior any processing. You can see here, if the user typed the number in base DEC, HEX, BIN ...  

See also
---
[`SCPI_NumberToStr()`]({{ site.baseurl }}/api/SCPI_NumberToStr)

Usage example
---

Simple brightness parameter that should allow values like "0.251", "25.1 PCT" and possibly "25100 PPM". All these example values are equal and there is no need for special handling of all cases. Everything is done in `SCPI_ParamNumber`.
This example also allows special values "UP" and "DOWN".

```c
scpi_bool_t res;
scpi_number_t paramBrightness;
res = SCPI_ParamNumber(context, scpi_special_numbers_def, &paramBrightness, TRUE);
if (!res)
{
    return SCPI_RES_ERR;
}

/* We can easilly implement handling of values "UP" and "DOWN". 
 * If there is no need for special values, just return error with SCPI_ERROR_ILLEGAL_PARAMETER_VALUE. */
if (paramBrightness.special) {
    switch (paramBrightness.tag) {
    case SCPI_NUM_UP: brightnessInc(); break;
    case SCPI_NUM_DOWN: brightnessDec(); break;
    default: 
        SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
        return SCPI_RES_ERR;
    }
} else {
    /* handling of numeric value */
    if (paramBrightness.unit == SCPI_UNIT_NONE || paramBrightness.unit == SCPI_UNIT_UNITLESS ) {
        /* range check of value */
        if (paramBrightness.value < 0.0 || paramBrightness.value > 1.0) {
            SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);
            return SCPI_RES_ERR;
        } else {
            brightnessSet(paramBrightness.value);
        }
    } else {
        SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SUFFIX);
        return SCPI_RES_ERR;
    }
}
return SCPI_RES_OK;
```



