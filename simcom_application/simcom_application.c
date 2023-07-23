#include "simcom_api.h"


typedef void (*app_t)(void * argv);
typedef struct
{
	app_t app_entry;

} appRegItem_t;
#define _appRegTable_attr_ __attribute__((unused, section(".appRegTable")))



static void Application(void * argv)
{

    /* Do not comment the initialization code below */
    unsigned long *apiTable = (unsigned long*)argv;
    if(apiTable != NULL)
    {
         get_sAPI(apiTable);
         sAPI_Debug("sc_Init successfull...");

    }
    /*End of initialization*/

    //sAPI_enableDUMP();
    sAPI_TaskSleep(5 * 200);
    sAPI_Debug("sAPI_Malloc:%p",sAPI_Malloc);
    sAPI_Debug("sAPI_Debug is OK...");
    sAPI_Debug("helloworld is finish...");
#ifdef SIMCOM_A7620_GREE
    sAPP_WatchDogTaskInit();
#endif
#ifdef SIMCOM_UI_DEMO
    sAPP_SimcomUIDemo();
#endif

    sAPP_UartTask();
    sAPP_UrcTask();
#ifndef AT_COMMAND_SUPPORT
    sAPP_UsbVcomTask();
#endif
    sAPP_KeyTask();
#ifdef BT_SUPPORT
    sAPP_BleServerDemo();
#endif
    sAPP_HelloWorldDemo();
}

appRegItem_t helloworldapp_entry _appRegTable_attr_ = {.app_entry = Application};
