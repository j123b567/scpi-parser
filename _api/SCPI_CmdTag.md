---
title: SCPI_CmdTag()
category: commands
---

```c
int32_t
SCPI_CmdTag(
    scpi_t * context);
```

Return crrent detected command tag. This can be used to handle more commands by single callback and distinguish between these commands by tag.