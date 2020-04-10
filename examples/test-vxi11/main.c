#include "vxi11.h"
#include <memory.h>
#include <netinet/in.h>
#include <rpc/pmap_clnt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "../common/scpi-def.h"
#include "scpi/scpi.h"

#ifndef SIG_PF
#define SIG_PF void (*)(int)
#endif

#define RX_BUF_SIZE 512

/* a global output buffer to collect output data */
#define SCPI_OUPUT_BUFFER_SIZE (256)
char SCPI_outputBuffer[SCPI_OUPUT_BUFFER_SIZE];
unsigned int SCPI_outputBuffer_wr_idx = 0;
unsigned int SCPI_outputBuffer_rd_idx = 0;

char available_bytes = 0;

enum {
    VXI11_CORE_ERROR_NO_ERROR = 0,
    VXI11_CORE_ERROR_SYNTAX_ERROR = 1,
    VXI11_CORE_ERROR_DEVICE_NOT_ACCESSIBLE = 3,
    VXI11_CORE_ERROR_INVALID_ID = 4,
    VXI11_CORE_ERROR_PARAMETER_ERROR = 5,
    VXI11_CORE_ERROR_CHANNEL_NOT_ESTABLISHED = 6,
    VXI11_CORE_ERROR_OPERATION_NOT_SUPPORTED = 8,
    VXI11_CORE_ERROR_OUT_OF_RESOURCES = 9,
    VXI11_CORE_ERROR_DEVICE_LOCKED = 11,
    VXI11_CORE_ERROR_NO_LOCK_HELD = 12,
    VXI11_CORE_ERROR_IO_TIMEOUT = 15,
    VXI11_CORE_ERROR_IO_ERROR = 17,
    VXI11_CORE_ERROR_INVALID_ADDRESS = 21,
    VXI11_CORE_ERROR_ABORT = 23,
    VXI11_CORE_ERROR_CHANNEL_ALREADY_ESTABLISHED = 29
};

enum {
    VXI11_REASON_REQCNT = 1,
    VXI11_REASON_CHR = 2,
    VXI11_REASON_END = 4
};

size_t SCPI_Write(scpi_t* context, const char* data, size_t len)
{
    if (STB_MAV & SCPI_RegGet(context, SCPI_REG_STB)) {
        SCPI_RegClearBits(context, SCPI_REG_STB, STB_MAV);
        SCPI_ErrorPush(context, SCPI_ERROR_QUERY_INTERRUPTED);
        // Restart output buffer index
        SCPI_outputBuffer_wr_idx = 0;
        SCPI_outputBuffer_rd_idx = 0;
    }

    if ((SCPI_outputBuffer_wr_idx + len) > (SCPI_OUPUT_BUFFER_SIZE - 1)) {
        len = (SCPI_OUPUT_BUFFER_SIZE - 1) - SCPI_outputBuffer_wr_idx; /* limit length to left over space */
        /* apparently there is no mechanism to cope with buffers that are too small */
    }
    memcpy(&SCPI_outputBuffer[SCPI_outputBuffer_wr_idx], data, len);
    SCPI_outputBuffer_wr_idx += len;

    SCPI_outputBuffer[SCPI_outputBuffer_wr_idx] = '\0';
    return 0;
}

scpi_result_t SCPI_Flush(scpi_t* context)
{
    SCPI_RegSetBits(context, SCPI_REG_STB, STB_MAV);
    SCPI_outputBuffer_rd_idx = 0;
    return SCPI_RES_OK;
}

int SCPI_Error(scpi_t* context, int_fast16_t err)
{
    (void)context;
    /* BEEP */
    fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int16_t)err, SCPI_ErrorTranslate(err));
    return 0;
}

scpi_result_t SCPI_Control(scpi_t* context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val)
{
    (void)context;

    if (SCPI_CTRL_SRQ == ctrl) {
        fprintf(stderr, "**SRQ: 0x%X (%d)\r\n", val, val);
    } else {
        fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t* context)
{
    (void)context;

    fprintf(stderr, "**Reset\r\n");
    return SCPI_RES_OK;
}

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t* context)
{
    (void)context;

    return SCPI_RES_ERR;
}

