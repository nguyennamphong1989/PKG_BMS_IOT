/** 
* @file         cus_usb_vcom.c 
* @brief        
* @author       HT
* @date         2019/2/13
* @version      V1.0.0 
* @par Copyright (c):  
*               SIMCom Co.Ltd 2003-2019
* @par History: 1:Create         
*   
*/

#include "simcom_common.h"
#include "simcom_debug.h"
#include "simcom_api.h"

#define USB_VCOM_RX_BUFFER_SIZE     512 

#ifndef AT_COMMAND_SUPPORT
void UsbVcomCBFunc(void *para)
{
    int readLen = 0;
    char *usbVCcomData = sAPI_Malloc(USB_VCOM_RX_BUFFER_SIZE);
    SIM_MSG_T usbMsg = {0,0,0,NULL};

    readLen = sAPI_UsbVcomRead((UINT8 *)usbVCcomData, USB_VCOM_RX_BUFFER_SIZE);
    sAPI_Debug("%s, usbVCcomData = %s",__func__,usbVCcomData);
    sAPI_Debug("%s, readLen[%d]",__func__,readLen);

#ifdef SIMCOM_UI_DEMO_TO_USB_AT_PORT
    usbMsg.msg_id = SRV_UART;
    usbMsg.arg2 = readLen;
    usbMsg.arg3 = usbVCcomData;
    sendMsgToUIDemo(usbMsg);
#endif

    sAPI_Free((void*)usbVCcomData);
    
    return;
}

void UsbVcomCBFuncEx(int len, void *para)
{
    int readLen = 0;
    char *usbVCcomData = sAPI_Malloc(len);
    SIM_MSG_T usbMsg = {0,0,0,NULL};

    readLen = sAPI_UsbVcomRead((UINT8 *)usbVCcomData, len);
    sAPI_Debug("%s, usbVCcomData = %s",__func__,usbVCcomData);
    sAPI_Debug("%s, readLen[%d]",__func__,readLen);

#ifdef SIMCOM_UI_DEMO_TO_USB_AT_PORT
    usbMsg.msg_id = SRV_UART;
    usbMsg.arg2 = readLen;
    usbMsg.arg3 = usbVCcomData;
    sendMsgToUIDemo(usbMsg);
#endif

    sAPI_Free((void*)usbVCcomData);

    return;
}

void sAPP_UsbVcomTask(void)
{
    sAPI_UsbVcomRegisterCallbackEX(UsbVcomCBFuncEx, (void *)"UsbVcomCBFuncEx");
    sAPI_Debug("sAPP_UsbVcomTask Succeed!\n");
}
#endif

