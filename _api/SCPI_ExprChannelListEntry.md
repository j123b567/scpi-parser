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

It is possible to handle channel list expression e.g. "(@1!2:3!4,5!6)" by this function.

This function can be used to iterate over channel list from index=0 to the end. For every item of the list, there should be two steps. Run function with `NULL` parameers for data
```c
scpi_expr_result_t expr_res = SCPI_ExprChannelListEntry(context, &channel_list_param, index, &is_range, NULL, NULL, 0, &dimensions);
```

If the result is `SCPI_EXPR_OK` we know that this part of expression is valid. `dimensions` is holding number of dimensions of this item and `is_range` is holding, if the item is range or single value.

If the result is `SCPI_EXPR_ERROR` there was an error in the expression and parsing should end. Correct error was already pushed to error queue. All values should be treaed as invalid.

If the result is `SCPI_EXPR_NO_MORE` means end of the valid channel list.

After this step, it is time to run the function again with allocated arrays.

```c
SCPI_ExprChannelListEntry(context, &channel_list_param, index, &is_range, values_from, values_to, available_dimensions, &dimensions);
```

It is possible to do everything in one step and just check if `available_dimensions >= dimensions` to ensure all dimensions ware processed.


Example
---

```c
/**
 * @brief 
 * parses lists
 * channel numbers > 0.
 * no checks yet.
 * valid: (@1), (@3!1:1!3), ...
 * (@1!1:3!2) would be 1!1, 1!2, 2!1, 2!2, 3!1, 3!2.
 * (@3!1:1!3) would be 3!1, 3!2, 3!3, 2!1, 2!2, 2!3, ... 1!3.
 * 
 * @param channel_list channel list, compare to SCPI99 Vol 1 Ch. 8.3.2
 */
static scpi_result_t TEST_Chanlst(scpi_t *context) {
    scpi_parameter_t channel_list_param;
#define maxrow 2    //maximum number of rows
#define maxcol 6    //maximum number of columns
#define maxdim 2    //maximum number of dimensions
    scpi_channel_value_t array[maxrow * maxcol]; //array which holds values in order (2D)
    size_t chanlst_idx; //index for channel list
    size_t arr_idx = 0; //index for array
    size_t n, m = 1; //counters for row (n) and columns (m)

    // get channel list
    if (SCPI_Parameter(context, &channel_list_param, TRUE)) {
        scpi_expr_result_t res;
        scpi_bool_t is_range;
        int32_t values_from[maxdim];
        int32_t values_to[maxdim];
        size_t dimensions;

        bool for_stop_row = false; //true if iteration for rows has to stop
        bool for_stop_col = false; //true if iteration for columns has to stop
        int32_t dir_row = 1; //direction of counter for rows, +/-1
        int32_t dir_col = 1; //direction of counter for columns, +/-1

        // the next statement is valid usage and it gets only real number of dimensions for the first item (index 0)
        if (!SCPI_ExprChannelListEntry(context, &channel_list_param, 0, &is_range, NULL, NULL, 0, &dimensions)) {
            chanlst_idx = 0; //call first index
            arr_idx = 0; //set arr_idx to 0
            do { //if valid, iterate over channel_list_param index while res == valid (do-while cause we have to do it once)
                res = SCPI_ExprChannelListEntry(context, &channel_list_param, chanlst_idx, &is_range, values_from, values_to, 4, &dimensions);
                if (is_range == false) { //still can have multiple dimensions
                    if (dimensions == 1) {
                        //here we have our values
                        //row == values_from[0]
                        //col == 0 (fixed number)
                        //call a function or something
                        array[arr_idx].row = values_from[0];
                        array[arr_idx].col = 0;
                    } else if (dimensions == 2) {
                        //here we have our values
                        //row == values_fom[0]
                        //col == values_from[1]
                        //call a function or something
                        array[arr_idx].row = values_from[0];
                        array[arr_idx].col = values_from[1];
                    } else {
                        return SCPI_RES_ERR;
                        break;
                    }
                    arr_idx++; //inkrement array where we want to save our values to, not neccessary otherwise
                } else if (is_range == true) {
                    if (values_from[0] > values_to[0]) {
                        dir_row = -1; //we have to decrement from values_from
                    } else { //if (values_from[0] < values_to[0])
                        dir_row = +1; //default, we increment from values_from
                    }

                    //iterating over rows, do it once -> set for_stop_row = false
                    //needed if there is channel list index isn't at end yet
                    for_stop_row = false;
                    for (n = values_from[0]; for_stop_row == false; n += dir_row) {
                        //usual case for ranges, 2 dimensions
                        if (dimensions == 2) {
                            if (values_from[1] > values_to[1]) {
                                dir_col = -1;
                            } else if (values_from[1] < values_to[1]) {
                                dir_col = +1;
                            }
                            //iterating over columns, do it at least once -> set for_stop_col = false
                            //needed if there is channel list index isn't at end yet
                            for_stop_col = false;
                            for (m = values_from[1]; for_stop_col == false; m += dir_col) {
                                //here we have our values
                                //row == n
                                //col == m
                                //call a function or something
                                array[arr_idx].row = n;
                                array[arr_idx].col = m;
                                arr_idx++;
                                if (m == (size_t)values_to[1]) {
                                    //endpoint reached, stop column for-loop
                                    for_stop_col = true;
                                }
                            }
                            //special case for range, example: (@2!1)
                        } else if (dimensions == 1) {
                            //here we have values
                            //row == n
                            //col == 0 (fixed number)
                            //call function or sth.
                            array[arr_idx].row = n;
                            array[arr_idx].col = 0;
                            arr_idx++;
                        }
                        if (n == (size_t)values_to[0]) {
                            //endpoint reached, stop row for-loop
                            for_stop_row = true;
                        }
                    }


                } else {
                    return SCPI_RES_ERR;
                    break;
                }
                //increase index
                chanlst_idx++;
            } while (SCPI_EXPR_OK == SCPI_ExprChannelListEntry(context, &channel_list_param, chanlst_idx, &is_range, values_from, values_to, 4, &dimensions));
            //while checks, whether incremented index is valid
        }
        //do something at the end if needed
        array[arr_idx].row = 0;
        array[arr_idx].col = 0;
    }
    
    {
        size_t i;
        fprintf(stderr, "TEST_Chanlst: ");
        for (i = 0; i< arr_idx; i++) {
            fprintf(stderr, "%d!%d, ", array[i].row, array[i].col);
        }
        fprintf(stderr, "\r\n");
    }
    return SCPI_RES_OK;
}
```