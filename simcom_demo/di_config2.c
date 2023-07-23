#include "simcom_simcard.h"
#include "simcom_os.h"
#include "simcom_system.h"
#include "simcom_api.h"
#include "simcom_uart.h"


extern void PrintfResp(INT8 *format);
extern void DI1_handler();
void di_sleep_mode();
void di_wakeup_enable();
void di_config1();

SC_GPIOReturnCode sleep;
SC_GPIOReturnCode wakeup;
SC_GPIOReturnCode EXTI;

void di_config2(void){
    di_sleep_set();
    //while (1)
    {
        
        //if (EXTI == )
        {
            di_wakeup_enable();
            di_config1();
        }
        di_sleep_set(); 
    }
    
}

void di_sleep_set(void)
{
    //SC_GPIOReturnCode ret;
    PrintfResp("Set sleep mode on \r\n");
    sleep= sAPI_SystemSleepSet(SC_SYSTEM_SLEEP_ENABLE );
    if(sleep != SC_GPIORC_OK)
        {
            PrintfResp("\r\nGpio set sleep failed !\r\n");
            
        }
    else
        PrintfResp("\r\nGpio set sleep successful !\r\n");
}

void di_wakeup_enable(void)
{   
    sAPI_SystemSleepSet(SC_SYSTEM_SLEEP_DISABLE);
    //SC_GPIOReturnCode ret;
    PrintfResp(" Set DI wake up\r\n");
    sAPI_GpioWakeupEnable(10, SC_GPIO_FALL_EDGE);
    wakeup= sAPI_GpioWakeupEnable(5, SC_GPIO_FALL_EDGE);
    
    if(wakeup != SC_GPIORC_OK)
        {
            PrintfResp("\r\nGpio set wake up failed !\r\n");
            
        }
    else
        PrintfResp("\r\nGpio set wake up successful !\r\n");
    
}

/*void DI1_handler(void)
{
    static unsigned int i = 0;
    if (i % 2 == 0)
        sAPI_GpioSetValue(10, 1);
    else
        sAPI_GpioSetValue(10, 0);
    i++;
}*/

void di_config1(void)
{
    //SC_GPIOReturnCode ret;
    EXTI = sAPI_GpioSetDirection(5, 0); // GPIO0 input
    if (EXTI != SC_GPIORC_OK)
        PrintfResp("GPIO5 set direction failed\r\n");
    else
        PrintfResp("GPIO5 set direction ok\r\n");
    EXTI = sAPI_GpioConfigInterrupt(5, SC_GPIO_RISE_EDGE, DI1_handler);
    if (EXTI != SC_GPIORC_OK)
        {
        PrintfResp("GPIO5 set interrupt failed\r\n");
        //return 0;
        }
    else
        {
        PrintfResp("GPIO5 set interrupt ok\r\n");
        //return 1;
        }
    sAPI_GpioSetDirection(10, 1);
}