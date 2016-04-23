---
title: SCPI_NumberToStr()
category: results
---

```c
size_t
SCPI_NumberToStr(
    scpi_t * context,
    const scpi_choice_def_t * special,
    scpi_number_t * value,
    char * str,
    size_t len);
```

This function converts special number to string representation with units. This function is inverse function to [`SCPI_ParamNumber()`]({{ site.baseurl }}/api/SCPI_ParamNumber).