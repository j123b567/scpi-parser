---
title: SCPI_ParamErrorOccurred()
category: parameters
---

```c
scpi_bool_t
SCPI_ParamErrorOccurred(
    scpi_t * context);
```

This function detects, if there was an error during handling of parameter. This function can detect only one error. You should stop processing of parameters, if result of this function is `TRUE`.


Usage example
---

```c
int32_t value
if (!SCPI_ParamInt(context, &value, FALSE)) {
    if (SCPI_ParamErrorOccurred(context)) {
        // error during processing of parameter
        return SCPI_RES_ERR;   
    }
    // not a real error, i.e. optional parameter is missing, continue    
}
```