bool_t
device_abort_1_svc(Device_Link* argp, Device_Error* result, struct svc_req* rqstp)
{
    bool_t retval;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

int device_async_1_freeresult(SVCXPRT* transp, xdrproc_t xdr_result, caddr_t result)
{
    xdr_free(xdr_result, result);
    return 1;
}

bool_t
create_link_1_svc(Create_LinkParms* argp, Create_LinkResp* result, struct svc_req* rqstp)
{
    result->lid = 0;
    result->maxRecvSize = RX_BUF_SIZE;
    result->abortPort = rqstp->rq_xprt->xp_port;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
device_write_1_svc(Device_WriteParms* argp, Device_WriteResp* result, struct svc_req* rqstp)
{
    SCPI_Input(&scpi_context, argp->data.data_val, argp->data.data_len);
    result->size = argp->data.data_len;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
device_read_1_svc(Device_ReadParms* argp, Device_ReadResp* result, struct svc_req* rqstp)
{
    if (STB_MAV & SCPI_RegGet(&scpi_context, SCPI_REG_STB)) {

        char* data = NULL;

        uint32_t in_buffer = (SCPI_outputBuffer_wr_idx - SCPI_outputBuffer_rd_idx);
        uint32_t to_copy = (argp->requestSize > in_buffer) ? in_buffer : argp->requestSize;

        data = (unsigned char*)malloc(to_copy);
        if (NULL == data) {
            result->error = VXI11_CORE_ERROR_IO_ERROR;
            return 1;
        }

        memcpy(data, &SCPI_outputBuffer[SCPI_outputBuffer_rd_idx], to_copy);
        SCPI_outputBuffer_rd_idx += to_copy;

        result->data.data_val = (char*)data;
        result->data.data_len = (u_int)to_copy;

        result->reason = (to_copy == argp->requestSize) ? VXI11_REASON_REQCNT : 0;

        fprintf(stderr, "in_buffer: %d\n", in_buffer);
        fprintf(stderr, "to_copy: %d\n", to_copy);
        fprintf(stderr, "SCPI_outputBuffer_wr_idx: %d\n", SCPI_outputBuffer_wr_idx);
        fprintf(stderr, "SCPI_outputBuffer_rd_idx: %d\n", SCPI_outputBuffer_rd_idx);

        if (SCPI_outputBuffer_rd_idx == SCPI_outputBuffer_wr_idx) {
            SCPI_RegClearBits(&scpi_context, SCPI_REG_STB, STB_MAV);
            SCPI_outputBuffer_wr_idx = 0;
            SCPI_outputBuffer_rd_idx = 0;
            result->reason |= VXI11_REASON_END;
        }

        result->error = VXI11_CORE_ERROR_NO_ERROR;
    } else {
        result->error = VXI11_CORE_ERROR_IO_TIMEOUT;
    }

    return 1;
}

bool_t
device_readstb_1_svc(Device_GenericParms* argp, Device_ReadStbResp* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)rqstp;
    result->stb = (u_char)SCPI_RegGet(&scpi_context, SCPI_REG_STB);
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
device_trigger_1_svc(Device_GenericParms* argp, Device_Error* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)rqstp;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
device_clear_1_svc(Device_GenericParms* argp, Device_Error* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)rqstp;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
device_remote_1_svc(Device_GenericParms* argp, Device_Error* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)rqstp;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
device_local_1_svc(Device_GenericParms* argp, Device_Error* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)rqstp;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
device_lock_1_svc(Device_LockParms* argp, Device_Error* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)rqstp;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
device_unlock_1_svc(Device_Link* argp, Device_Error* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)rqstp;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
device_enable_srq_1_svc(Device_EnableSrqParms* argp, Device_Error* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)rqstp;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
device_docmd_1_svc(Device_DocmdParms* argp, Device_DocmdResp* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)rqstp;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
destroy_link_1_svc(Device_Link* argp, Device_Error* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)rqstp;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
create_intr_chan_1_svc(Device_RemoteFunc* argp, Device_Error* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)rqstp;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

bool_t
destroy_intr_chan_1_svc(void* argp, Device_Error* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)rqstp;
    result->error = VXI11_CORE_ERROR_NO_ERROR;
    return 1;
}

int device_core_1_freeresult(SVCXPRT* transp, xdrproc_t xdr_result, caddr_t result)
{
    (void)transp;
    xdr_free(xdr_result, result);
    return 1;
}

bool_t
device_intr_srq_1_svc(Device_SrqParms* argp, void* result, struct svc_req* rqstp)
{
    (void)argp;
    (void)result;
    (void)rqstp;
    return 1;
}

int device_intr_1_freeresult(SVCXPRT* transp, xdrproc_t xdr_result, caddr_t result)
{
    (void)transp;
    xdr_free(xdr_result, result);
    return 1;
}

static void
device_async_1(struct svc_req* rqstp, register SVCXPRT* transp)
{
    union {
        Device_Link device_abort_1_arg;
    } argument;
    union {
        Device_Error device_abort_1_res;
    } result;
    bool_t retval;
    xdrproc_t _xdr_argument, _xdr_result;
    bool_t (*local)(char*, void*, struct svc_req*);

    switch (rqstp->rq_proc) {
    case NULLPROC:
        (void)svc_sendreply(transp, (xdrproc_t)xdr_void, (char*)NULL);
        return;

    case device_abort:
        _xdr_argument = (xdrproc_t)xdr_Device_Link;
        _xdr_result = (xdrproc_t)xdr_Device_Error;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_abort_1_svc;
        break;

    default:
        svcerr_noproc(transp);
        return;
    }
    memset((char*)&argument, 0, sizeof(argument));
    if (!svc_getargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument)) {
        svcerr_decode(transp);
        return;
    }
    retval = (bool_t)(*local)((char*)&argument, (void*)&result, rqstp);
    if (retval > 0 && !svc_sendreply(transp, (xdrproc_t)_xdr_result, (char*)&result)) {
        svcerr_systemerr(transp);
    }
    if (!svc_freeargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument)) {
        fprintf(stderr, "%s", "unable to free arguments");
        exit(1);
    }
    if (!device_async_1_freeresult(transp, _xdr_result, (caddr_t)&result))
        fprintf(stderr, "%s", "unable to free results");

    return;
}

