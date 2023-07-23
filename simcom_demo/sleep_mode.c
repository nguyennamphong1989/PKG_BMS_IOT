#include "simcom_simcard.h"
#include "simcom_os.h"
#include "simcom_system.h"
#include "simcom_api.h"
#include "simcom_uart.h"
//#include "di_config.c"

extern void PrintfResp(INT8 *format);
extern void di_config(void);
//extern void DI1_handler();
void di_sleep_set2(void)
{
    SC_GPIOReturnCode ret, value;
    unsigned int direction;
    /*PrintfResp("\r\nSet sleep mode on \r\n");
    ret= sAPI_SystemSleepSet(SC_SYSTEM_SLEEP_ENABLE );
    if(ret != SC_GPIORC_OK)
        {
            PrintfResp("\r\nGpio set sleep failed !\r\n");
            
        }
    else
        PrintfResp("\r\nGpio set sleep successful !\r\n");*/
    ret= sAPI_GpioSetDirection(17,0);
    direction= sAPI_GpioGetDirection(17);
    value= sAPI_GpioGetValue(17);
    PrintfResp("ok\r\n");
    //while(1)
    {
    if(value==1 && direction==0)
        {
            //sAPI_SystemSleepSet(SC_SYSTEM_SLEEP_DISABLE);
            PrintfResp("ko\r\n");
            di_config();
            //break;
        }    
    }
}
