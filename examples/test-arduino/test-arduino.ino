#include <scpi-parser.h>

#include "scpi-def.h"

/*
 *
 */

void SerialPrintf(const char *format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);

    Serial.println(buffer);
}


/*
 *
 */

size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    (void) context;
    return Serial.write(data, len);
}

scpi_result_t SCPI_Flush(scpi_t * context) {
    return SCPI_RES_OK;
}

int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;

    SerialPrintf("**ERROR: %d, \"%s\"\r\n", (int16_t) err, SCPI_ErrorTranslate(err));
    return 0;
}

scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    if (SCPI_CTRL_SRQ == ctrl) {
        SerialPrintf("**SRQ: 0x%X (%d)\r\n", val, val);
    } else {
        SerialPrintf("**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t * context) {
    SerialPrintf("**Reset\r\n");
    return SCPI_RES_OK;
}

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context) {
    return SCPI_RES_ERR;
}

/*
 *
 */

void setup() {
    int result;

    Serial.begin(9600);
    while (!Serial);
    Serial.println("serial com ready");

    SCPI_Init(&scpi_context);

#define TEST_SCPI_INPUT(cmd)    result = SCPI_Input(&scpi_context, cmd, strlen(cmd))

    TEST_SCPI_INPUT("*CLS\r\n");
    TEST_SCPI_INPUT("*RST\r\n");
    TEST_SCPI_INPUT("MEAS:volt:DC? 12,50;*OPC\r\n");
    TEST_SCPI_INPUT("*IDN?\r\n");
    TEST_SCPI_INPUT("SYST:VERS?");
    TEST_SCPI_INPUT("\r\n*ID");
    TEST_SCPI_INPUT("N?");
    TEST_SCPI_INPUT(""); // emulate command timeout

    TEST_SCPI_INPUT("*ESE\r\n"); // cause error -109, missing parameter
    TEST_SCPI_INPUT("*ESE #H20\r\n");

    TEST_SCPI_INPUT("*SRE #HFF\r\n");

    TEST_SCPI_INPUT("IDN?\r\n"); // cause error -113, undefined header

    TEST_SCPI_INPUT("SYST:ERR?\r\n");
    TEST_SCPI_INPUT("SYST:ERR?\r\n");
    TEST_SCPI_INPUT("*STB?\r\n");
    TEST_SCPI_INPUT("*ESR?\r\n");
    TEST_SCPI_INPUT("*STB?\r\n");

    TEST_SCPI_INPUT("meas:volt:dc? 0.01 V, Default\r\n");
    TEST_SCPI_INPUT("meas:volt:dc?\r\n");
    TEST_SCPI_INPUT("meas:volt:dc? def, 0.00001\r\n");
    TEST_SCPI_INPUT("meas:volt:dc? 0.00001\r\n");

    TEST_SCPI_INPUT("test:text 'a'\r\n");
    TEST_SCPI_INPUT("test:text 'a a'\r\n");
    TEST_SCPI_INPUT("test:text 'aa a'\r\n");
    TEST_SCPI_INPUT("test:text 'aaa aaaa'\r\n");

	(void)result;
}

void loop() {
  // put your main code here, to run repeatedly:

}
