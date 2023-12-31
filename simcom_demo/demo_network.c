#include "simcom_api.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#if 0
static sTaskRef networkTaskRef;
static unsigned char *networkTaskStack = NULL;
#endif

sTimerRef networkTimerRef;
extern sMsgQRef simcomUI_msgq;
extern void PrintfOptionMenu(INT8* options_list[], int array_size);
extern void PrintfResp(INT8* format);

typedef struct{
    int cid;
    char pdptype[20];
    char apn[20];
}SCcgdcontParm;
typedef struct{
    int mode;
    int format;
    char oper[20];
    int act;
}SCcopsParm;

#if 0
static void sTask_NetWork(void *arg)
{
    SC_STATUS status;
    SIM_MSG_T cmmsg;

    int ret = 0;
    unsigned int loop=0;

    while(loop++ < 100)
    {
        unsigned char csq;
        unsigned char ret;
        SCcpsiParm Scpsi;
        char cops[100]={0};
        char cpin[50]={0};
        char cgdcont[500]={0};
        char cgact[200]={0};
        int creg,cgreg,cnmp,cgatt;
        char iccid[32] = {0};
        int cfun = 0;

        sAPI_TaskSleep(200*30);
#if 1
        //sAPI_NetworkSetCfun(0);
        sAPI_TaskSleep(200*2);

        ret = sAPI_NetworkGetCfun(&cfun);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get cfun success. cfun=%d!",cfun);
        else
            sAPI_Debug("Get cfun falied!");
        sAPI_TaskSleep(200*2);
        //sAPI_NetworkSetCfun(1);

        ret = sAPI_NetworkGetCfun(&cfun);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get cfun success. cfun=%d!",cfun);
        else
            sAPI_Debug("Get cfun falied!");

        ret = sAPI_NetworkGetCsq(&csq);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get csq success. csq=%d!",csq);
        else
            sAPI_Debug("Get csq falied!");

        ret = sAPI_SysGetIccid(iccid);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get iccid success. iccid=%s ",iccid);
        else
            sAPI_Debug("Get iccid falied!");


        sAPI_TaskSleep(200*2);

        sAPI_Debug("Get creg !");
        ret = sAPI_NetworkGetCreg(&creg);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get creg success. creg=%d!",creg);
        else
            sAPI_Debug("Get creg falied!");

        sAPI_TaskSleep(200*2);

        ret = sAPI_NetworkGetCpsi(&Scpsi);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get cpsi success. NEmode=%s,MM=%s,LAC=%d,CELL=%d,Gband=%s,Lband=%s,TAC=%d!",Scpsi.networkmode,Scpsi.Mnc_Mcc,Scpsi.LAC,Scpsi.CellID,Scpsi.GSMBandStr,Scpsi.LTEBandStr,Scpsi.TAC);
        else
            sAPI_Debug("Get cpsi falied!");

        sAPI_TaskSleep(200*2);
        ret = sAPI_NetworkGetCgreg(&cgreg);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get cgreg success. cgreg=%d!",cgreg);
        else
            sAPI_Debug("Get cgreg falied!");

        sAPI_TaskSleep(200*2);
        sAPI_NetworkSetCnmp(38);
        sAPI_TaskSleep(200*2);
        ret = sAPI_NetworkGetCnmp(&cnmp);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get cnmp success. cnmp=%d!",cnmp);
        else
            sAPI_Debug("Get cnmp falied!");
        sAPI_TaskSleep(200*2);
        sAPI_NetworkSetCnmp(2);

        sAPI_TaskSleep(200*2);
        sAPI_NetworkSetCops(1,2,"46001",7);
        sAPI_TaskSleep(200*2);
        ret = sAPI_NetworkGetCops(cops);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get cops success. cops=%s!",cops);
        else
            sAPI_Debug("Get cops falied:%d!",ret);


        sAPI_TaskSleep(200*2);
        ret = sAPI_SimPinGet(cpin);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get Cpin success. Cpin=%s!",cpin);
        else
            sAPI_Debug("Get Cpin falied!");


        sAPI_TaskSleep(200*2);
        ret = sAPI_NetworkGetCgatt(&cgatt);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get Cgatt success. Cgatt=%d!",cgatt);
        else
            sAPI_Debug("Get Cgatt falied!");


        sAPI_TaskSleep(200*2);
        sAPI_NetworkSetCgdcont(2,"IP","cnnet");
        sAPI_TaskSleep(200*2);
        ret = sAPI_NetworkGetCgdcont(cgdcont);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get Cgdcont success. Cgdcont=%s!",cgdcont);
        else
            sAPI_Debug("Get Cgdcont falied:%d!",ret);

        sAPI_TaskSleep(200*2);
        sAPI_NetworkSetCgact(1,2);
        sAPI_TaskSleep(200*2);
        ret = sAPI_NetworkGetCgact(cgact);
        if(ret == SC_NET_SUCCESS)
            sAPI_Debug("Get Cgact success. Cgact=%s!",cgact);
        else
            sAPI_Debug("Get Cgact falied!");
#endif
    }
}
#endif

