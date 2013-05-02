#ifndef _SCPI_SERVER_H_
#define _SCPI_SERVER_H_


#include <stdint.h>

void scpi_server_init(void);

void SCPI_AddError(int16_t err);
void SCPI_RequestControl(void);


#endif /* _SCPI_SERVER_H_ */
