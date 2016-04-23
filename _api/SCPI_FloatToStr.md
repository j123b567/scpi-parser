---
title: SCPI_FloatToStr()
category: results
---

```c
size_t
SCPI_FloatToStr(
    float val,
    char * str,
    size_t len);
```

Convert float value `val` to prepared string buffer `str`. This function expects maximal length of the `str` buffer as `len`.