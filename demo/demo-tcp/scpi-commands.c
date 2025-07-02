/*
 * SCPI Commands Definition for TCP Demo
 * 
 * This file demonstrates how to define SCPI commands using the SCPI Parser library
 * for a TCP server implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <scpi/scpi.h>
#include "scpi-commands.h"

/* Demo instrument state */
static struct {
    double voltage;
    double current;
    double frequency;
    bool output_enabled;
    char error_message[256];
} instrument_state = {
    .voltage = 0.0,
    .current = 0.0, 
    .frequency = 1000.0,
    .output_enabled = false,
    .error_message = ""
};

/*
 * *IDN? - Identification Query
 */
static scpi_result_t DMM_IdnQ(scpi_t * context) {
    SCPI_ResultMnemonic(context, "DEMO_SCPI_PARSER");
    SCPI_ResultMnemonic(context, "TCP Demo");
    SCPI_ResultMnemonic(context, "SN54321");  
    SCPI_ResultMnemonic(context, "Rev A");
    return SCPI_RES_OK;
}

/*
 * *RST - Reset
 */
static scpi_result_t DMM_Rst(scpi_t * context) {
    (void) context;
    
    printf("TCP Demo instrument reset\n");
    instrument_state.voltage = 0.0;
    instrument_state.current = 0.0;
    instrument_state.frequency = 1000.0;
    instrument_state.output_enabled = false;
    strcpy(instrument_state.error_message, "");
    
    return SCPI_RES_OK;
}

/*
 * *TST? - Self Test Query
 */
static scpi_result_t DMM_TstQ(scpi_t * context) {
    SCPI_ResultInt32(context, 0); // 0 = passed, 1 = failed
    printf("TCP Demo self test completed: PASSED\n");
    return SCPI_RES_OK;
}

/*
 * SYSTem:ERRor[:NEXT]? - Error Query
 */
static scpi_result_t DMM_SystemErrorNextQ(scpi_t * context) {
    if (strlen(instrument_state.error_message) > 0) {
        SCPI_ResultText(context, instrument_state.error_message);
        strcpy(instrument_state.error_message, ""); // Clear after reading
    } else {
        SCPI_ResultText(context, "0,\"No error\"");
    }
    return SCPI_RES_OK;
}

/*
 * MEASure:VOLTage[:DC]? - Measure DC Voltage
 */
static scpi_result_t DMM_MeasureVoltDcQ(scpi_t * context) {
    // Simulate measurement with some variation
    double measured_voltage = instrument_state.voltage + (rand() % 100 - 50) / 1000.0;
    SCPI_ResultDouble(context, measured_voltage);
    printf("TCP Demo measured voltage: %.3f V\n", measured_voltage);
    return SCPI_RES_OK;
}

/*
 * MEASure:CURRent[:DC]? - Measure DC Current  
 */
static scpi_result_t DMM_MeasureCurrDcQ(scpi_t * context) {
    // Simulate measurement with some variation
    double measured_current = instrument_state.current + (rand() % 10 - 5) / 1000.0;
    SCPI_ResultDouble(context, measured_current);
    printf("TCP Demo measured current: %.6f A\n", measured_current);
    return SCPI_RES_OK;
}

/*
 * SOURce:VOLTage[:LEVel][:IMMediate][:AMPlitude] <value>
 */
static scpi_result_t DMM_SourceVoltLevelImmedAmpl(scpi_t * context) {
    double voltage;
    
    if (!SCPI_ParamDouble(context, &voltage, TRUE)) {
        return SCPI_RES_ERR;
    }
    
    if (voltage < -30.0 || voltage > 30.0) {
        strcpy(instrument_state.error_message, "-222,\"Data out of range\"");
        printf("TCP Demo Error: Voltage out of range (±30V)\n");
        return SCPI_RES_ERR;
    }
    
    instrument_state.voltage = voltage;
    printf("TCP Demo set voltage: %.3f V\n", voltage);
    return SCPI_RES_OK;
}

/*
 * SOURce:VOLTage[:LEVel][:IMMediate][:AMPlitude]?
 */
static scpi_result_t DMM_SourceVoltLevelImmedAmplQ(scpi_t * context) {
    SCPI_ResultDouble(context, instrument_state.voltage);
    return SCPI_RES_OK;
}

/*
 * SOURce:CURRent[:LEVel][:IMMediate][:AMPlitude] <value>
 */