void CfunDemo(void)
{
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    int opt = 0;
    UINT8 cfun;
    UINT8 ret;
    char cfunResp[50];
    INT8 *note = "\r\n1.GET CFUN   2.SET CFUN 1   3.SET CFUN 0   4.SET CFUN 4  99.back.\r\n";
    while(1)
    {
        PrintfResp(note);
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);
        switch(opt)
        {
            case 1:
            {
                ret = sAPI_NetworkGetCfun(&cfun);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Get cfun success. cfun=%d!",cfun);
                    sprintf(cfunResp,"\r\nGet Cfun success. Cfun=%d!\r\n",cfun);
                    PrintfResp(cfunResp);
                    break;
                }
                else
                {
                    sAPI_Debug("Get cfun falied!");
                    PrintfResp("\r\nGet cfun falied!\r\n");
                    break;
                }
            }
            case 2:
            {
                ret = sAPI_NetworkSetCfun(1);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Set cfun 1 success.!");
                    PrintfResp("\r\nSet cfun 1 success.!\r\n");
                    break;
                }
                else
                {
                    sAPI_Debug("Set cfun falied!");
                    PrintfResp("\r\nSet cfun falied!\r\n");
                    break;
                }
            }
            case 3:
            {
                ret = sAPI_NetworkSetCfun(0);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Set cfun 0 success.!");
                    PrintfResp("\r\nSet cfun 0 success.!\r\n");
                    break;
                }
                else
                {
                    sAPI_Debug("Set cfun falied!");
                    PrintfResp("\r\nSet cfun falied!\r\n");
                    break;
                }
            }
            case 4:
            {
                ret = sAPI_NetworkSetCfun(4);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Set cfun 4 success.!");
                    PrintfResp("\r\nSet cfun 4 success.!\r\n");
                    break;
                }
                else
                {
                    sAPI_Debug("Set cfun falied!");
                    PrintfResp("\r\nSet cfun falied!\r\n");
                    break;
                }
            }
            case 99:
            {
                return;
            }
            default:
                break;
        }
    }
}

void CgattDemo(void)
{
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    int opt = 0;
    int cgatt;
    UINT8 ret;
    char cgattresp[50]={0};
    INT8 *note = "\r\n1.GET CGATT   2.Packet domain attach   3.Packet domain detach   99.back.\r\n";
    while(1)
    {
        PrintfResp(note);
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);
        switch(opt)
        {
            case 1:
            {
                ret = sAPI_NetworkGetCgatt(&cgatt);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Get Cgatt success. Cgatt=%d!",cgatt);
                    sprintf(cgattresp,"\r\nGet Cgatt success. Cgatt=%d!\r\n",cgatt);
                    PrintfResp(cgattresp);
                    break;
                }
                else
                {
                    sAPI_Debug("Get cgatt falied!");
                    PrintfResp("\r\nGet cgatt falied!\r\n");
                    break;
                }
            }
            case 2:
            {
                ret = sAPI_NetworkSetCgatt(1);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Set attach success.!");
                    PrintfResp("\r\nSet attach success.!\r\n");
                    break;
                }
                else
                {
                    sAPI_Debug("Set attach falied!");
                    PrintfResp("\r\nSet attach falied!\r\n");
                    break;
                }
            }
            case 3:
            {
                ret = sAPI_NetworkSetCgatt(0);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Set detach success.!");
                    PrintfResp("\r\nSet detach success.!\r\n");
                    break;
                }
                else
                {
                    sAPI_Debug("Set detach falied!");
                    PrintfResp("\r\nSet detach falied!\r\n");
                    break;
                }
            }
            case 99:
            {
                return;
            }
            default:
                break;
        }
    }
}

void SetCgact(void)
{
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    int listlen = 0;
    UINT8 ret;
    int opt = 0;
    int cgactcid = 0XFF;
    int cgactstate = 0XFF;
    INT8 *cgact_list[] = {
        "1. CID:(1-16)",
        "2. state:(0,1)",
        "99.back",
    };

    while(1)
    {
        PrintfResp(cgact_list[listlen]);
        listlen++;
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);
        switch(listlen)
        {
            case 1:
            {
                cgactcid = opt;
                if((1 > cgactcid )||(cgactcid > 16))
                {
                    PrintfResp("\r\nPlease enter valid cid parameters\r\n");
                    return;
                }
                break;
            }
            case 2:
            {
                cgactstate = opt;
                ret = sAPI_NetworkSetCgact(cgactstate,cgactcid);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Set cgactt success.!");
                    PrintfResp("\r\nSet cgact success.!\r\n");
                    return;
                }
                else
                {
                    sAPI_Debug("Set cgact falied!");
                    PrintfResp("\r\nSet cgact falied!\r\n");
                    return;
                }
            }
            case 99:
            {
                return;
            }
            default:
                break;
        }
    }
}

