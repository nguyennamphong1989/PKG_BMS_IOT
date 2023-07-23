/** 
* @file         cus_key.c 
* @brief        
* @author       LY
* @date         2021/06/09
* @version      V1.0.0 
* @par Copyright (c):  
*               SIMCom Co.Ltd 2003-2019
* @par History: 1:Create         
*   
*/

#include "simcom_common.h"
#include "simcom_debug.h"
#include "simcom_api.h"
#include "string.h"


void PowerKeyIntCBFunc(void)
{
    ONKEY_STATUS keyStatus;

    keyStatus = sAPI_GetPowerKeyStatus();
    sAPI_Debug("%s keyStatus:%d", __func__, keyStatus);
}

void sAPP_KeyTask(void)
{
    /* Power key edge trigger call back function register */
    sAPI_PowerKeyRegisterCallback(PowerKeyIntCBFunc);

    sAPI_Debug("%s run !",__func__);
}


