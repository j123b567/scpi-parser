---
title: SCPI_DoubleToStr()
category: results_helper
---

```c
size_t
SCPI_DoubleToStr(
    double val,
    char * str,
    size_t len);
```

Convert double value `val` to prepared string buffer `str`. This function expects maximal length of the `str` buffer as `len`.