void CgactDemo(void)
{
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    int opt = 0;
    int i;
    UINT8 ret;
    SCAPNact SCact[8] = {0};
    char cgactresp[10]={0};
    INT8 *note = "\r\n1.GET CGACT   2.SET CGACT   99.back.\r\n";
    while(1)
    {
        PrintfResp(note);
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);
        switch(opt)
        {
            case 1:
            {
                ret = sAPI_NetworkGetCgact(&SCact);
                if(ret == SC_NET_SUCCESS)
                {
                    for(i = 0;i < 8;i++)/*Read it eight times*/
                    {
                        sAPI_Debug("Get cgact success. cid=%d,actstate=%d!",SCact[i].cid,SCact[i].isActived);
                        if(SCact[i].isdefine)
                        {
                            sprintf(cgactresp,"\r\n cid=%d, actstate=%d\r\n",SCact[i].cid,SCact[i].isActived);
                            PrintfResp(cgactresp);
                        }
                    }
                    break;
                }
                else
                {
                    sAPI_Debug("Get cgact falied!");
                    PrintfResp("\r\nGet cgact falied!\r\n");
                    break;
                }
            }
            case 2:
            {
                SetCgact();
                break;
            }
            case 99:
            {
                return;
            }
            default:
                break;
        }
    }
}
void SetCgdcont(void)
{
    SIM_MSG_T optionMsg = {0,0,0,NULL};
    int listlen = 0;
    UINT8 ret;
    char opt[100];
    SCcgdcontParm cgdcontParm = {0,{0},{0}};
    INT8 *cgact_list[] = {
        "1. CID:(1-16)",
        "2. pdptype:(IP,IPV6,IPV4V6)",
        "3. APN:",
        "99.back",
    };

    while(1)
    {
        PrintfResp(cgact_list[listlen]);
        listlen++;
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        memset(opt,0,sizeof(opt));
        memcpy(opt , optionMsg.arg3,strlen(optionMsg.arg3));
        sAPI_Free(optionMsg.arg3);
        switch(listlen)
        {
            case 1:
            {
                cgdcontParm.cid = atoi(opt);
                if((1 > cgdcontParm.cid )||(cgdcontParm.cid > 16))
                {
                    PrintfResp("\r\nPlease enter valid cid parameters\r\n");
                    return;
                }
                break;
            }
            case 2:
            {
                if(strstr(opt,"\r\n") != NULL)
                    memcpy(cgdcontParm.pdptype,opt,strlen(opt) - 2);
                else
                    memcpy(cgdcontParm.pdptype,opt,strlen(opt));
                break;
            }
            case 3:
            {
                if(strstr(opt,"\r\n") != NULL)
                    memcpy(cgdcontParm.apn,opt,strlen(opt) - 2);
                else
                    memcpy(cgdcontParm.apn,opt,strlen(opt));
                sAPI_Debug("Set cgdcont:%d   %s   %s",cgdcontParm.cid,cgdcontParm.pdptype,cgdcontParm.apn);
                ret = sAPI_NetworkSetCgdcont(cgdcontParm.cid,cgdcontParm.pdptype,cgdcontParm.apn);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Set cgdcont success.!");
                    PrintfResp("\r\nSet cgdcont success.!\r\n");
                    return;
                }
                else
                {
                    sAPI_Debug("Set cgdcont falied!");
                    PrintfResp("\r\nSet cgdcont falied!\r\n");
                    return;
                }
            }
            case 99:
            {
                return;
            }
            default:
                break;
        }
    }
}

void GetCGAUTH()
{
    SIM_MSG_T optionMsg = {0,0,0,NULL};
    int cid = 0;
    UINT8 ret;
    int opt = 0;
    char cgauthresp[100];
    SCCGAUTHParm cgauthParm = {0,0,{0},{0}};
    INT8 cgauth_list[50] = "\r\n1. CID:(1-16),99.back\r\n";
    while(1)
    {
        PrintfResp(cgauth_list);
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);
        switch(opt)
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            {
                cid = opt;
                ret = sAPI_NetworkGetCgauth(&cgauthParm,cid);
                if(SC_NET_SUCCESS == ret)
                {
                    sprintf(cgauthresp,"\r\n cid=%d,type=%d,usr=%s,passwd=%s \r\n",cgauthParm.cid,cgauthParm.authtype,cgauthParm.user,cgauthParm.passwd);
                    PrintfResp(cgauthresp);
                }
                else
                {
                    sprintf(cgauthresp,"\r\n GET FAIL \r\n");
                    PrintfResp(cgauthresp);
                }
                break;
            }
            case 99:
            {
                return;
            }
            default:
                break;
        }
    }
}

