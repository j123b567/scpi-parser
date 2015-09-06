---
title: SCPI_ParamNumber()
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


Value description
---

`scpi_number_t` type has this fields

* `special` means, that the number is not a number, but MIN, MAX, DEFAULT, 
*  `tag` is the index to `scpi_special_numbers_def` or other array of definitions and is valid only if `special==TRUE`. For `scpi_special_numbers_def`, there are set of defines `SCPI_NUM_MIN`, `SCPI_NUM_MAX`, ... but you can use your own list with your own values.
* `value` is value in base units and is valid only if `special==FALSE`
* `unit` is unit of `value` if available
* `base` is the base that the number had prior any processing. You can see here, if the user typed the number in base DEC, HEX, BIN ...  


Usage example
---

```c
scpi_bool_t res;
scpi_number_t param1;

res = SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE);

// handle default value for the parameter
if (!res || (param1.special && param1.tag==SCPI_NUM_DEF)) {
    param1.special = FALSE;
    param1.value = 23.42;
    param1.unit = SCPI_UNIT_VOLT;
}
// continue with regular processing of param1
double myvoltage = param1.value;
```



