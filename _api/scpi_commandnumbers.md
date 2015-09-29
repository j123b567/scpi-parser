---
title: SCPI_CommandNumbers()
category: commands
---

```c
scpi_bool_t
SCPI_CommandNumbers(
    scpi_t * context,
    int32_t * numbers,
    size_t len);
```


This function implements handling of "SCPI-99:6.2.5.2 Multiple Identical Capabilities". In the command pattern, you should specify, where are numbers allowed. In the callback, just call `SCPI_CommandNumbers()` and it will fill provided array by real numbers.

`numbers` is provided array, that will be filled with numbers.

`len` is length of provided array.

Order and count of numbers is always the same for one pattern, even if the number is not typed - e.g. `OUT:FREQ` is the same as `OUT1:FREQ` and result will be array of one element which will be `1`.

There are few examples of command patterns:

- `OUTPut#:FREQuency`
- `SENSe#[:CHANnel]:FUNCtion:RESult:IND?`
- `OUTPut#:MODulation#:FM#`

Usage example
---
Example is extracted from [`scpi-def.c`](https://github.com/j123b567/scpi-parser/blob/master/examples/common/scpi-def.c)

pattern:

~~~~c
{.pattern = "TEST#:NUMbers#", .callback = TEST_Numbers,},
~~~~

callback
~~~~c
static scpi_result_t TEST_Numbers(scpi_t * context) {
    int32_t numbers[2];

    SCPI_CommandNumbers(context, numbers, 2);

    fprintf(stderr, "TEST numbers %d %d\r\n", numbers[0], numbers[1]);

    return SCPI_RES_OK;
}
~~~~

Because there are two `#` in pattern, expected size of `numbers` array is `2` and third parameter of `SCPI_CommandNumbers` is also `2`.

You can use [interactive command line demo](https://github.com/j123b567/scpi-parser/tree/master/examples/test-interactive) and you will get:

~~~~
TEST2:NUM3
> TEST numbers 2 3

TEST:NUM3
> TEST numbers 1 3

TEST:NUM
> TEST numbers 1 1
~~~~