void SetCGAUTH()
{
    SCCGAUTHParm auth;
    int ret = SC_NET_SUCCESS;

    auth.cid = 1;
    auth.authtype = 2;
    sprintf(auth.user, "simcom");
    sprintf(auth.passwd, "simcom");
    ret = sAPI_NetworkSetCgauth(&auth,0);/*delflag:1(Delete the parameters corresponding to CID)   0(Set the parameters corresponding to CID)*/
}

void CgdcontDemo(void)
{
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    int opt = 0;
    int i;
    UINT8 ret;
    SCApnParm SCapn[8]={0};
    char cgdcontresp[100]={0};
    INT8 *note = "\r\n1.GET CGDCONT   2.SET CGDCONT   3.GET CGAUTH   4.SET CGAUTH   99.back.\r\n";
    while(1)
    {
        PrintfResp(note);
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);
        switch(opt)
        {
            case 1:
            {
                ret = sAPI_NetworkGetCgdcont(&SCapn);
                if(ret == SC_NET_SUCCESS)
                {
                    for(i = 0;i < 8;i++)/*Read it eight times*/
                    {
                        sAPI_Debug("Get Cgdcont success. cid=%d,iptype=%d,apnstr=%s!",SCapn[i].cid,SCapn[i].iptype,SCapn[i].ApnStr);
                        if(SCapn[i].iptype)/*Iptype 0 means undefined*/
                        {
                            sprintf(cgdcontresp,"\r\n cid=%d,iptype=%d,apnstr=%s \r\n",SCapn[i].cid,SCapn[i].iptype,SCapn[i].ApnStr);
                            PrintfResp(cgdcontresp);
                        }
                    }
                    break;
                }
                else
                {
                    sAPI_Debug("Get cgdcont falied!");
                    PrintfResp("\r\nGet cgdcont falied!\r\n");
                    break;
                }
            }
            case 2:
            {
                SetCgdcont();
                break;
            }
            case 3:
            {
                GetCGAUTH();
                break;
            }
            case 4:
            {
                SetCGAUTH();
                break;
            }
            case 99:
            {
                return;
            }
            default:
                break;
        }
    }
}
void SetCops(void)
{
    SIM_MSG_T optionMsg = {0,0,0,NULL};
    int listlen = 0;
    UINT8 ret;
    char opt[50] = {0};
    SCcopsParm copsParm = {0,0,{0},0};
    INT8 *cgact_list[] = {
        "1. mode:",
        "2. format:",
        "3. oper:",
        "4. ACT:",
        "99.back",
    };

    while(1)
    {
        PrintfResp(cgact_list[listlen]);
        listlen++;
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        memset(opt,0,sizeof(opt));
        memcpy(opt , optionMsg.arg3,strlen(optionMsg.arg3));
        sAPI_Free(optionMsg.arg3);
        switch(listlen)
        {
            case 1:
            {
                copsParm.mode = atoi(opt);
                break;
            }
            case 2:
            {
                copsParm.format = atoi(opt);
                break;
            }
            case 3:
            {
                if(strstr(opt,"\r\n") != NULL)
                    memcpy(copsParm.oper,opt,strlen(opt) - 2);
                else
                    memcpy(copsParm.oper,opt,strlen(opt));
                break;
            }
            case 4:
            {
                copsParm.act = atoi(opt);
                ret = sAPI_NetworkSetCops(copsParm.mode,copsParm.format,copsParm.oper,copsParm.act);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Set cops success.!");
                    PrintfResp("\r\nSet cops success.!\r\n");
                    return;
                }
                else
                {
                    sAPI_Debug("Set cops falied!");
                    PrintfResp("\r\nSet cops falied!\r\n");
                    return;
                }
                break;
            }
            case 99:
            {
                return;
            }
            default:
                break;
        }
    }
}

void CopsDemo(void)
{
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    int opt = 0;
    UINT8 ret;
    char cops[100]={0};
    char copsresp[100]={0};
    INT8 *note = "\r\n1.GET COPS   2.SET COPS   99.back.\r\n";
    while(1)
    {
        PrintfResp(note);
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);
        switch(opt)
        {
            case 1:
            {
                ret = sAPI_NetworkGetCops(cops);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Get cops success. cops=%s!",cops);
                    sprintf(copsresp,"\r\nGet cops success. cops=%s!\r\n",cops);
                    PrintfResp(copsresp);
                    break;
                }
                else
                {
                    sAPI_Debug("Get cops falied!");
                    PrintfResp("\r\nGet cops falied!\r\n");
                    break;
                }
            }
            case 2:
            {
                SetCops();
                break;
            }
            case 99:
            {
                return;
            }
            default:
                break;
        }
    }
}