static void
device_core_1(struct svc_req* rqstp, register SVCXPRT* transp)
{
    union {
        Create_LinkParms create_link_1_arg;
        Device_WriteParms device_write_1_arg;
        Device_ReadParms device_read_1_arg;
        Device_GenericParms device_readstb_1_arg;
        Device_GenericParms device_trigger_1_arg;
        Device_GenericParms device_clear_1_arg;
        Device_GenericParms device_remote_1_arg;
        Device_GenericParms device_local_1_arg;
        Device_LockParms device_lock_1_arg;
        Device_Link device_unlock_1_arg;
        Device_EnableSrqParms device_enable_srq_1_arg;
        Device_DocmdParms device_docmd_1_arg;
        Device_Link destroy_link_1_arg;
        Device_RemoteFunc create_intr_chan_1_arg;
    } argument;
    union {
        Create_LinkResp create_link_1_res;
        Device_WriteResp device_write_1_res;
        Device_ReadResp device_read_1_res;
        Device_ReadStbResp device_readstb_1_res;
        Device_Error device_trigger_1_res;
        Device_Error device_clear_1_res;
        Device_Error device_remote_1_res;
        Device_Error device_local_1_res;
        Device_Error device_lock_1_res;
        Device_Error device_unlock_1_res;
        Device_Error device_enable_srq_1_res;
        Device_DocmdResp device_docmd_1_res;
        Device_Error destroy_link_1_res;
        Device_Error create_intr_chan_1_res;
        Device_Error destroy_intr_chan_1_res;
    } result;
    bool_t retval;
    xdrproc_t _xdr_argument, _xdr_result;
    bool_t (*local)(char*, void*, struct svc_req*);

    switch (rqstp->rq_proc) {
    case NULLPROC:
        (void)svc_sendreply(transp, (xdrproc_t)xdr_void, (char*)NULL);
        return;

    case create_link:
        _xdr_argument = (xdrproc_t)xdr_Create_LinkParms;
        _xdr_result = (xdrproc_t)xdr_Create_LinkResp;
        local = (bool_t(*)(char*, void*, struct svc_req*))create_link_1_svc;
        break;

    case device_write:
        _xdr_argument = (xdrproc_t)xdr_Device_WriteParms;
        _xdr_result = (xdrproc_t)xdr_Device_WriteResp;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_write_1_svc;
        break;

    case device_read:
        _xdr_argument = (xdrproc_t)xdr_Device_ReadParms;
        _xdr_result = (xdrproc_t)xdr_Device_ReadResp;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_read_1_svc;
        break;

    case device_readstb:
        _xdr_argument = (xdrproc_t)xdr_Device_GenericParms;
        _xdr_result = (xdrproc_t)xdr_Device_ReadStbResp;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_readstb_1_svc;
        break;

    case device_trigger:
        _xdr_argument = (xdrproc_t)xdr_Device_GenericParms;
        _xdr_result = (xdrproc_t)xdr_Device_Error;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_trigger_1_svc;
        break;

    case device_clear:
        _xdr_argument = (xdrproc_t)xdr_Device_GenericParms;
        _xdr_result = (xdrproc_t)xdr_Device_Error;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_clear_1_svc;
        break;

    case device_remote:
        _xdr_argument = (xdrproc_t)xdr_Device_GenericParms;
        _xdr_result = (xdrproc_t)xdr_Device_Error;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_remote_1_svc;
        break;

    case device_local:
        _xdr_argument = (xdrproc_t)xdr_Device_GenericParms;
        _xdr_result = (xdrproc_t)xdr_Device_Error;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_local_1_svc;
        break;

    case device_lock:
        _xdr_argument = (xdrproc_t)xdr_Device_LockParms;
        _xdr_result = (xdrproc_t)xdr_Device_Error;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_lock_1_svc;
        break;

    case device_unlock:
        _xdr_argument = (xdrproc_t)xdr_Device_Link;
        _xdr_result = (xdrproc_t)xdr_Device_Error;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_unlock_1_svc;
        break;

    case device_enable_srq:
        _xdr_argument = (xdrproc_t)xdr_Device_EnableSrqParms;
        _xdr_result = (xdrproc_t)xdr_Device_Error;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_enable_srq_1_svc;
        break;

    case device_docmd:
        _xdr_argument = (xdrproc_t)xdr_Device_DocmdParms;
        _xdr_result = (xdrproc_t)xdr_Device_DocmdResp;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_docmd_1_svc;
        break;

    case destroy_link:
        _xdr_argument = (xdrproc_t)xdr_Device_Link;
        _xdr_result = (xdrproc_t)xdr_Device_Error;
        local = (bool_t(*)(char*, void*, struct svc_req*))destroy_link_1_svc;
        break;

    case create_intr_chan:
        _xdr_argument = (xdrproc_t)xdr_Device_RemoteFunc;
        _xdr_result = (xdrproc_t)xdr_Device_Error;
        local = (bool_t(*)(char*, void*, struct svc_req*))create_intr_chan_1_svc;
        break;

    case destroy_intr_chan:
        _xdr_argument = (xdrproc_t)xdr_void;
        _xdr_result = (xdrproc_t)xdr_Device_Error;
        local = (bool_t(*)(char*, void*, struct svc_req*))destroy_intr_chan_1_svc;
        break;

    default:
        svcerr_noproc(transp);
        return;
    }
    memset((char*)&argument, 0, sizeof(argument));
    if (!svc_getargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument)) {
        svcerr_decode(transp);
        return;
    }
    retval = (bool_t)(*local)((char*)&argument, (void*)&result, rqstp);
    if (retval > 0 && !svc_sendreply(transp, (xdrproc_t)_xdr_result, (char*)&result)) {
        svcerr_systemerr(transp);
    }
    if (!svc_freeargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument)) {
        fprintf(stderr, "%s", "unable to free arguments");
        exit(1);
    }
    if (!device_core_1_freeresult(transp, _xdr_result, (caddr_t)&result))
        fprintf(stderr, "%s", "unable to free results");

    return;
}

