---
title: Migration from v1 to v2
order: 30
---

Functions [`SCPI_ParamNumber`]({{ site.baseurl }}/api/SCPI_ParamNumber) and [`SCPI_NumberToStr`]({{ site.baseurl }}/api/SCPI_NumberToStr) has one more parameter - array of special values. It is still possible to use provided `scpi_special_numbers_def`, but you are free to use different deffinition per parameter.

Function [`SCPI_ParamChoice`]({{ site.baseurl }}/api/SCPI_ParamChoice) has different type of choice list. It is now array of `scpi_choice_def_t` and not array of `const char *`. It is now possible to define `tag`. [`SCPI_ParamChoice`]({{ site.baseurl }}/api/SCPI_ParamChoice) will now return value of this tag and not index to array. If you want print the name of the choice value, you should now use [`SCPI_ChoiceToName`]({{ site.baseurl }}/api/SCPI_ChoiceToName) instead of direct indexing of the array.

Function `SCPI_ParamText` has been replaced by [`SCPI_ParamCharacters`]({{ site.baseurl }}/api/SCPI_ParamCharacters). This function just returns pointer to raw buffer with text. If you need propper handling of quotes inside text, you should use [`SCPI_ParamCopyText`]({{ site.baseurl }}/api/SCPI_ParamCopyText). It needs secondary buffer but it handle strings correctly like `"normal ""quoted"" normal"` will be converted to `normal "quoted" normal`.

It is now possible to use [`SCPI_ParamArbitraryBlock`]({{ site.baseurl }}/api/SCPI_ParamArbitraryBlock) and [`SCPI_ResultArbitraryBlock`]({{ site.baseurl }}/api/SCPI_ResultArbitraryBlock) to work with binary data input and output.

Test callback is removed from context. You should now reimplement whole `*TST?` command callback.