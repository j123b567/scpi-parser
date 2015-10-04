---
title: SCPI_ParamToChoice()
category: parameters_ex
---

```c
scpi_bool_t
SCPI_ParamToChoice(
    scpi_t * context,
    scpi_parameter_t * parameter,
    const scpi_choice_def_t * options,
    int32_t * value);
```

Usage example
---

```c
scpi_bool_t res;
scpi_parameter_t param;
int32_t value = 0;

scpi_choice_def_t bool_options[] = {
    {"OFF", 0},
    {"ON", 1},
    SCPI_CHOICE_LIST_END /* termination of option list */
};

res = SCPI_Parameter(context, &param, FALSE);

if (res && param.type == SCPI_TOKEN_PROGRAM_MNEMONIC) {
    SCPI_ParamToChoice(context, &param, bool_options, &value);
}
```