/**
 * @file         cus_uart.c
 * @brief
 * @author       HT
 * @date         2019/2/13
 * @version      V1.0.0
 * @par Copyright (c):
 *               SIMCom Co.Ltd 2003-2019
 * @par History: 1:Create
 *
 */

#include "simcom_uart.h"
#include "simcom_common.h"
#include "simcom_debug.h"
#include "simcom_api.h"
#include "string.h"

#define UART_RX_BUFFER_SIZE 128 // RX buffer can not more than 2048
#define UART_TX_BUFFER_SIZE 128

#ifdef SIMCOM_UI_DEMO
INT8 state = 0;
extern sMsgQRef simcomUI_msgq;
void sendMsgToUIDemo(SIM_MSG_T UartMsg)
{
    SC_STATUS status = SC_SUCCESS;
    SIM_MSG_T optionMsg = {0, 0, 0, NULL};
    optionMsg.msg_id = UartMsg.msg_id;
    optionMsg.arg2 = UartMsg.arg2;

    optionMsg.arg3 = sAPI_Malloc(optionMsg.arg2 + 1);
    memset(optionMsg.arg3, 0, optionMsg.arg2 + 1);
    memcpy(optionMsg.arg3, UartMsg.arg3, UartMsg.arg2);

    status = sAPI_MsgQSend(simcomUI_msgq, &optionMsg);
    if (status != SC_SUCCESS)
    {
        sAPI_Free(optionMsg.arg3);
        sAPI_Debug("send msg error,status = [%d]", status);
    }
}
#endif

void UartCBFunc(SC_Uart_Port_Number portNumber, void *para)
{
    int readLen = 0;
    char *uartData = sAPI_Malloc(UART_RX_BUFFER_SIZE);
    SIM_MSG_T uartMsg = {0, 0, 0, NULL};

    readLen = sAPI_UartRead(portNumber, (UINT8 *)uartData, UART_RX_BUFFER_SIZE);
    // sAPI_Debug("%s, portNumber is %d, readLen[%d].", __func__, portNumber, readLen);

#ifdef SIMCOM_UI_DEMO_TO_UART1_PORT
    uartMsg.msg_id = SRV_UART;
    uartMsg.arg2 = readLen;
    uartMsg.arg3 = uartData;
    sendMsgToUIDemo(uartMsg);
#endif

    sAPI_Free((void *)uartData);
    return;
}

void UartCBFuncEx(SC_Uart_Port_Number portNumber, int len, void *para)
{
    int readLen = 0;
    char *uartData = sAPI_Malloc(len);
    SIM_MSG_T uartMsg = {0, 0, 0, NULL};

    readLen = sAPI_UartRead(portNumber, (UINT8 *)uartData, len);
    sAPI_Debug("%s, portNumber is %d, readLen[%d].", __func__, portNumber, readLen);

#ifdef SIMCOM_UI_DEMO_TO_UART1_PORT
    uartMsg.msg_id = SRV_UART;
    uartMsg.arg2 = readLen;
    uartMsg.arg3 = uartData;
    sendMsgToUIDemo(uartMsg);
#endif

    sAPI_Free((void *)uartData);

    return;
}

void Uart2CBFunc(SC_Uart_Port_Number portNumber, void *para)
{
    int readLen = 0;
    UINT8 *uart2data = sAPI_Malloc(UART_RX_BUFFER_SIZE);

    readLen = sAPI_UartRead(portNumber, uart2data, UART_RX_BUFFER_SIZE);
    sAPI_Debug("%s, portNumber is %d, readLen[%d].", __func__, portNumber, readLen);

    sAPI_Free((void *)uart2data);
    return;
}

void Uart2CBFuncEx(SC_Uart_Port_Number portNumber, int len, void *para)
{
    int readLen = 0;
    UINT8 *uart2data = sAPI_Malloc(len);

    readLen = sAPI_UartRead(portNumber, uart2data, len);
    sAPI_Debug("%s, portNumber is %d, readLen[%d].", __func__, portNumber, readLen);

    sAPI_Free((void *)uart2data);
    return;
}

void Uart3CBFunc(SC_Uart_Port_Number portNumber, void *reserve)
{
    int readLen = 0;
    UINT8 *uart3data = sAPI_Malloc(UART_RX_BUFFER_SIZE);

    readLen = sAPI_UartRead(portNumber, uart3data, UART_RX_BUFFER_SIZE);
    sAPI_Debug("%s, portNumber is %d, readlen[%d].", __func__, portNumber, readLen);

    sAPI_Free((void *)uart3data);
    return;
}

void Uart3CBFuncEx(SC_Uart_Port_Number portNumber, int len, void *reserve)
{
    int readLen = 0;
    UINT8 *uart3data = sAPI_Malloc(len);

    readLen = sAPI_UartRead(portNumber, uart3data, len);
    sAPI_Debug("%s, portNumber is %d, readlen[%d].", __func__, portNumber, readLen);

    sAPI_Free((void *)uart3data);
    return;
}

void sAPP_UartTask(void)
{
    SCuartConfiguration uartConfig, uart2Config, uart3Config;

    /*************************Configure UART again*********************************/
    /*******The user can modify the initialization configuratin of UART in here.***/
    /******************************************************************************/
    uartConfig.BaudRate = SC_UART_BAUD_115200;
    uartConfig.DataBits = SC_UART_WORD_LEN_8;
    uartConfig.ParityBit = SC_UART_NO_PARITY_BITS;
    uartConfig.StopBits = SC_UART_ONE_STOP_BIT;
    if (sAPI_UartSetConfig(SC_UART, &uartConfig) == SC_UART_RETURN_CODE_ERROR)
    {
        sAPI_Debug("%s: Configure UART failure!!", __func__);
    }

    /*************************Configure UART2 again*********************************/
    /*******The user can modify the initialization configuratin of UART2 in here.***/
    /*******************************************************************************/
    uart2Config.BaudRate = SC_UART_BAUD_115200;
    uart2Config.DataBits = SC_UART_WORD_LEN_8;
    uart2Config.ParityBit = SC_UART_NO_PARITY_BITS;
    uart2Config.StopBits = SC_UART_ONE_STOP_BIT;
    if (sAPI_UartSetConfig(SC_UART2, &uart2Config) == SC_UART_RETURN_CODE_ERROR)
    {
        sAPI_Debug("%s: Configure UART2 failure!!", __func__);
    }

    /*************************Configure UART3 again*********************************/
    /*******The user can modify the initialization configuratin of UART3 in here.***/
    /*******************************************************************************/
    uart3Config.BaudRate = SC_UART_BAUD_115200;
    uart3Config.DataBits = SC_UART_WORD_LEN_8;
    uart3Config.ParityBit = SC_UART_NO_PARITY_BITS;
    uart3Config.StopBits = SC_UART_ONE_STOP_BIT;
    if (sAPI_UartSetConfig(SC_UART3, &uart3Config) == SC_UART_RETURN_CODE_ERROR)
    {
        sAPI_Debug("%s: Configure UART3 failure!!", __func__);
    }

    sAPI_Debug("%s: UART Configuration is complete!!\n", __func__);

    sAPI_UartRegisterCallback(SC_UART, UartCBFunc);
    sAPI_UartRegisterCallback(SC_UART2, Uart2CBFunc);
    sAPI_UartRegisterCallbackEX(SC_UART3, Uart3CBFuncEx, (void *)"Uart3CBFuncEx");

    sAPI_Debug("%s Task creation completed!!\n", __func__);
}
