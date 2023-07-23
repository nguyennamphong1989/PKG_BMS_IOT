#include "simcom_api.h"





void sAPP_ReadSystemInfoDemo(void)
{
    UINT32 cpuUsedRate, heapFreeSize;
    sAPI_GetSystemInfo(&cpuUsedRate,&heapFreeSize);
    return;
}
   

