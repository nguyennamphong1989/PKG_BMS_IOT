#include "simcom_api.h"
#include "string.h"

void sAPP_AtSendDemo(void)
{
    /*OpenSDK Bool type not defined*/

    /*Responses are captured through attention SC_URC_INTERNAL_AT_RESP_MASK */
    if(sAPI_AtSend("AT+CSQ\r\n",strlen("AT+CSQ\r\n"))!= 1/*TRUE*/)
        sAPI_Debug("send at fail");
}






