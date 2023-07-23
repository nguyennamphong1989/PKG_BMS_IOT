#include "simcom_api.h"

void pingResult(int size,char *statistics)
{
    if(statistics != NULL)
        sAPI_Debug("size[%d] statistics[%s]",size,statistics);
}

void simcom_ping_test(void)
{
    SCPingResultCode ret ;

    SCping ctx;
    memset(&ctx,0,sizeof(ctx));
    ctx.destination = "www.baidu.com";
    ctx.count = 20;
    ctx.interval = 10;
    ctx.resultcb = pingResult;

    
    ret = sAPI_Ping(&ctx);

    sAPI_Debug("simcom_ping_test ret[%d]",ret);
}