static scpi_result_t DMM_SourceCurrLevelImmedAmpl(scpi_t * context) {
    double current;
    
    if (!SCPI_ParamDouble(context, &current, TRUE)) {
        return SCPI_RES_ERR;
    }
    
    if (current < -1.0 || current > 1.0) {
        strcpy(instrument_state.error_message, "-222,\"Data out of range\"");
        printf("TCP Demo Error: Current out of range (±1A)\n");
        return SCPI_RES_ERR;
    }
    
    instrument_state.current = current;
    printf("TCP Demo set current: %.6f A\n", current);
    return SCPI_RES_OK;
}

/*
 * SOURce:CURRent[:LEVel][:IMMediate][:AMPlitude]?
 */
static scpi_result_t DMM_SourceCurrLevelImmedAmplQ(scpi_t * context) {
    SCPI_ResultDouble(context, instrument_state.current);
    return SCPI_RES_OK;
}

/*
 * SOURce:FREQuency[:LEVel][:IMMediate][:AMPlitude] <value>
 */
static scpi_result_t DMM_SourceFreqLevelImmedAmpl(scpi_t * context) {
    double frequency;
    
    if (!SCPI_ParamDouble(context, &frequency, TRUE)) {
        return SCPI_RES_ERR;
    }
    
    if (frequency < 1.0 || frequency > 1000000.0) {
        strcpy(instrument_state.error_message, "-222,\"Data out of range\"");
        printf("TCP Demo Error: Frequency out of range (1Hz - 1MHz)\n");
        return SCPI_RES_ERR;
    }
    
    instrument_state.frequency = frequency;
    printf("TCP Demo set frequency: %.1f Hz\n", frequency);
    return SCPI_RES_OK;
}

/*
 * SOURce:FREQuency[:LEVel][:IMMediate][:AMPlitude]?
 */
static scpi_result_t DMM_SourceFreqLevelImmedAmplQ(scpi_t * context) {
    SCPI_ResultDouble(context, instrument_state.frequency);
    return SCPI_RES_OK;
}

/*
 * OUTPut[:STATe] <boolean>
 */
static scpi_result_t DMM_OutputState(scpi_t * context) {
    scpi_bool_t output_state;
    
    if (!SCPI_ParamBool(context, &output_state, TRUE)) {
        return SCPI_RES_ERR;
    }
    
    instrument_state.output_enabled = output_state;
    printf("TCP Demo output %s\n", output_state ? "ENABLED" : "DISABLED");
    return SCPI_RES_OK;
}

/*
 * OUTPut[:STATe]?
 */
static scpi_result_t DMM_OutputStateQ(scpi_t * context) {
    SCPI_ResultBool(context, instrument_state.output_enabled);
    return SCPI_RES_OK;
}

/*
 * SCPI command tree definition
 */
const scpi_command_t scpi_commands[] = {
    /* IEEE Mandated Commands */
    { .pattern = "*IDN?", .callback = DMM_IdnQ,},
    { .pattern = "*RST", .callback = DMM_Rst,},
    { .pattern = "*TST?", .callback = DMM_TstQ,},

    /* System Commands */
    { .pattern = "SYSTem:ERRor[:NEXT]?", .callback = DMM_SystemErrorNextQ,},

    /* Measurement Commands */
    { .pattern = "MEASure:VOLTage[:DC]?", .callback = DMM_MeasureVoltDcQ,},
    { .pattern = "MEASure:CURRent[:DC]?", .callback = DMM_MeasureCurrDcQ,},

    /* Source Commands - Voltage */
    { .pattern = "SOURce:VOLTage[:LEVel][:IMMediate][:AMPlitude]", .callback = DMM_SourceVoltLevelImmedAmpl,},
    { .pattern = "SOURce:VOLTage[:LEVel][:IMMediate][:AMPlitude]?", .callback = DMM_SourceVoltLevelImmedAmplQ,},
    
    /* Source Commands - Current */
    { .pattern = "SOURce:CURRent[:LEVel][:IMMediate][:AMPlitude]", .callback = DMM_SourceCurrLevelImmedAmpl,},
    { .pattern = "SOURce:CURRent[:LEVel][:IMMediate][:AMPlitude]?", .callback = DMM_SourceCurrLevelImmedAmplQ,},
    
    /* Source Commands - Frequency */
    { .pattern = "SOURce:FREQuency[:LEVel][:IMMediate][:AMPlitude]", .callback = DMM_SourceFreqLevelImmedAmpl,},
    { .pattern = "SOURce:FREQuency[:LEVel][:IMMediate][:AMPlitude]?", .callback = DMM_SourceFreqLevelImmedAmplQ,},

    /* Output Commands */
    { .pattern = "OUTPut[:STATe]", .callback = DMM_OutputState,},
    { .pattern = "OUTPut[:STATe]?", .callback = DMM_OutputStateQ,},

    SCPI_CMD_LIST_END
}; 