void CgpaddrDemo(void)
{
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    int opt = 0;
    UINT8 ret;
    SCcgpaddrParm cgpaddrParm;
    INT8 *note = "\r\n1.GET (1-15) IPaddr  99.back.\r\n";
    char NetResp[200];

    while(1)
    {
        PrintfResp(note);
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);
        switch(opt)
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            {
                memset(&cgpaddrParm,0,sizeof(cgpaddrParm));
                ret = sAPI_NetworkGetCgpaddr(opt,&cgpaddrParm);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Get Ipaddr success. cid=%d,type=%d,ipv4=%s,ipv6=%s!",cgpaddrParm.cid,cgpaddrParm.iptype,cgpaddrParm.ipv4addr,cgpaddrParm.ipv6addr);
                    sprintf(NetResp,"\r\nGet Ipaddr success. cid=%d,type=%d,ipv4=%s,ipv6=%s!",cgpaddrParm.cid,cgpaddrParm.iptype,cgpaddrParm.ipv4addr,cgpaddrParm.ipv6addr);
                    PrintfResp(NetResp);
                    break;
                }
                else
                {
                    sAPI_Debug("Get Ipaddr falied!");
                    PrintfResp("\r\nGet Ipaddr falied!\r\n");
                    break;
                }
            }
            case 99:
            {
                return;
            }
            default:
                break;
        }
    }
}

void CnmpDemo(void)
{
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    int opt = 0;
    int cnmp;
    char cnmpresp[50];
    UINT8 ret;
    INT8 *note = "\r\n1.GET CNMP   2.SET CNMP:2   13.SET CNMP:13   38.SET CNMP:38   99.back.\r\n";
    while(1)
    {
        PrintfResp(note);
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);
        switch(opt)
        {
            case 1:
            {
                ret = sAPI_NetworkGetCnmp(&cnmp);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Get cnmp success. cnmp=%d!",cnmp);
                    sprintf(cnmpresp,"\r\nGet cnmp success. cnmp:%d!\r\n",cnmp);
                    PrintfResp(cnmpresp);
                    break;
                }
                else
                {
                    sAPI_Debug("Get cnmp falied!");
                    PrintfResp("\r\nGet cnmp falied!\r\n");
                    break;
                }
            }
            case 2:
            {
                ret = sAPI_NetworkSetCnmp(2);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Set cnmp 2 success.!");
                    PrintfResp("\r\nSet cnmp 2 success.!\r\n");
                    break;
                }
                else
                {
                    sAPI_Debug("Set cnmp falied!");
                    PrintfResp("\r\nSet cnmp falied!\r\n");
                    break;
                }
            }
            case 13:
            {
                ret = sAPI_NetworkSetCnmp(13);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Set cnmp 13 success.!");
                    PrintfResp("\r\nSet cnmp 13 success.!\r\n");
                    break;
                }
                else
                {
                    sAPI_Debug("Set cnmp falied!");
                    PrintfResp("\r\nSet cnmp falied!\r\n");
                    break;
                }
            }
            case 38:
            {
                ret = sAPI_NetworkSetCnmp(38);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Set cnmp 38 success.!");
                    PrintfResp("\r\nSet cnmp 38 success.!\r\n");
                    break;
                }
                else
                {
                    sAPI_Debug("Set cnmp falied!");
                    PrintfResp("\r\nSet cnmp falied!\r\n");
                    break;
                }
            }
            case 99:
            {
                return;
            }
            default:
                break;
        }
    }
}


