
#include "fifo_private.h"

/**
 * Initialize fifo
 * @param fifo
 */
void fifo_init(scpi_fifo_t * fifo, scpi_error_t * data, int16_t size) {
    fifo->wr = 0;
    fifo->rd = 0;
    fifo->count = 0;
    fifo->data = data;
    fifo->size = size;
}

/**
 * Empty fifo
 * @param fifo
 */
void fifo_clear(scpi_fifo_t * fifo) {
    fifo->wr = 0;
    fifo->rd = 0;
    fifo->count = 0;
}

/**
 * Test if fifo is empty
 * @param fifo
 * @return
 */
scpi_bool_t fifo_is_empty(scpi_fifo_t * fifo) {
    return fifo->count == 0;
}

/**
 * Test if fifo is full
 * @param fifo
 * @return
 */
scpi_bool_t fifo_is_full(scpi_fifo_t * fifo) {
    return fifo->count == fifo->size;
}

/**
 * Add element to fifo. If fifo is full, return FALSE.
 * @param fifo
 * @param err
 * @param info
 * @return
 */
scpi_bool_t fifo_add(scpi_fifo_t * fifo, const scpi_error_t * value) {
    /* FIFO full? */
    if (fifo_is_full(fifo)) {
        return FALSE;
    }
    if (!value) {
        return FALSE;
    }

    fifo->data[fifo->wr] = *value;
    fifo->wr = (fifo->wr + 1) % (fifo->size);
    fifo->count += 1;
    return TRUE;
}

/**
 * Remove element form fifo
 * @param fifo
 * @param value
 * @return FALSE - fifo is empty
 */
scpi_bool_t fifo_remove(scpi_fifo_t * fifo, scpi_error_t * value) {
    /* FIFO empty? */
    if (fifo_is_empty(fifo)) {
        return FALSE;
    }

    if (value) {
        *value = fifo->data[fifo->rd];
    }

    fifo->rd = (fifo->rd + 1) % (fifo->size);
    fifo->count -= 1;

    return TRUE;
}

/**
 * Remove last element from fifo
 * @param fifo
 * @param value
 * @return FALSE - fifo is empty
 */
scpi_bool_t fifo_remove_last(scpi_fifo_t * fifo, scpi_error_t * value) {
    /* FIFO empty? */
    if (fifo_is_empty(fifo)) {
        return FALSE;
    }

    fifo->wr = (fifo->wr + fifo->size - 1) % (fifo->size);

    if (value) {
        *value = fifo->data[fifo->wr];
    }
    fifo->count -= 1;

    return TRUE;
}

/**
 * Retrive number of elements in fifo
 * @param fifo
 * @param value
 * @return
 */
scpi_bool_t fifo_count(scpi_fifo_t * fifo, int16_t * value) {
    *value = fifo->count;
    return TRUE;
}
