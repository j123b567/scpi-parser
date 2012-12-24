#ifndef __SCPI_DEF_H_
#define __SCPI_DEF_H_

#include "scpi/scpi.h"

extern scpi_t scpi_context;

size_t SCPI_Write(scpi_t * context, const char * data, size_t len);
int SCPI_Error(scpi_t * context, int_fast16_t err);
scpi_result_t SCPI_Srq(scpi_t * context);
scpi_result_t SCPI_Reset(scpi_t * context);
scpi_result_t SCPI_Test(scpi_t * context);




#endif // __SCPI_DEF_H_