void NetWorkDemo(void)
{
    UINT8 csq;
    UINT8 ret;
    int creg,cgreg;
    SCcpsiParm Scpsi = { 0 };
    SCcnetciParm Snetci[12];
    UINT8 cpin;
    char imsi[50]={0};
    char NetResp[1000]={0};
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    int opt = 0;
    int APInum;
    SCcgpaddrParm cgpaddrParm;
    INT8 *note = "\r\nPlease select an option to test from the items listed below.\r\n";
    INT8 *options_list[] = {
        "1. Query signal quality(CSQ)",
        "2. CS domain(CREG)",
        "3. PS domain(CGREG)",
        "4. system information(CPSI)",
        "5. mode selection(CNMP)",
        "6. Operator selection(COPS)",
        "7. PDP context",
        "8. act or deact(CGACT)",
        "9. attach or detach(CGATT)",
        "10.phone functionality(CFUN)",
        "11.CPIN",
        "12.IMSI",
        "13.Ipaddr",
        "14.test GPRS",
        "15.test voice call",
        "16.platform connection",
        "17. Adjacent base station information(CNETCI)",
        "99.back",
    };
    /*Create flag for network,the initialization operation must be used*/
    sAPI_NetworkInit();

    while(1)
    {
        PrintfResp(note);
        PrintfOptionMenu(options_list,sizeof(options_list)/sizeof(options_list[0]));
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }

        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);

        switch(opt)
        {
            case 1:
            {
                ret = sAPI_NetworkGetCsq(&csq);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Get csq success. csq=%d!",csq);
                    sprintf(NetResp,"\r\nGet csq success. csq:%d!\r\n",csq);
                    PrintfResp(NetResp);
                    break;
                }
                else
                {
                    sAPI_Debug("Get csq falied!");
                    PrintfResp("\r\nGet csq falied!\r\n");
                    break;
                }
            }

            case 2:
            {
                ret = sAPI_NetworkGetCreg(&creg);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Get creg success. creg=%d!",creg);
                    sprintf(NetResp,"\r\nGet creg success. creg=%d!\r\n",creg);
                    PrintfResp(NetResp);
                    break;
                }
                else
                {
                    sAPI_Debug("Get creg falied!");
                    PrintfResp("\r\nGet creg falied!\r\n");
                    break;
                }
            }

            case 3:
            {
                ret = sAPI_NetworkGetCgreg(&cgreg);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Get cgreg success. cgreg=%d!",cgreg);
                    sprintf(NetResp,"\r\nGet cgreg success. cgreg=%d!\r\n",cgreg);
                    PrintfResp(NetResp);
                    break;
                }
                else
                {
                    sAPI_Debug("Get cgreg falied!");
                    PrintfResp("\r\nGet cgreg falied!\r\n");
                    break;
                }
            }

            case 4:
            {
                ret = sAPI_NetworkGetCpsi(&Scpsi);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Get cpsi success. NEmode=%s,MM=%s,LAC=%d,CELL=%d,Gband=%s,Lband=%s,TAC=%d,RSRP=%d,RXLEV=%d,TA=%d!",Scpsi.networkmode,Scpsi.Mnc_Mcc,Scpsi.LAC,Scpsi.CellID,Scpsi.GSMBandStr,Scpsi.LTEBandStr,Scpsi.TAC,Scpsi.Rsrp,Scpsi.RXLEV,Scpsi.TA);
                    sprintf(NetResp,"\r\nGet cpsi success. NEmode=%s,MM=%s,LAC=%d,CELL=%d,Gband=%s,Lband=%s,TAC=%d,RSRP=%d,RXLEV=%d,TA=%d,SINR=%d!\r\n",Scpsi.networkmode,Scpsi.Mnc_Mcc,Scpsi.LAC,Scpsi.CellID,Scpsi.GSMBandStr,Scpsi.LTEBandStr,Scpsi.TAC,Scpsi.Rsrp,Scpsi.RXLEV,Scpsi.TA,Scpsi.SINR);
                    PrintfResp(NetResp);
                    break;
                }
                else
                {
                    sAPI_Debug("Get cpsi falied!");
                    PrintfResp("\r\nGet cpsi falied!\r\n");
                    break;
                }
            }

            case 5:
            {
                CnmpDemo();
                break;
            }

            case 6:
            {
                CopsDemo();
                break;
            }

            case 7:
            {
                CgdcontDemo();
                break;
            }

            case 8:
            {
                CgactDemo();
                break;
            }

            case 9:
            {
                CgattDemo();
                break;
            }

            case 10:
            {
                CfunDemo();
                break;
            }

            case 11:
            {
                ret = sAPI_SimPinGet(&cpin);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Get Cpin success. Cpin=%d!",cpin);
                    sprintf(NetResp,"\r\nGet Cpin success. Cpin=%d!\r\n",cpin);
                    PrintfResp(NetResp);
                    break;
                }
                else
                {
                    sAPI_Debug("Get cpin falied!");
                    PrintfResp("\r\nGet cpin falied!\r\n");
                    break;
                }
            }
            case 12:
            {
                memset(imsi,0,sizeof(imsi));
                ret = sAPI_SysGetimsi(imsi);
                if(ret == SC_NET_SUCCESS)
                {
                    sAPI_Debug("Get IMSI success. IMSI=%s!",imsi);
                    sprintf(NetResp,"\r\nGet IMSI success. IMSI=%s!\r\n",imsi);
                    PrintfResp(NetResp);
                    break;
                }
                else
                {
                    sAPI_Debug("Get IMSI falied!");
                    PrintfResp("\r\nGet IMSI falied!\r\n");
                    break;
                }
            }
            case 13:
            {
                CgpaddrDemo();
                break;
            }
            case 14:
            {
                /*This is an example of testing the state of the network ready to use a data service,
                This is a generic example,Users can also design according to their actual needs*/
                while(1)
                {
                    if(APInum > 20)/*You can define the number of retries you want*/
                    {
                        /*check card*/
                    }
                    APInum++;
                    ret = sAPI_SimPinGet(&cpin);/*Power on to inquire the SIM card status*/
                    if(ret == SC_NET_SUCCESS)
                    {
                        if(cpin == SC_SIM_READY)
                        {
                            PrintfResp("\r\nGet cpin success and cpin ready\r\n");
                            break;
                        }
                        else if((cpin == SC_SIM_PIN)||(cpin == SC_SIM_PUK))
                        {
                            PrintfResp("\r\nGet cpin success and cpin PIN\r\n");
                            sAPI_TaskSleep(200);

                            /*<Call the SimCard interface>*/

                            sAPI_TaskSleep(200);
                            continue;/*retry*/
                        }
                        else
                        {
                            sAPI_TaskSleep(200);
                            continue;/*retry*/
                        }
                    }
                    else
                    {
                        sAPI_TaskSleep(200);/*All API query times are recommended once 1S*/
                        PrintfResp("\r\nGet cpin falied!\r\n");
                        continue;/*If this fails, you can wait for 1S to continue the query*/
                    }
                }
                while(1)
                {
                    if(APInum > 60)/*You can define the number of retries you want*/
                    {
                        /*It is recommended to switch to cFUN or CNMP mode*/
                    }
                    APInum++;
                    ret = sAPI_NetworkGetCpsi(&Scpsi);/*Power on to inquire the SIM card status*/
                    if(ret == SC_NET_SUCCESS)
                    {
                        if(strncmp(Scpsi.networkmode,"No",sizeof(char)*2) == 0)
                        {
                            sAPI_TaskSleep(200);
                            continue;/*retry*/
                        }
                        if(strncmp(Scpsi.networkmode,"LTE",sizeof(char)*3) == 0)
                        {
                            PrintfResp("\r\nGet cpsi success and LTE\r\n");
                            break;/*You can use the TCPIP API*/
                        }
                        if(strncmp(Scpsi.networkmode,"GSM",sizeof(char)*3) == 0)
                        {
                            PrintfResp("\r\nGet cpsi success and GSM\r\n");
                            break;/*Maybe some other API has to be called*/
                        }
                    }
                    else
                    {
                        sAPI_TaskSleep(200);/*All API query times are recommended once 1S*/
                        PrintfResp("\r\nGet cpin falied!\r\n");
                        continue;/*If this fails, you can wait for 1S to continue the query*/
                    }
                }
                /*If you are already registered for a GSM network,You can try switching CNMP =38 and register LTE*/
                /*If you accept GSM,Please refer to the CgdcontDemo:case 2,After setting it up successfully,You can use the TCPIP API*/
            }
            case 15:
            {
                /*The detection of the card refers to Case 15*/
                while(1)
                {
                    if(APInum > 60)/*You can define the number of retries you want*/
                    {
                        /*It is recommended to switch to cFUN or CNMP mode*/
                    }
                    APInum++;
                    ret = sAPI_NetworkGetCpsi(&Scpsi);
                    if(ret == SC_NET_SUCCESS)
                    {
                        if(strncmp(Scpsi.networkmode,"No",sizeof(char)*2) == 0)
                        {
                            sAPI_TaskSleep(200);
                            continue;/*retry*/
                        }
                        if(strncmp(Scpsi.networkmode,"LTE",sizeof(char)*3) == 0)
                        {
                            PrintfResp("\r\nGet cpsi success and LTE\r\n");
                            break;
                        }
                        if(strncmp(Scpsi.networkmode,"GSM",sizeof(char)*3) == 0)
                        {
                            PrintfResp("\r\nGet cpsi success and GSM\r\n");
                            break;
                        }
                    }
                    else
                    {
                        sAPI_TaskSleep(200);/*All API query times are recommended once 1S*/
                        PrintfResp("\r\nGet cpin falied!\r\n");
                        continue;/*If this fails, you can wait for 1S to continue the query*/
                    }
                }
                while(1)
                {
                    if(APInum > 30)/*You can define the number of retries you want*/
                    {
                        /*It is recommended to switch to cFUN or CNMP mode*/
                    }
                    APInum++;
                    ret = sAPI_NetworkGetCreg(&creg);/*creg judgment cs      cgreg judgment ps*/
                    if(ret == SC_NET_SUCCESS)
                    {
                        if((creg == 1)||(creg == 5))
                        {
                            /*you can voice call*/
                            break;
                        }
                        else
                        {
                            sAPI_TaskSleep(200);
                            continue;/*retry*/
                        }
                    }
                    else
                    {
                        sAPI_TaskSleep(200);/*All API query times are recommended once 1S*/
                        PrintfResp("\r\nGet cpin falied!\r\n");
                        continue;/*If this fails, you can wait for 1S to continue the query*/
                    }
                }
                break;
            }
            case 16:
            {
                NetLedPlatConnectDemo();
                break;
            }
            case 17:
            {
                ret = sAPI_NetworkGetCnetci(Snetci);
                if(ret == SC_NET_SUCCESS)
                {
                    int i = 0;
                    for(i = 0;i < 12;i++)/*Read it ten times*/
                    {
                        sAPI_Debug("Get cnetci success.i=%d:MM=%s,TAC=%d,CELL=%d,RSRP=%d,RSRQ=%d,RXSIGLEVEL=%d",i,Snetci[i].Mnc_Mcc,Snetci[i].TAC,Snetci[i].CellID,Snetci[i].Rsrp,Snetci[i].Rsrq,Snetci[i].RXSIGLEVEL);
                        if(Snetci[i].Rsrp || Snetci[i].RXSIGLEVEL)/*Iptype 0 means undefined*/
                        {
                            sprintf(NetResp,"\r\n i=%d,MM=%s,TAC=%d,CELL=%d,RSRP=%d,RSRQ=%d,RXSIGLEVEL=%d",i,Snetci[i].Mnc_Mcc,Snetci[i].TAC,Snetci[i].CellID,Snetci[i].Rsrp,Snetci[i].Rsrq,Snetci[i].RXSIGLEVEL);
                            PrintfResp(NetResp);
                        }
                    }
                    break;
                }
                else
                {
                    sAPI_Debug("Get cnetci falied!");
                    PrintfResp("\r\nGet cnetci falied!\r\n");
                    break;
                }
            }
            case 99:
            {
                return;
            }
            default :
                break;
        }
    }
}
void SetNetLedPlatConnectStatus(void)
{
#if 0
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    UINT32 ret = 0;
    int opt = 0;
    INT8 *connet_status_list[] = {
        "1.Unconnected",
        "2.Connected",
        "99.back",
    };

    while(1)
    {
        PrintfOptionMenu(connet_status_list,sizeof(connet_status_list)/sizeof(connet_status_list[0]));
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);
        switch(opt)
        {
            case 1:
            {
                ret=sAPI_NetLedPlatConnectStatusSet(SC_NETLED_PLATFORM_UNCONNECTED);
                PrintfResp("\r\nSet platform unconnection status!!!\r\n");
                if (SC_NETLED_RETURN_CODE_ERROR == ret)
                {
                    sAPI_Debug("%s: The flag of setting platform unconnection status is error!",__func__);
                }
                break;
            }
            case 2:
            {
                ret=sAPI_NetLedPlatConnectStatusSet(SC_NETLED_PLATFORM_CONNECTED);
                PrintfResp("\r\nSet platform connection status!!!\r\n");
                if (SC_GNSS_RETURN_CODE_ERROR == ret)
                {
                    sAPI_Debug("%s: The flag of setting platform connection status is error!",__func__);
                }
                break;
            }
            case 99:
            {
                return;
            }
            default:
                break;
        }
    }
