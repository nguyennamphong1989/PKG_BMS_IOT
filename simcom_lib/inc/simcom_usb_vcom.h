/**
* @file         simcom_usb_vcom.h
* @brief        SIMCom OpenLinux USB EX API
* @author       HT
* @date         2019/2/13
* @version      V1.0.0
* @par Copyright (c):
*               SIMCom Co.Ltd 2003-2019
* @par History: 1:Create
*
*/

#ifndef SIMCOMUSBVCOM_H
#define SIMCOMUSBVCOM_H


/****************************************************************************
    Define enum
*****************************************************************************/
typedef enum  /* The status of Tx */
{
    USB_TX_SUCCEED  = 0,
    USB_TX_FAIL     = -1
}Usb_Tx_Status;

typedef enum  /* The status of USB Vbus */
{
    USB_VBUS_LOW      = 0,
    USB_VBUS_HIGH     = 1,
}SC_Usb_Vbus_Status;

/*****************************************************************************
 * FUNCTION
 *  sAPI_UsbVcomWrite
 *
 * DESCRIPTION
 *  Send data to usb Tx
 *
 * PARAMETERS
 *  *data & length
 *
 * RETURNS
 *  Usb_Tx_Status
 *
 *  USB_TX_SUCCEED  = 0          Sent successfully
 *  USB_TX_FAIL     = -1         fail in send
 *
 * NOTE
 *
 * int sAPI_UsbVcomWrite(unsigned char* data, unsigned long length);
*****************************************************************************/




typedef void (*SC_Usb_Callback)(void *para);
typedef void (*SC_Usb_CallbackEX)(int len, void *reserve);


#endif


