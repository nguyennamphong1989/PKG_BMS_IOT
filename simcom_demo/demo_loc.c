#include "simcom_api.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"


extern sMsgQRef simcomUI_msgq;
extern void PrintfOptionMenu(INT8* options_list[], int array_size);
extern void PrintfResp(INT8* format);
int lbsCurrentTime = 1;
int lbsTestTime = 0;
extern void LbsTestDemoInit(void);



sMsgQRef SC_LbsResp_msgq;

void LbsDemo(void)
{
    INT16 channel = 0;
    INT16 type;
    INT32 pGreg = 0;
    UINT32 opt = 0;
    SC_LBS_RETURNCODE ret;
    
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    SC_STATUS status;
    INT8 *note = "\r\nPlease select an option to test from the items listed below.\r\n\r\nPlease make sure the IMEI has set before LBS(AT+SIMEI=IMEI)\r\n";
    INT8 *options_list[] = {
    "1. get longitude and latitude",
    "2. get detail address",
    "3. get error number",
    "4. get longitude latitude and date time",
    "5. test the API for LBS",
    "99. back",
     }; 
    
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

        if(SC_LbsResp_msgq == NULL)
        {
            status = sAPI_MsgQCreate(&SC_LbsResp_msgq, "SC_LbsResp_msgq", (sizeof(SIM_MSG_T)), 4, SC_FIFO);
            if(SC_SUCCESS != status)
            {
                sAPI_Debug("osaStatus = [%d],create msg error!",status);
            }
        }
        else
        {
            sAPI_Debug("msg has been created!"); 
        }

        switch(opt)
        {
            case  SC_LBS_GET_LONLAT:
                {
                    type = 1;
                    INT8 path[300] = {0};
                    ret = sAPI_LocGet(channel, SC_LbsResp_msgq, type);
                    if(ret != SC_LBS_SUCCESS)
                    {
                        sAPI_Debug("get location fail!\r\n");
                        PrintfResp("\r\nLBS get longitude and latitude Fail!\r\n");
                        break;
                    }
                    else
                    {
                        sAPI_Debug("get location success!\r\n");
                        SIM_MSG_T msgQ_data_recv = {0, 0, 0, NULL};                 
                        SC_lbs_info_t *sub_data = NULL;
                        SC_STATUS osaStatus = sAPI_MsgQRecv(SC_LbsResp_msgq, &msgQ_data_recv, SC_SUSPEND); 
                        if(SC_SUCCESS != osaStatus)
                        {
                            sAPI_Debug("osaStatus = [%d],recv msg error!",osaStatus);
                            PrintfResp("\r\nLBS get longitude and latitude Fail!\r\n");
                            break;
                        }
                        else
                        {
                            if(msgQ_data_recv.msg_id == SC_SRV_LBS)
                            {
                                sub_data = (SC_lbs_info_t *)msgQ_data_recv.arg3;
                                if(sub_data->u8ErrorCode != 0)
                                {
                                    snprintf(path,sizeof(path),"\r\nLBS get longitude and latitude Fail!Recv LBS info:errocode:%d\r\n",sub_data->u8ErrorCode);
                                    PrintfResp(path); 
                                    sAPI_Free(sub_data);
                                    
                                    break;
                                }
                                else
                                {
                                    sAPI_Snprintf(path,sizeof(path),"\r\nRecv LBS info:errocode:%d,Lng:%f,Lat:%f,Acc:%d\r\n",sub_data->u8ErrorCode,(float)(sub_data->u32Lng) / 1000000,(float)(sub_data->u32Lat) / 1000000,sub_data->u16Acc);
                                    sAPI_Debug("%s",path);
                                    sAPI_Free(sub_data);
                                    PrintfResp(path);
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }
            
            case SC_LBS_GET_DETAILADDRESS:
                {
                    type = 2;
                    INT8 path[300] = {0};
                    ret = sAPI_LocGet(channel, SC_LbsResp_msgq, type);
                    if(ret != SC_LBS_SUCCESS)
                    {
                        sAPI_Debug("get location fail!\r\n");
                        PrintfResp("\r\nLBS get detail address Fail!\r\n");
                        break;
                    }
                    else
                    {
                        sAPI_Debug("get location success!\r\n");
                        SIM_MSG_T msgQ_data_recv = {0, 0, 0, NULL};                 
                        SC_lbs_info_t *sub_data = NULL;

                        
                        SC_STATUS osaStatus = sAPI_MsgQRecv(SC_LbsResp_msgq, &msgQ_data_recv, SC_SUSPEND); 
                        if(SC_SUCCESS != osaStatus)
                        {
                            sAPI_Debug("osaStatus = [%d],recv msg error!",osaStatus);
                            PrintfResp("\r\nLBS get detail address Fail!\r\n");
                            break;
                        }
                        else
                        {
                            if(msgQ_data_recv.msg_id == SC_SRV_LBS)
                            {
                                sub_data = (SC_lbs_info_t *)msgQ_data_recv.arg3;
                                if(sub_data->u8ErrorCode != 0)
                                {
                                    snprintf(path,sizeof(path),"\r\nLBS get detail address Fail!Recv LBS info:errocode:%d\r\n",sub_data->u8ErrorCode);
                                    sAPI_Free(sub_data);

                                    PrintfResp(path); 
                                    
                                    break;
                                }
                                else
                                {
                                    INT8 tempBuf[151];
                                    memset(tempBuf,0,151);
                                    INT8 *p_tmp=tempBuf;
                                    UINT16 i;	
                                    for(i=0; i<sub_data->u32AddrLen; i++)
                                    {
                                        p_tmp+= sprintf(p_tmp,"%02x", sub_data->u8LocAddress[i]); 
                                        sAPI_Debug("%02x",sub_data->u8LocAddress[i]);
                                    }
                                    snprintf(path,sizeof(path),"\r\nRecv LBS info:errocode:%d,detail_addr:%s\r\n",sub_data->u8ErrorCode,tempBuf);
                                    sAPI_Free(sub_data);
                                    PrintfResp(path);
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }
            
            case SC_LBS_GET_ERRNO:
                {
                    type = 3;
                    INT8 path[300] = {0};
                    ret = sAPI_LocGet(channel, SC_LbsResp_msgq, type);
                    if(ret != SC_LBS_SUCCESS)
                    {
                        sAPI_Debug("get location fail!\r\n");
                        PrintfResp("\r\nLBS get error number Fail!\r\n");
                        break;
                    }
                    else
                    {
                        sAPI_Debug("get location success!\r\n");
                        SIM_MSG_T msgQ_data_recv = {0, 0, 0, NULL};                 
                        SC_lbs_info_t *sub_data = NULL;
                        SC_STATUS osaStatus = sAPI_MsgQRecv(SC_LbsResp_msgq, &msgQ_data_recv, SC_SUSPEND); 
                        if(SC_SUCCESS != osaStatus)
                        {
                            sAPI_Debug("osaStatus = [%d],recv msg error!",osaStatus);
                            PrintfResp("\r\nLBS get error number Fail!\r\n");
                            break;
                        }
                        else
                        {
                            if(msgQ_data_recv.msg_id == SC_SRV_LBS)
                            {
                                sub_data = (SC_lbs_info_t *)msgQ_data_recv.arg3;
                                if(sub_data->u8ErrorCode != 0)
                                {
                                    snprintf(path,sizeof(path),"\r\nLBS get error number Fail!Recv LBS info:errocode:%d\r\n",sub_data->u8ErrorCode);
                                    sAPI_Free(sub_data);
                                    PrintfResp(path); 
                                    
                                    break;
                                }
                                else
                                {
                                    snprintf(path,sizeof(path),"\r\nRecv LBS info:errocode:%d\r\n",sub_data->u8ErrorCode);
                                    sAPI_Free(sub_data);

                                    PrintfResp(path);
                                    
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }
            case SC_LBS_GET_LONLATTIME:
                {
                    type = 4;
                    INT8 path[300] = {0};
                    ret = sAPI_LocGet(channel, SC_LbsResp_msgq, type);
                    if(ret != SC_LBS_SUCCESS)
                    {
                        sAPI_Debug("get location fail!\r\n");
                        PrintfResp("\r\nLBS get longitude latitude and date time Fail!\r\n");
                        break;
                    }
                    else
                    {
                        sAPI_Debug("get location success!\r\n");
                        SIM_MSG_T msgQ_data_recv = {0, 0, 0, NULL};                 
                        SC_lbs_info_t *sub_data = NULL;
                        SC_STATUS osaStatus = sAPI_MsgQRecv(SC_LbsResp_msgq, &msgQ_data_recv, SC_SUSPEND); 
                        if(SC_SUCCESS != osaStatus)
                        {
                            sAPI_Debug("osaStatus = [%d],recv msg error!",osaStatus);
                            PrintfResp("\r\nLBS get longitude latitude and date time Fail!\r\n");
                            break;
                        }
                        else
                        {
                            if(msgQ_data_recv.msg_id == SC_SRV_LBS)
                            {
                                sub_data = (SC_lbs_info_t *)msgQ_data_recv.arg3;
                                if(sub_data->u8ErrorCode != 0)
                                {
                                    snprintf(path,sizeof(path),"\r\nLBS get longitude latitude and date time Fail!Recv LBS info:errocode:%d\r\n",sub_data->u8ErrorCode);
                                    sAPI_Debug("%s",path);
                                    sAPI_Free(sub_data);
                                    PrintfResp(path); 
                                    break;
                                }
                                else
                                {   
                                    snprintf(path,sizeof(path),"\r\nRecv LBS info:errocode %d,Lng:%ld.%ld,Lat:%ld.%ld,Acc:%d,DateAndTime:%s\r\n",sub_data->u8ErrorCode,(sub_data->u32Lng)/1000000,(sub_data->u32Lng)%1000000,(sub_data->u32Lat)/1000000,(sub_data->u32Lat)%1000000, sub_data->u16Acc,sub_data->u8DateAndTime);
                                    sAPI_Free(sub_data);
                                    PrintfResp(path);
                                    break;
                                }
                         
                            }
                        }
                    }
                    break;
                }
            case SC_LBS_TEST:
                {
                    sAPI_Debug("Start lbs demo test thread!");
                    PrintfResp("\r\nPlease input test time.\r\n");                    
                    //optionMsg = GetParamFromUart();
                    SIM_MSG_T option_Msg ={0,0,0,NULL};
                    sAPI_MsgQRecv(simcomUI_msgq,&option_Msg,SC_SUSPEND);
                    sAPI_Debug("arg3 = [%s]",option_Msg.arg3);
                    lbsTestTime = atoi(option_Msg.arg3);
                    lbsCurrentTime = 1;
                    sAPI_Debug("lbsTestTime:%d,lbsCurrentTime:%d",lbsTestTime,lbsCurrentTime);
                    sAPI_Free(option_Msg.arg3);
                    
                    LbsTestDemoInit();
                    break;
                }
            case SC_LBS_DEMO_MAX:
                {
                    return;
                }

            default :
                 break;
        }
    } 
}

#if 0
 

#include "simcom_api.h"


static sTaskRef SC_LbsTask = NULL;
static UINT32 SC_LbsTaskStack[4096*4] = {0};

int all_type;
sMsgQRef SC_LbsResp_msgq;

static void sAPP_LbsProcesser(void * arg)
{
    int channel = 0;
    int type;
    all_type = type;
    UINT8 pGreg = 0;
    SC_LBS_RETURNCODE ret; 
    
    while(1)
    {
        sAPI_NetworkGetCgreg(&pGreg);
        if(1 != pGreg)
        {
            sAPI_Debug("LBS NETWORK STATUS IS [%d]",pGreg);
            sAPI_TaskSleep(10*300);
        }
        else
        {
            sAPI_Debug("LBS NETWORK STATUS IS NORMAL");
            break;
        }
    }

    type = 1;
    ret = sAPI_LocGet(channel, SC_LbsResp_msgq, type);
    if(ret != SC_LBS_SUCCESS)
    {
        sAPI_Debug("get location fail!\r\n");
    }
    else
    {
        sAPI_Debug("get location success!\r\n");
        SIM_MSG_T msgQ_data_recv = {0, 0, 0, NULL};                 
        SC_lbs_info_t *sub_data = NULL;
        SC_STATUS osaStatus = sAPI_MsgQRecv(SC_LbsResp_msgq, &msgQ_data_recv, SC_SUSPEND); 
        if(SC_SUCCESS != osaStatus)
        {
            sAPI_Debug("osaStatus = [%d],recv msg error!",osaStatus);
        }
        else
        {
            if(msgQ_data_recv.msg_id == SC_SRV_LBS)
            {
                sub_data = (SC_lbs_info_t *)msgQ_data_recv.arg3;
                if(sub_data->u8ErrorCode != 0)
                {
                    sAPI_Debug("Recv LBS info:errocode:%d",sub_data->u8ErrorCode);
                    sAPI_Free(sub_data);
                }
                else
                {
                    sAPI_Debug("Recv LBS info:errocode:%d,Lng:%f,Lat:%f,Acc:%d",sub_data->u8ErrorCode,(float)(sub_data->u32Lng)/1000000,(float)(sub_data->u32Lat)/1000000,sub_data->u16Acc);
                    sAPI_Free(sub_data);
                }
            }
        }
    }

    type = 2;
    ret = sAPI_LocGet(channel, SC_LbsResp_msgq, type);
    if(ret != SC_LBS_SUCCESS)
    {
        sAPI_Debug("get location fail!\r\n");
    }
    else
    {
        sAPI_Debug("get location success!\r\n");
        SIM_MSG_T msgQ_data_recv = {0, 0, 0, NULL};                 
        SC_lbs_info_t *sub_data = NULL;
        SC_STATUS osaStatus = sAPI_MsgQRecv(SC_LbsResp_msgq, &msgQ_data_recv, SC_SUSPEND); 
        if(SC_SUCCESS != osaStatus)
        {
            sAPI_Debug("osaStatus = [%d],recv msg error!",osaStatus);
        }
        else
        {
            if(msgQ_data_recv.msg_id == SC_SRV_LBS)
            {
                sub_data = (SC_lbs_info_t *)msgQ_data_recv.arg3;
                if(sub_data->u8ErrorCode != 0)
                {
                    sAPI_Debug("sub_data->u8ErrorCode:%d",sub_data->u8ErrorCode);
                    sAPI_Free(sub_data);
                }
                else
                {
                    char tempBuf[151];
                    memset(tempBuf,0,151);
                    char *p_tmp=tempBuf;
    	            unsigned int i;	
                    for(i=0; i<sub_data->u32AddrLen; i++)
        			{
        				p_tmp+= sprintf(p_tmp,"%02x", sub_data->u8LocAddress[i]); 
                        sAPI_Debug("%02x",sub_data->u8LocAddress[i]);
        			}
                    sAPI_Debug("Recv LBS info:errocode:%d,detail_addr:%s",sub_data->u8ErrorCode,tempBuf);
                    sAPI_Free(sub_data);
                }
            }
        }
    }
    
    type = 3;
    ret = sAPI_LocGet(channel, SC_LbsResp_msgq, type);
    if(ret != SC_LBS_SUCCESS)
    {
        sAPI_Debug("get location fail!\r\n");
    }
    else
    {
        sAPI_Debug("get location success!\r\n");
        SIM_MSG_T msgQ_data_recv = {0, 0, 0, NULL};                 
        SC_lbs_info_t *sub_data = NULL;
        SC_STATUS osaStatus = sAPI_MsgQRecv(SC_LbsResp_msgq, &msgQ_data_recv, SC_SUSPEND); 
        if(SC_SUCCESS != osaStatus)
        {
            sAPI_Debug("osaStatus = [%d],recv msg error!",osaStatus);
        }
        else
        {
            if(msgQ_data_recv.msg_id == SC_SRV_LBS)
            {
                sub_data = (SC_lbs_info_t *)msgQ_data_recv.arg3;
                if(sub_data->u8ErrorCode != 0)
                {
                    sAPI_Debug("Recv LBS info:errocode:%d",sub_data->u8ErrorCode);
                    sAPI_Free(sub_data);
                }
                else
                {
             
                sAPI_Debug("Recv LBS info:errocode:%d",sub_data->u8ErrorCode);
                sAPI_Free(sub_data);
                    }
            }
        }
    }

    type = 4;
    ret = sAPI_LocGet(channel, SC_LbsResp_msgq, type);
    if(ret != SC_LBS_SUCCESS)
    {
        sAPI_Debug("get location fail!\r\n");
    }
    else
    {
        sAPI_Debug("get location success!\r\n");
        SIM_MSG_T msgQ_data_recv = {0, 0, 0, NULL};                 
        SC_lbs_info_t *sub_data = NULL;
        SC_STATUS osaStatus = sAPI_MsgQRecv(SC_LbsResp_msgq, &msgQ_data_recv, SC_SUSPEND); 
        if(SC_SUCCESS != osaStatus)
        {
            sAPI_Debug("osaStatus = [%d],recv msg error!",osaStatus);
        }
        else
        {
            if(msgQ_data_recv.msg_id == SC_SRV_LBS)
            {
                sub_data = (SC_lbs_info_t *)msgQ_data_recv.arg3;
                if(sub_data->u8ErrorCode != 0)
                {
                    sAPI_Debug("Recv LBS info:errocode:%d",sub_data->u8ErrorCode);
                    sAPI_Free(sub_data);
                }
                else
                {
             
                sAPI_Debug("Recv LBS info:errocode %d,Lng:%f,Lat:%f,Acc:%d,DateAndTime:%s",sub_data->u8ErrorCode,(float)(sub_data->u32Lng)/1000000,\
                    (float)(sub_data->u32Lat)/1000000, sub_data->u16Acc,sub_data->u8DateAndTime); 
                sAPI_Free(sub_data);
                    }
            }
        }
    }

}

void sAPP_LbsTaskDemo(void)
{
    sAPI_Debug("LBS task init\r\n");
    
    BOOL is_inited = 0;
    if(1 == is_inited)
    return;
    
    SC_STATUS status;
    status = sAPI_MsgQCreate(&SC_LbsResp_msgq, "SC_LbsResp_msgq", (sizeof(SIM_MSG_T)), 4, SC_FIFO);
    if(SC_SUCCESS != status)
    {
        sAPI_Debug("osaStatus = [%d],create msg error!",status);
    }
    
    sAPI_TaskCreate(&SC_LbsTask, SC_LbsTaskStack, 4096 * 4,70, (char*)"lbs_demo_thread", sAPP_LbsProcesser, (void *)0);

    if(SC_SUCCESS == status)
    is_inited = 1;

}

#endif