static void
device_intr_1(struct svc_req* rqstp, register SVCXPRT* transp)
{
    union {
        Device_SrqParms device_intr_srq_1_arg;
    } argument;
    union {
    } result;
    bool_t retval;
    xdrproc_t _xdr_argument, _xdr_result;
    bool_t (*local)(char*, void*, struct svc_req*);

    switch (rqstp->rq_proc) {
    case NULLPROC:
        (void)svc_sendreply(transp, (xdrproc_t)xdr_void, (char*)NULL);
        return;

    case device_intr_srq:
        _xdr_argument = (xdrproc_t)xdr_Device_SrqParms;
        _xdr_result = (xdrproc_t)xdr_void;
        local = (bool_t(*)(char*, void*, struct svc_req*))device_intr_srq_1_svc;
        break;

    default:
        svcerr_noproc(transp);
        return;
    }
    memset((char*)&argument, 0, sizeof(argument));
    if (!svc_getargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument)) {
        svcerr_decode(transp);
        return;
    }
    retval = (bool_t)(*local)((char*)&argument, (void*)&result, rqstp);
    if (retval > 0 && !svc_sendreply(transp, (xdrproc_t)_xdr_result, (char*)&result)) {
        svcerr_systemerr(transp);
    }
    if (!svc_freeargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument)) {
        fprintf(stderr, "%s", "unable to free arguments");
        exit(1);
    }
    if (!device_intr_1_freeresult(transp, _xdr_result, (caddr_t)&result))
        fprintf(stderr, "%s", "unable to free results");

    return;
}

