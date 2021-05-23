---
title: SCPI_ErrorPop()
category: errors
---

```c
scpi_bool_t
SCPI_ErrorPop(
    scpi_t * context,
    scpi_error_t * error);
```

Remove error from queue and place it in `error`.
Return `TRUE` on success, `FALSE` on failure.
