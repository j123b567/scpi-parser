---
title: SCPI_Input()
category: parser
---

```c
int
SCPI_Input(
    scpi_t * context,
    const char * data,
    int len)
```

This function feeds the parser with chunks of input data. Empty length mean end of message/message timeout. There is no need to handle end of message. This function can do it for you.

It is computionally intensive function. It can work to feed it character by character but it is not optimal. This function runs lexical analysis to search for the end of the message.

See also
---
[`SCPI_Parse()`]({{ site.baseurl }}/api/SCPI_Parse)