int main(int argc, char** argv)
{
    register SVCXPRT* transp;

    /* user_context will be pointer to socket */
    scpi_context.user_context = NULL;

    SCPI_Init(&scpi_context,
        scpi_commands,
        &scpi_interface,
        scpi_units_def,
        SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
        scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
        scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

    pmap_unset(DEVICE_ASYNC, DEVICE_ASYNC_VERSION);
    pmap_unset(DEVICE_CORE, DEVICE_CORE_VERSION);
    pmap_unset(DEVICE_INTR, DEVICE_INTR_VERSION);

    transp = svcudp_create(RPC_ANYSOCK);
    if (transp == NULL) {
        fprintf(stderr, "%s", "cannot create udp service.");
        exit(1);
    }
    if (!svc_register(transp, DEVICE_ASYNC, DEVICE_ASYNC_VERSION, device_async_1, IPPROTO_UDP)) {
        fprintf(stderr, "%s", "unable to register (DEVICE_ASYNC, DEVICE_ASYNC_VERSION, udp).");
        exit(1);
    }
    if (!svc_register(transp, DEVICE_CORE, DEVICE_CORE_VERSION, device_core_1, IPPROTO_UDP)) {
        fprintf(stderr, "%s", "unable to register (DEVICE_CORE, DEVICE_CORE_VERSION, udp).");
        exit(1);
    }
    if (!svc_register(transp, DEVICE_INTR, DEVICE_INTR_VERSION, device_intr_1, IPPROTO_UDP)) {
        fprintf(stderr, "%s", "unable to register (DEVICE_INTR, DEVICE_INTR_VERSION, udp).");
        exit(1);
    }

    transp = svctcp_create(RPC_ANYSOCK, 0, 0);
    if (transp == NULL) {
        fprintf(stderr, "%s", "cannot create tcp service.");
        exit(1);
    }
    if (!svc_register(transp, DEVICE_ASYNC, DEVICE_ASYNC_VERSION, device_async_1, IPPROTO_TCP)) {
        fprintf(stderr, "%s", "unable to register (DEVICE_ASYNC, DEVICE_ASYNC_VERSION, tcp).");
        exit(1);
    }
    if (!svc_register(transp, DEVICE_CORE, DEVICE_CORE_VERSION, device_core_1, IPPROTO_TCP)) {
        fprintf(stderr, "%s", "unable to register (DEVICE_CORE, DEVICE_CORE_VERSION, tcp).");
        exit(1);
    }
    if (!svc_register(transp, DEVICE_INTR, DEVICE_INTR_VERSION, device_intr_1, IPPROTO_TCP)) {
        fprintf(stderr, "%s", "unable to register (DEVICE_INTR, DEVICE_INTR_VERSION, tcp).");
        exit(1);
    }

    svc_run();
    fprintf(stderr, "%s", "svc_run returned");
    return 0;
}
