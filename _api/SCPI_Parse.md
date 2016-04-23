---
title: SCPI_Parse()
category: parser
---

```c
int
SCPI_Parse(
    scpi_t * context,
    char * data,
    int len);
```

This function handles input buffer and expects full message. If it is not true, error occur.

See also
---
[`SCPI_Input()`]({{ site.baseurl }}/api/SCPI_Input)