#endif
}
void NetLedPlatConnectDemo(void)
{
#if 0
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    int opt = 0;
    int i;
    UINT32 ret = 0;
    SC_NETLED_PLATFORM_CONNECTED_Status connectStatus;
    char cgactresp[10]={0};
    //INT8 *note = "\r\nPlease select an option to test from the items listed below, demo just seting the status of the network light status connected to platform.\r\n";
    INT8 *note = "\r\n1.Get NetLedPlatConnectStatus   2.Set NetLedPlatConnectStatus   99.back.\r\n";

    while(1)
    {
        PrintfResp(note);
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }
        sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);
        switch(opt)
        {
            case 1:
            {
                connectStatus = sAPI_NetLedPlatConnectStatusGet();
                switch(connectStatus)
                {
                    case SC_NETLED_PLATFORM_UNCONNECTED:
                        PrintfResp("\r\nUnconnected to the platform!!\r\n");
                        break;
                    case SC_NETLED_PLATFORM_CONNECTED:
                        PrintfResp("\r\nConnected to the platform!!\r\n");
                        break;
                    default:
                        sAPI_Debug("Get the status of connected to platform falied!");
                        PrintfResp("\r\nError!\r\n");
                        break;
                }
                break;
            }
            case 2:
            {
                SetNetLedPlatConnectStatus();
                break;
            }
            case 99:
            {
                return;
            }
            default :
                break;
        }
    }
#endif
}


