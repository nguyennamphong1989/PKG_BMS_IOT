#include "simcom_api.h"

sTaskRef helloWorldProcesser;
static UINT8 helloWorldProcesserStack[1024];

void simcom_printf(const char *format,...){
    char tmpstr[200];

    va_list args;

    memset(tmpstr,0,sizeof(tmpstr));

    va_start(args,format);
    sAPI_Vsnprintf(tmpstr,sizeof(tmpstr),format,args);
    va_end(args);

    
    
    sAPI_Debug("simcom_printf [%s]",tmpstr);

}

void sTask_HelloWorldProcesser(void* argv)
{
    sAPI_Debug("Task runs successfully");
 
    simcom_printf("%s  %d %f","simcom",2020,10.23);

}

void sAPP_HelloWorldDemo(void)
{
    SC_STATUS status = SC_SUCCESS;

    status = sAPI_TaskCreate(&helloWorldProcesser, helloWorldProcesserStack, 1024, 150, "helloWorldProcesser",sTask_HelloWorldProcesser,(void *)0);
    if(SC_SUCCESS != status)
    {
        sAPI_Debug("Task create fail,status = [%d]",status);
    }        
}
