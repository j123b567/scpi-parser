---
title: SCPI_Parameter()
---

```c
scpi_bool_t
SCPI_Parameter(
    scpi_t * context,
    scpi_parameter_t * parameter,
    scpi_bool_t mandatory);
```

This function is for universal processing of parameters. It doesn't do any cnversions. It is used in all `SCPI_ParamXYZ()` functions so for basic usage, you can prefere these functions. If you need some special processing of parameter, which can be for example integer, mnemonic or expression, you need to define your own processing functions. 

Next parameter is stored in `parameter` structure.

If `mandatory` is `TRUE` and parameter is missing, SCPI error -109, "Missing parameter" is also generated for this.

Parameter description
---

`scpi_parameter_t` type has this fields

* `type` is type of the parameter. List of available values are in `scpi_token_type_t`. Relevant values for this functions are
  * `SCPI_TOKEN_HEXNUM`
  * `SCPI_TOKEN_OCTNUM`
  * `SCPI_TOKEN_BINNUM`
  * `SCPI_TOKEN_PROGRAM_MNEMONIC`
  * `SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA`
  * `SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA_WITH_SUFFIX`
  * `SCPI_TOKEN_ARBITRARY_BLOCK_PROGRAM_DATA`
  * `SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA`
  * `SCPI_TOKEN_DOUBLE_QUOTE_PROGRAM_DATA`
  * `SCPI_TOKEN_PROGRAM_EXPRESSION`
* `ptr` is pointer to input buffer where this parameter resists. It is *not* NULL terminated.
* `len` is length of this parameter

It is possible to use functions [`SCPI_ParamIsNumber()`](/api/scpi_paramisnumber), [`SCPI_ParamToInt()`](/api/scpi_paramtoint), [`SCPI_ParamToUnsignedInt()`](/api/scpi_paramtounsignedint), [`SCPI_ParamToDouble()`](/api/scpi_paramtodouble), [`SCPI_ParamToChoice()`](/api/scpi_paramtochoice) for further processing of parameter.

Usage example
---

```c
scpi_bool_t res;
scpi_parameter_t param1;
uint32_t value = 0;

res = SCPI_Parameter(context, &param1, FALSE);

if (res) {
    if (param.type == SCPI_TOKEN_PROGRAM_EXPRESSION) {
        if (strncpy(param.ptr, "(1)", param.len)) {
            value = 1;
        }
    }
}
```