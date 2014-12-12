
#include "fifo_private.h"

/**
 * Initialize fifo
 * @param fifo
 */
void fifo_init(scpi_fifo_t * fifo) {
    fifo->wr = 0;
    fifo->rd = 0;
    fifo->size = FIFO_SIZE;
}

/**
 * Empty fifo
 * @param fifo
 */
void fifo_clear(scpi_fifo_t * fifo) {
    fifo->wr = 0;
    fifo->rd = 0;
}

/**
 * Add element to fifo. If fifo is full, remove last element.
 * @param fifo
 * @param value
 * @return 
 */
scpi_bool_t fifo_add(scpi_fifo_t * fifo, int16_t value) {
    /* FIFO full? */
    if (fifo->wr == ((fifo->rd + fifo->size) % (fifo->size + 1))) {
        fifo_remove(fifo, NULL);
    }

    fifo->data[fifo->wr] = value;
    fifo->wr = (fifo->wr + 1) % (fifo->size + 1);

    return TRUE;
}

/**
 * Remove element form fifo
 * @param fifo
 * @param value
 * @return FALSE - fifo is empty
 */
scpi_bool_t fifo_remove(scpi_fifo_t * fifo, int16_t * value) {
    /* FIFO empty? */
    if (fifo->wr == fifo->rd) {
        return FALSE;
    }

    if(value) {
        *value = fifo->data[fifo->rd];
    }

    fifo->rd = (fifo->rd + 1) % (fifo->size + 1);

    return TRUE;
}

/**
 * Retrive number of elements in fifo
 * @param fifo
 * @param value
 * @return 
 */
scpi_bool_t fifo_count(scpi_fifo_t * fifo, int16_t * value) {
    *value = fifo->wr - fifo->rd;
    if (*value < 0) {
        *value += (fifo->size + 1);
    }
    return TRUE;
}
