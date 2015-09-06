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