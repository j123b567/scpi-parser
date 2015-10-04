---
title: SCPI_ExprChannelListEntry()
category: expressions
---

```c
scpi_expr_result_t
SCPI_ExprChannelListEntry(
    scpi_t * context,
    scpi_parameter_t * param,
    int index,
    scpi_bool_t * isRange,
    int32_t * valuesFrom,
    int32_t * valuesTo,
    size_t length,
    size_t * dimensions);
```
