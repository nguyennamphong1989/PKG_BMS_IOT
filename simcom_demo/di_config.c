#include "simcom_api.h"
#include "simcom_system.h"
#include "simcom_ping.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#define MQTT_SEND_TIME (30*200) //30 secs
#define LED_BLINK_TIME (3*200) //3 secs
#define SECTOR_1 0
#define SECTOR_2 4096
#define ERASE_SIZE 4096
char loggerID[8] = "";
UINT16 version = 9;
/*
Từ khóa "extern" chỉ ra 1 biến hoặc 1 hàm đã được khai báo ở 1 nơi
khác trong mã nguồn.
*/
extern void PrintfResp(INT8 *format); // Đã khai báo trong hàm simcom_os.h
extern sMsgQRef urc_mqtt_msgq_1; // Đã khai báo trong hàm simcom_demo.h
INT8 request[21] = {0x4e,0x57, 0,0x13, 0,0,0,0, 0x06, 0x03, 0, 0, 0,0,0,0, 0x68, 0,0,0x01,0x29};

extern sMsgQRef simcomUI_msgq; // Đã khai báo trong hàm simcom_demo.h
sMsgQRef uart_mqtt_msgq_1;
// INT8 dataBMS[1024] = {0};
char test_payload[200] = {0};
UINT8 parameters_update = 0; 
BOOL first_start = 1;
INT32 BMS_tick;
INT32 LED_tick;
char imei_value[16];
UINT8 DMS_state= 0; // updated by MQTT control
UINT8 DMS_is_diff=0;

void GetCSQ()
{
    UINT8 csq;
    UINT8 ret;
    char NetResp[100]={0};
    ret = sAPI_NetworkGetCsq(&csq);
    if(ret == SC_NET_SUCCESS)
    {
        sprintf(NetResp,"\n Get csq success. csq:%d!",csq); // Định dạng mảng và in vào chuỗi NetResp
        sAPI_UartWrite(SC_UART3, NetResp,strlen(NetResp));
    }
    else
    {
        sAPI_UartWrite(SC_UART3, "\n Get csq falied!",18);
    }
}

void GetCnmp()
{
    UINT8 ret;
    int cnmp;
    char cnmpresp[50];    
    ret = sAPI_NetworkGetCnmp(&cnmp);
    if(ret == SC_NET_SUCCESS)
    {
        sprintf(cnmpresp,"\n Get cnmp success. cnmp:%d!",cnmp);
        PrintfResp(cnmpresp);
    }
    else
    {
        PrintfResp("\n Get cnmp falied!");
    }
}

void GetCgpaddr()
{
    UINT8 ret;
    SCcgpaddrParm cgpaddrParm;
    char NetResp[200];

    memset(&cgpaddrParm,0,sizeof(cgpaddrParm));
    ret = sAPI_NetworkGetCgpaddr(15,&cgpaddrParm);
    if(ret == SC_NET_SUCCESS)
    {
        sprintf(NetResp,"\n Get Ipaddr success. cid=%d,type=%d,ipv4=%s,ipv6=%s!",cgpaddrParm.cid,cgpaddrParm.iptype,cgpaddrParm.ipv4addr,cgpaddrParm.ipv6addr);
        PrintfResp(NetResp);
    }
    else
    {
        PrintfResp("\n Get Ipaddr falied!");
    }    
}

void GetCreg()
{
    UINT8 ret;
    int creg;   
    char NetResp[200];        
    ret = sAPI_NetworkGetCreg(&creg);
    if(ret == SC_NET_SUCCESS)
    {
        sprintf(NetResp,"\n Get creg success. creg=%d!",creg);
        PrintfResp(NetResp);
    }
    else
    {
        PrintfResp("\n Get creg falied!");
    }
}

void GetCGreg()
{
    UINT8 ret;
    int cgreg;   
    char NetResp[200];    
    ret = sAPI_NetworkGetCgreg(&cgreg);
    if(ret == SC_NET_SUCCESS)
    {
        sprintf(NetResp,"\n Get cgreg success. cgreg=%d!",cgreg);
        PrintfResp(NetResp);
    }
    else
    {
        PrintfResp("\n Get cgreg falied!");
    }
}

void GetCpsi()
{
    UINT8 ret;
    SCcpsiParm Scpsi = { 0 };  
    char NetResp[200];    
    ret = sAPI_NetworkGetCpsi(&Scpsi);
    if(ret == SC_NET_SUCCESS)
    {
        sprintf(NetResp,"\n Get cpsi success. NEmode=%s,MM=%s,LAC=%d,CELL=%d,Gband=%s,Lband=%s,TAC=%d,RSRP=%d,RXLEV=%d,TA=%d,SINR=%d!",Scpsi.networkmode,Scpsi.Mnc_Mcc,Scpsi.LAC,Scpsi.CellID,Scpsi.GSMBandStr,Scpsi.LTEBandStr,Scpsi.TAC,Scpsi.Rsrp,Scpsi.RXLEV,Scpsi.TA,Scpsi.SINR);
        PrintfResp(NetResp);
    }
    else
    {
        PrintfResp("\n Get cpsi falied!");
    }    
}

void Publish_array(char *input)
{
    char *topic = sAPI_Malloc(100);
    memset(topic, 0, 100);
    sprintf(topic, "pkg/%s/data",imei_value);
    sAPI_MqttTopic(0, topic, strlen(topic));
    sAPI_MqttPayload(0, input, strlen(input));
    sAPI_MqttPub(0, 1, 60, 0, 0); 
}

void addMsg(INT8 *buff, char *msg)
{
    INT32 pos = strlen(buff);
    sprintf(buff + pos, "%s", msg);
}

void uart_read()
{
    SIM_MSG_T optionMsg = {0, 0, 0, NULL};
    INT32 totallen = 0;
    INT32 packetlen = 0;
    INT8 nbBattery = 0;
    INT32 pos = 0;
    INT32 bufferpos = 0;
    // memset(dataBMS, 0, 1024);
    INT32 nbMsg;
    INT32 tmp;
    int32_t tmp1;
    char *buffer = sAPI_Malloc(1024);
    char *payload= sAPI_Malloc(6000);
    memset(payload, 0, 6000);
    char temp[300];
    //Clear Queue
    sAPI_MsgQPoll(simcomUI_msgq, &nbMsg);
    while (nbMsg > 0)
    {
        sAPI_MsgQRecv(simcomUI_msgq, &optionMsg, 200);
        sAPI_Free(optionMsg.arg3);
        sAPI_MsgQPoll(simcomUI_msgq, &nbMsg);
    }
    //print test
    // memset(temp, 0, 50);
    sprintf(temp, "\n Queue-nbMsg = %d", nbMsg);
    sAPI_UartWrite(SC_UART3, temp,strlen(temp));

    sAPI_UartWrite(SC_UART, request, 21);
    sAPI_TaskSleep(200);
    sAPI_MsgQPoll(simcomUI_msgq, &nbMsg);
    // print test
    memset(temp, 0, 50);
    sprintf(temp, "\n Readall nbMsg = %d", nbMsg);
    sAPI_UartWrite(SC_UART3, temp,strlen(temp));

    while (nbMsg > 0)
    {
        sAPI_MsgQRecv(simcomUI_msgq, &optionMsg, 200);
        memcpy(buffer + bufferpos, optionMsg.arg3, optionMsg.arg2);
        bufferpos += optionMsg.arg2;
        sAPI_MsgQPoll(simcomUI_msgq, &nbMsg);
    }

    // sAPI_Free(optionMsg.arg3);
    packetlen = bufferpos;
    //print test
    // memset(temp, 0, 50);
    // sprintf(temp, "\n BMS-RspLength = %d", packetlen);
    // sAPI_UartWrite(SC_UART3, temp,strlen(temp));
    // sAPI_UartWrite(SC_UART3,"\n buffer:",9);
    // memset(temp, 0, 50);    
    // for(bufferpos=0;bufferpos<packetlen;bufferpos++)
    // {
    //     sprintf(temp,"%02X ", buffer[bufferpos]);
    //     sAPI_UartWrite(SC_UART3,temp,strlen(temp));
    // }

    bufferpos += 11;
    SCsysTime_t t1;
    sAPI_GetSysLocalTime(&t1);

    bufferpos = 0;
    if (buffer[bufferpos] != 0x4e || buffer[bufferpos + 1] != 0x57) // 2 bytes header
    {
        tmp = sAPI_GetTicks()/200;
        sprintf(payload, "%4d%02d%02d%02d%02d%02d\tMCU:%d\tVER:%d",t1.tm_year,t1.tm_mon,t1.tm_mday,t1.tm_hour,t1.tm_min,t1.tm_sec,tmp,version);
        Publish_array(payload);
        sAPI_Free(payload);
        return;
    }
        
    totallen = (buffer[bufferpos + 2] << 8) + (buffer[bufferpos + 3]) + 2; // 2 byte length
    if (totallen != packetlen)
    {
        return;
    }
        

    while (bufferpos < totallen)
        switch (buffer[bufferpos])
        {
        case 0x79:
            nbBattery = buffer[bufferpos + 1] / 3;
            bufferpos += 2;
            sprintf(payload, "%4d%02d%02d%02d%02d%02d\tCV1:",t1.tm_year,t1.tm_mon,t1.tm_mday,t1.tm_hour,t1.tm_min,t1.tm_sec);
            for (INT8 i = 0; i < nbBattery; i++)
            {
                
                tmp = (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
                if(i==0)
                {
                    sprintf(temp,"%d",tmp);
                    addMsg(payload,temp);
                }
                else
                {
                    sprintf(temp,"\tCV%d:%d",i+1,tmp);
                    addMsg(payload,temp);
                }
                bufferpos += 3;  
            }
            // addMsg(payload, "]}");    
            break;
        case 0x80:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 100);            
            sprintf(temp, "\tTT:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;
        case 0x81:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 100);    
            sprintf(temp, "\tBOT:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;
        case 0x82:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 100);  
            sprintf(temp, "\tBAT:%d",tmp);  
            addMsg(payload,temp);
            bufferpos += 3;
            break;
        case 0x83:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 100);    
            sprintf(temp, "\tBV:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;
        case 0x84:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            if(tmp>(1<<15)) tmp = tmp - (1<<15);
            memset(temp, 0, 100);    
            sprintf(temp, "\tBCU:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;
        case 0x85:
            tmp= buffer[bufferpos + 1] ;
            memset(temp, 0, 100);    
            sprintf(temp, "\tBP:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 2;
            break;
        case 0x86:
            bufferpos += 2;
            break;
        case 0x87:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 100);  
            sprintf(temp, "\tBCY:%d",tmp);  
            addMsg(payload,temp);
            bufferpos += 3;            
            break;
        case 0x89:        
            tmp = (buffer[bufferpos + 1] << 24) + (buffer[bufferpos + 2] << 16) + (buffer[bufferpos + 3] << 8) + buffer[bufferpos + 4];
            memset(temp, 0, 300);    
            sprintf(temp, "\tBCA:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 5;
            break;            
        case 0x8a:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 100);   
            sprintf(temp, "\tTS:%d",tmp); 
            addMsg(payload,temp);
            bufferpos += 3;     
            break;            
        case 0x8b:
            tmp = (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            if (tmp > 0)
            {
                memset(temp, 0, 300);
                addMsg(payload, "\tW:");
                if (tmp & 0x0001)
                    addMsg(temp, "LOW CAPACITY,");
                if (tmp & 0x0002)
                    addMsg(temp, "TUBE OVERTEMP,");
                if (tmp & 0x0004)
                    addMsg(temp, "CHARGE OVERVOLT,");
                if (tmp & 0x0008)
                    addMsg(temp, "DISCHAR UNDERVOL,");
                if (tmp & 0x0010)
                    addMsg(temp, "BATT OVERTEMP,");
                if (tmp & 0x0020)
                    addMsg(temp, "CHARGE OVERCURR,");
                if (tmp & 0x0040)
                    addMsg(temp, "DISCHAR OVERCURR,");
                if (tmp & 0x0080)
                    addMsg(temp, "CELL DIFFER,");
                if (tmp & 0x0100)
                    addMsg(temp, "BOX OVERTEMP,");
                if (tmp & 0x0200)
                    addMsg(temp, "BATT LOWTEMP,");
                if (tmp & 0x0400)
                    addMsg(temp, "MONO OVERVOLT,");
                if (tmp & 0x0800)
                    addMsg(temp, "MONO UNDERVOL,");
                if (tmp & 0x1000)
                    addMsg(temp, "309A PROTECT,");
                if (tmp & 0x2000)
                    addMsg(temp, "309B PROTECT,");
                // addMsg(temp, "\"}");
                addMsg(payload,temp);
            }
            else
            {
                memset(temp, 0, 300);
                addMsg(payload, "\tW:NORMAL");
                addMsg(payload,temp);
            }
            bufferpos += 3;
            break;           
        case 0x8c:
            // tmp = buffer[bufferpos + 2];
            // memset(temp, 0, 300);
            // addMsg(payload, "\tSI:");
            // if (tmp & 0x01) addMsg(temp, "Charge MOS ON,");
            // else addMsg(temp, "Charge MOS OFF,");
            // if (tmp & 0x02) addMsg(temp, "Discharge MOS ON,");
            // else addMsg(temp, "Disharge MOS OFF,");    
            // if (tmp & 0x04) addMsg(temp, "Balance ON,");
            // else addMsg(temp, "Balance OFF,");    
            // if (tmp & 0x08) addMsg(temp, "Batt is CONNECTED,");
            // else addMsg(temp, "Batt is DISCONNECTED,");
            // addMsg(payload,temp);            
            bufferpos += 3;
            break;    
        case 0x8e:
            // tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            // memset(temp, 0, 300);    
            // sprintf(temp, ",{\"pc\":\"total_overvoltage\",\"lid\":\"1\",\"d\":%d}",tmp);
            // addMsg(payload,temp);  
            bufferpos += 3;
            break;
        case 0x8f:
            // tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            // memset(temp, 0, 300);    
            // sprintf(temp, ",{\"pc\":\"total_undervoltage\",\"lid\":\"1\",\"d\":%d}",tmp);
            // addMsg(payload,temp);         
            bufferpos += 3;
            break;       
        case 0x90:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tSO:%d",tmp);
            addMsg(payload,temp);             
            bufferpos += 3;
            break;        
        case 0x91:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);  
            sprintf(temp, "\tMOR:%d",tmp);  
            addMsg(payload,temp); 
            bufferpos += 3;
            break;          
        case 0x92:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);  
            sprintf(temp, "\tSUPV:%d",tmp);  
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0x93:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tMURV:%d",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;        
        case 0x94:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tSUPD:%d",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;                  
        case 0x95:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tDVPDC:%d",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;            
        case 0x96:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tDCPV:%d",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0x97:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tDCPD:%d",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0x98:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tCCPV:%d",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0x99:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tCCPD:%d",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0x9a:
            bufferpos += 3;
            break;    
        case 0x9b:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tEODP:%d",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;        
        case 0x9c:
            bufferpos += 3;
            break;          
        case 0x9d:
            tmp= (buffer[bufferpos + 1]) ;
            memset(temp, 0, 300);           
            sprintf(temp, "\tPTTP:%d",tmp);     
 //0 - OFF, 1 - ON
             addMsg(payload,temp);        
            bufferpos += 2;
            break;         
        case 0x9e:
             bufferpos += 3;
            break;        
        case 0x9f:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tETPV:%d",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0xa0:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tETRV:%d",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;        
        case 0xa1:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tBTDP:%d",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0xa2:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tBCHT:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;           
        case 0xa3:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tBDHT:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;           
        case 0xa4:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, "\tCLTP:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break; 
        case 0xa5:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            if (tmp>(1<<15)) tmp = tmp-(1<<16);                        
            memset(temp, 0, 300);    
            sprintf(temp, "\tRVCL:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;        
        case 0xa6:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];   
            if (tmp>(1<<15)) tmp = tmp-(1<<16);         
            memset(temp, 0, 300);    
            sprintf(temp, "\tDLTP:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;        
        case 0xa7:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            if (tmp>(1<<15)) tmp = tmp-(1<<16);    
            memset(temp, 0, 300);    
            sprintf(temp, "\tRVDL:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;          
        case 0xa8:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            if (tmp>(1<<15)) tmp = tmp-(1<<16);    
            memset(temp, 0, 300);    
            sprintf(temp, "\tDLR:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;
        case 0xa9:
            tmp= buffer[bufferpos + 1];
            memset(temp, 0, 300);    
            sprintf(temp, "\tSS:%d",tmp);
            addMsg(payload,temp);         
            bufferpos += 2;
            break;                   
        case 0xaa:
            tmp = (buffer[bufferpos + 1] << 24) + (buffer[bufferpos + 2] << 16) + (buffer[bufferpos + 3] << 8) + buffer[bufferpos + 4];
            memset(temp, 0, 300);    
            sprintf(temp, "\tBCS:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 5;
            break;         
        case 0xab:
            tmp = buffer[bufferpos + 1];
            memset(temp, 0, 300); 
            sprintf(temp, "\tCMS:%d",tmp);
 //0 - OFF, 1 - ON
            addMsg(payload,temp);
            bufferpos += 2;
            break; 
        case 0xac:
            tmp = buffer[bufferpos + 1];
            // Handle DMS status
            if(tmp!= DMS_state)
            {
                uart_send("DMS",DMS_state);//Lock DMS_state
                tmp = DMS_state;
            }
            memset(temp, 0, 300); 
            sprintf(temp, "\tDMS:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 2;


            break;         
        case 0xad:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);  
            sprintf(temp, "\tCC:%d",tmp);  
            addMsg(payload,temp);
            bufferpos += 3;
            break;        
        case 0xae:
            bufferpos += 2;
            break;          
        case 0xaf:
            tmp = buffer[bufferpos + 1];
            memset(temp, 0, 300);
            sprintf(temp, "\tBT:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 2;
            break;                   
        case 0xb0:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);   
            sprintf(temp, "\tSWT:%d",tmp); 
            addMsg(payload,temp);
            bufferpos += 3;
            break;               
        case 0xb1:
            tmp= buffer[bufferpos + 1];
            memset(temp, 0, 300);    
            sprintf(temp, "\tLCA:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 2;
            break;         
        case 0xb2:
            memset(temp, 0, 300);    
            sprintf(temp, "\tMP:%d",tmp);
            memcpy(temp + 40, buffer + bufferpos + 1, 10);
            addMsg(payload,temp);            
            bufferpos += 11;
            break;
        case 0xb3:
            tmp = buffer[bufferpos + 1];
            memset(temp, 0, 300); 
            sprintf(temp, "\tSCS:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 2;
            break;         
        case 0xb4:
            memcpy(loggerID, buffer + bufferpos + 1, 8);
            bufferpos += 9;
            break;
        case 0xb5:
            bufferpos += 5;
            break;            
        case 0xb6:
            tmp = (buffer[bufferpos + 1] << 24) + (buffer[bufferpos + 2] << 16) + (buffer[bufferpos + 3] << 8) + buffer[bufferpos + 4];
            memset(temp, 0, 300);    
            sprintf(temp, "\tSWT:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 5;
            break;
        case 0xb7:
            memset(temp, 0, 300);    
            sprintf(temp, "\tSVN:%d",tmp);
            memcpy(temp + 36, buffer + bufferpos + 1, 15);
            addMsg(payload,temp);
            bufferpos += 16;
            break;
        case 0xb8:
            tmp = buffer[bufferpos + 1];
            memset(temp, 0, 300); 
            sprintf(temp, "\tSCC:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 2;
            break;          
        case 0xb9:
            tmp = (buffer[bufferpos + 1] << 24) + (buffer[bufferpos + 2] << 16) + (buffer[bufferpos + 3] << 8) + buffer[bufferpos + 4];
            memset(temp, 0, 300); 
            sprintf(temp, "\tACC:%d",tmp);   
            addMsg(payload,temp);
            bufferpos += 5;
            break;
        case 0xba:
            memset(temp, 0, 300);    
            sprintf(temp, "\tFI:%d",tmp);
            memcpy(temp + 35, buffer + bufferpos + 1, 24);
            // addMsg(temp, "\"}");
            addMsg(payload,temp);
            bufferpos += 25;
            break; 
        case 0xbb:
        case 0xbc:
        case 0xbd:
            bufferpos += 2;
            break;                                           
        case 0xbe:
        case 0xbf:
            bufferpos += 3;
            break;
        case 0xc0:
            tmp = buffer[bufferpos + 1];
            memset(temp, 0, 300); 
            sprintf(temp, "\tPV:%d",tmp);
            addMsg(payload,temp);
            bufferpos += 2;            
            break;
        default:
            bufferpos++;
            break;
        }
            
    memset(temp, 0, 300);
    tmp = sAPI_GetTicks()/200;
    sprintf(temp, "\tMCU:%d\tVER:%d",tmp,version);    
    addMsg(payload,temp);
    Publish_array(payload);

    //print test 
    memset(test_payload, 0, sizeof(test_payload));
    sprintf(test_payload, "\n Finish Reading BMS! Send Complete");
    sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));   
    // sAPI_UartWrite(SC_UART3, payload,strlen(payload));
    
    sAPI_Free(buffer);
    sAPI_Free(payload);


}

void uart_send(char *topic, char* payload) // SEND TO BMS and SEND MQTT UPDATE
{
    UINT8 code=0;
    UINT8 size =0;
    UINT16 value=0;
    UINT16 CRC=0;
    char* tmp = sAPI_Malloc(60);
    memset(tmp,0,sizeof(tmp));
    UINT8 *frame; //frame = frame_header + frame_data +frame_footer
    UINT8 frame_header[] = {0x4e,0x57, 0,0x15, 0,0,0,0, 0x02, 3, 0}; //11 bytes
    UINT8 frame_footer[] = {0,0,0,0, 0x68, 0,0,0,0}; //9 bytes
    UINT8 *frame_data;
    char* mqtt_rep = sAPI_Malloc(80);
    char password[31] = {0x4E, 0x57 ,0, 0x1C,0,0,0,0, 0x05, 0x03,0, 0xB2, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36 ,0,0,0,0,0,0,0,0, 0x68,0,0, 0x03, 0x18};
    sAPI_UartWrite(SC_UART,password,31);
    sAPI_TaskSleep(100);   
    SCsysTime_t time_sent;
    sAPI_GetSysLocalTime(&time_sent);
    if(strncmp(topic,"CMS",strlen(topic))==0)// CMS:0/1 -> OFF/ON 
    {
         
        code = 0xab;
        frame_header[3] = 0x14;
        size = 2;
        frame_data=sAPI_Malloc(size);
        frame=sAPI_Malloc(20 + size);
        memset(frame,0,20 + size);
        memset(frame_data,0,size);
        UINT8 lowByte;
        UINT8 highByte;
        char read_chargMOS[21] = {0x4E, 0x57, 0x00, 0x13, 0,0,0,0, 0x03, 0x03, 0, 0xAB,0,0,0,0, 0x68, 0, 0, 0x01, 0xD1};
        
        value= atoi(payload);
        if(value== 1) lowByte =1;
        if(value== 0) lowByte =0;
        
        frame_data[0] = code;
        frame_data[1] = lowByte;
           

        memcpy(frame,frame_header,11);
        memcpy(frame + 11,frame_data,size);
        memcpy(frame + 11 +size,frame_footer,9);

        //CRC 
        for(UINT8 i=0; i<20+size;i++)
        {
            CRC += frame[i];
        }
        lowByte = (UINT8) (CRC & 0xff);
        highByte = (UINT8) ((CRC >> 8) & 0xff);
        frame[18+size] = highByte;
        frame[19+size] = lowByte;

        // Print test
        sAPI_UartWrite(SC_UART3,"\r\n Update BMS: ",15);
        for(INT8 i=0; i<20+size; i++)
        {
            sprintf(tmp,"%02X ", frame[i]);
            sAPI_UartWrite(SC_UART3,tmp,strlen(tmp));
        }
        // Send to BMS
        sAPI_UartWrite(SC_UART,frame,(20 + size));
        sAPI_TaskSleep(200);
        sAPI_UartWrite(SC_UART,frame,(20 + size));
        sAPI_TaskSleep(200);
        
        // memset(mqtt_rep,0,sizeof(mqtt_rep));
        // sprintf(mqtt_rep, "%4d%02d%02d%02d%02d%02d\tCMS:%d",time_sent.tm_year,time_sent.tm_mon,time_sent.tm_mday,time_sent.tm_hour,time_sent.tm_min,time_sent.tm_sec,value);
        // Publish_array(mqtt_rep);

    }
    if(strncmp(topic,"DMS",strlen(topic))==0)
    {
        
        code = 0xac;
        size = 2;
        frame_header[3] = 0x14;
        frame_data=sAPI_Malloc(size);
        frame=sAPI_Malloc(20 + size);
        memset(frame,0,20 + size);
        memset(frame_data,0,size);
        char read_dischargMOS[21] = {0x4E, 0x57, 0x00, 0x13, 0,0,0,0, 0x03, 0x03, 0, 0xAC,0,0,0,0, 0x68, 0, 0, 0x01, 0xD2};
        UINT8 lowByte;
        UINT8 highByte;
        
        value= atoi(payload);
        DMS_state = value;
    
        
        int ret = sAPI_EraseFlashSector(SECTOR_1,ERASE_SIZE);
        ret = sAPI_WriteFlash(SECTOR_1, (UINT8*)&DMS_state,sizeof(DMS_state));
        if(ret==0)
        {
            sAPI_UartWrite(SC_UART3,"\r\n Save DMS state successfully",30);
            ret = sAPI_ReadFlash(SECTOR_1, (UINT8*)&DMS_state, sizeof(DMS_state));
            if(ret==0)
            {
                memset(test_payload, 0, sizeof(test_payload));
                sprintf(test_payload, "\n DMS state = %d",DMS_state);
                sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
            }
            else
            {
                sAPI_UartWrite(SC_UART3,"\r\n Load DMS state Failed",24);
                DMS_state=0;
            }
        }
        else
        {
            sAPI_UartWrite(SC_UART3,"\r\n Save DMS state Failed",24);
        }
      
        if(value== 1) lowByte =1;
        if(value== 0) lowByte =0;
        
        frame_data[0] = code;
        frame_data[1] = lowByte;

        // Print test
        // sprintf(tmp,"\n\rdata: %02X %02X %02X", frame_data[0],frame_data[1],frame_data[2])   ;
        // sAPI_UartWrite(SC_UART,tmp,strlen(tmp));            

        memcpy(frame,frame_header,11);
        memcpy(frame + 11,frame_data,size);
        memcpy(frame + 11 +size,frame_footer,9);

        //CRC 
        for(UINT8 i=0; i<20+size;i++)
        {
            CRC += frame[i];
        }
        lowByte = (UINT8) (CRC & 0xff);
        highByte = (UINT8) ((CRC >> 8) & 0xff);
        frame[18+size] = highByte;
        frame[19+size] = lowByte;

        // Print test
        sAPI_UartWrite(SC_UART3,"\r\n Update BMS: ",15);
        for(INT8 i=0; i<20+size; i++)
        {
            sprintf(tmp,"%02X ", frame[i]);
            sAPI_UartWrite(SC_UART3,tmp,strlen(tmp));
        }
        // Send to BMS
        sAPI_UartWrite(SC_UART,frame,(20 + size));
        sAPI_TaskSleep(200);
        sAPI_UartWrite(SC_UART,frame,(20 + size));
        sAPI_TaskSleep(200);       

        // memset(mqtt_rep,0,sizeof(mqtt_rep));
        // sprintf(mqtt_rep, "%4d%02d%02d%02d%02d%02d\tDMS:%d",time_sent.tm_year,time_sent.tm_mon,time_sent.tm_mday,time_sent.tm_hour,time_sent.tm_min,time_sent.tm_sec,value);
        // Publish_array(mqtt_rep);

    }
    if(strncmp(topic,"OTA",strlen(topic)) == 0){
        char url[100] = {0};
        memset(url, 0, 100);
        sprintf(url,"http://white-dev.aithings.vn:13579/%s",payload);
        otaUpdate(url);
    }
    sAPI_Free(tmp);
    sAPI_Free(frame);
    sAPI_Free(mqtt_rep);
    sAPI_TaskSleep(200); // wait BMS finish sending response
}

void otaUpdate(INT8 *url)
{
    SCAppDownloadPram pram;
    SCAppDwonLoadReturnCode ret = SC_APP_DOWNLOAD_SUCESSED;
    SCAppPackageInfo gAppUpdateInfo = {0};
    UINT8 pGreg = 0;

    pram.url = url;
    if (strncmp(pram.url, "https://", strlen("https://")) != 0 && strncmp(pram.url, "http://", strlen("http://")) != 0)
        return;

    pram.mod = SC_APP_DOWNLOAD_HTTP_MOD;
    pram.recvtimeout = 20000;
    memset(test_payload, 0, sizeof(test_payload));
    sprintf(test_payload, "\n Start downloading firmware:%s",pram.url);
    sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
    ret = sAPI_AppDownload(&pram);
    if (ret != SC_APP_DOWNLOAD_SUCESSED)
    {
        memset(test_payload, 0, sizeof(test_payload));
        sprintf(test_payload, "\n Firmware download failed");
        sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        return;
    }
    memset(test_payload, 0, sizeof(test_payload));
    sprintf(test_payload, "\n Firmware download OK");
    sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
    ret = sAPI_AppPackageCrc(&gAppUpdateInfo);
    if (SC_APP_DOWNLOAD_SUCESSED == ret)
    {
        memset(test_payload, 0, sizeof(test_payload));
        sprintf(test_payload, "\n Firmware CRC ok, resetting...");
        sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        sAPI_TaskSleep(200);
        sAPI_SysReset();
    }
    else
    {
        memset(test_payload, 0, sizeof(test_payload));
        sprintf(test_payload, "\n Firmware CRC failed");
        sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));        
    }
}

void MqttReceive(char *subtopic)
{
        SIM_MSG_T msgQ_data_recv = {SIM_MSG_INIT, 0, -1, NULL}; // NULL pointer for msgQ_data_recv.arg3 is necessary!
        SCmqttData *sub_data = NULL;
        UINT16 pos=0;
        UINT16 topic_len=0;
        UINT16 payload_len=0;
        UINT16 topic_buff_len=0;
        UINT16 payload_buff_len=0;

        char tmp[50];
        char *topic= sAPI_Malloc(200);
        char *payload= sAPI_Malloc(200);
        char *topic_buff = sAPI_Malloc(300);
        memset(topic_buff,0,sizeof(topic_buff));
        char *payload_buff = sAPI_Malloc(300);
        memset(payload_buff,0,sizeof(payload_buff));
                                   
        memset(test_payload, 0, sizeof(test_payload));
        sprintf(test_payload, "\n Waiting for Update from Server!");
        sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload)); 
        
        sAPI_MsgQRecv(urc_mqtt_msgq_1, &msgQ_data_recv, (200));//timeout 1 secs 
                                                              
        if (!((SC_SRV_MQTT != msgQ_data_recv.msg_id) || (0 != msgQ_data_recv.arg1) || (NULL == msgQ_data_recv.arg3))) // correct msg received 
        {                                                                                                                        
            sub_data = (SCmqttData *)(msgQ_data_recv.arg3);                                                                                             
            memcpy(topic_buff,sub_data->topic_P, sub_data->topic_len);
            memcpy(payload_buff,sub_data->payload_P, sub_data->payload_len);
            topic_buff_len=sub_data->topic_len;
            payload_buff_len=sub_data->payload_len;
            sAPI_Free(sub_data->topic_P);                                                          
            sAPI_Free(sub_data->payload_P);                                                         
            sAPI_Free(sub_data);

            // Print test
            // sAPI_UartWrite(SC_UART, "topic: ", strlen("topic: "));
            // sAPI_UartWrite(SC_UART, topic_buff, topic_buff_len);
            // sAPI_UartWrite(SC_UART, "\r\n", 2);
            // sAPI_UartWrite(SC_UART, "payload: ", strlen("payload: "));
            // sAPI_UartWrite(SC_UART, payload_buff, payload_buff_len);         
            // sAPI_UartWrite(SC_UART, "\r\n", 2);
        }

        if(!strncmp(topic_buff,subtopic,strlen(subtopic))) // subcribe topic correct
        {

            memset(test_payload, 0, sizeof(test_payload));
            sprintf(test_payload, "\n Command Received from Server");
            sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
            // sAPI_UartWrite(SC_UART3, check,strlen(check));

            memset(payload,0,200);
            UINT8 post[2];
            UINT8 cnt=0;
            for(int i=0;i<strlen(payload_buff);i++)
            {
                if(payload_buff[i]==':') 
                {
                    post[cnt] = i;
                    cnt++;
                }
            }
            memset(topic, 0, sizeof(topic));
            memset(payload, 0, sizeof(payload));
            strncpy(topic,payload_buff+post[0]+1,post[1]-post[0]-1);
            strncpy(payload,payload_buff+post[1]+1,payload_buff_len-post[1]-1);
            
            sAPI_Free(topic_buff);
            sAPI_Free(payload_buff);

            sAPI_UartWrite(SC_UART3, "\n Parameter: ", strlen("\n Parameter: "));
            sAPI_UartWrite(SC_UART3, topic, strlen(topic));
            sAPI_UartWrite(SC_UART3, "\r\n", 2);
            sAPI_UartWrite(SC_UART3, " Value: ", strlen(" Value: "));
            sAPI_UartWrite(SC_UART3, payload, strlen(payload));         
            sAPI_UartWrite(SC_UART3, "\r\n", 2);



            //Update BMS parameter 
            uart_send(topic,payload);
            sAPI_Free(topic);
            sAPI_Free(payload);
            //read BMS and send MQTT update
            uart_read();
        }                                                              
}

void di_config(void)
{

    INT32 nbMsg;
    INT32 ret;
    SIM_MSG_T optionMsg = {0, 0, 0, NULL};
    char *client_id = NULL;
    char *usrName = NULL;
    char *usrPwd = NULL;
    char host[100] = {0};
    char SubHost[100]={0};
    char subtopic [100];
    uint8_t LED_sts=0;

    usrName = sAPI_Malloc(100);
    usrPwd = sAPI_Malloc(100);
    client_id = sAPI_Malloc(100);

    memset(test_payload, 0, sizeof(test_payload));
    sprintf(test_payload, "\n>>>>---- Power-On -----<<<<");
    sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
    
    sAPI_SysGetImei(imei_value);
    memset(usrName, 0, sizeof(usrName));
    snprintf(usrName, 100, "%s", "admin@aithings");
    memset(usrPwd, 0, sizeof(usrPwd));
    snprintf(usrPwd, 100, "%s", "admin");
    memset(client_id, 0, sizeof(client_id));
    snprintf(client_id, 100, "%s", imei_value);
    memset(host, 0, sizeof(host));
    // sprintf(host, "%s", "tcp://171.244.143.71:1883");
    // sprintf(host, "%s", "tcp://test.mosquitto.org:1883");
    sprintf(host, "%s", "tcp://white-dev.aithings.vn:1883");
    
    //print test
    memset(test_payload, 0, sizeof(test_payload));
    sprintf(test_payload, "\n %s", host);
    sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
    
    //Subcribe
    memset(SubHost, 0, sizeof(SubHost));
    sprintf(SubHost,"pkg/%s/control",imei_value);           
    memset(subtopic, 0, sizeof(subtopic));
    sprintf(subtopic,"pkg/%s/control",imei_value);
    
    // STATUS LIGHT
    sAPI_GpioSetDirection(9, 1);
    sAPI_GpioSetValue(9, 1);
    // LIVE LIGHT
    // sAPI_GpioSetDirection(13, 1);
    // sAPI_GpioSetValue(13, 1);
 
    sAPI_NetworkSetCtzu(1);
    sAPI_NetworkInit();  

    ret = sAPI_ReadFlash(SECTOR_1, (UINT8*)&DMS_state, sizeof(DMS_state));
    if(ret==0)
    {
        memset(test_payload, 0, sizeof(test_payload));
        sprintf(test_payload, "\n DMS state = %d",DMS_state);
        sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // if(DMS_state>1) DMS_state=0;
    }
    else
    {
        sAPI_UartWrite(SC_UART3,"\r\n Load DMS state Failed",24);
        DMS_state=0;
    }

    LED_tick = sAPI_GetTicks();
    while (1)
    {
        SCsysTime_t t;
        sAPI_GetSysLocalTime(&t);
        memset(test_payload, 0, sizeof(test_payload));
        sprintf(test_payload, "\n\n>> Local Time: %02d:%02d:%02d %d-%d-%d",t.tm_hour, t.tm_min, t.tm_sec, t.tm_mday, t.tm_mon, t.tm_year);
        sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        memset(test_payload, 0, sizeof(test_payload));
        sprintf(test_payload, "\n Imei: %s",imei_value);
        sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // Get CSQ
        GetCSQ();


        ret = sAPI_MqttStart(-1);
        // if (SC_MQTT_RESULT_SUCCESS == ret)
        // {
        //     memset(test_payload, 0, sizeof(test_payload));
        //     sprintf(test_payload,"\n MQTT Start Successful!");
        //     sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // }
        // else
        // {
        //     memset(test_payload, 0, sizeof(test_payload));
        //     sprintf(test_payload,"\n Start FAIL,ERRCODE = [%d]", ret);
        //     sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // }
        // sAPI_TaskSleep(100);        
        ret = sAPI_MqttAccq(0, NULL, 0, client_id, 0, urc_mqtt_msgq_1);
        // if (SC_MQTT_RESULT_SUCCESS == ret)
        // {
        //     memset(test_payload, 0, sizeof(test_payload));
        //     sprintf(test_payload, "\n Init SUCCESSFUL");
        //     sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // }
        // else
        // {
        //     memset(test_payload, 0, sizeof(test_payload));
        //     sprintf(test_payload, "\n Init FAIL, ERRCODE = [%d]",ret);
        //     sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // }        
        // sAPI_TaskSleep(100);
        ret = sAPI_MqttCfg(0, NULL, 0, 0, 0);
        // if(SC_MQTT_RESULT_SUCCESS == ret)
        // {
        //     memset(test_payload, 0, sizeof(test_payload));
        //     sprintf(test_payload, "\n Config SUCCESSFUL");
        //     sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // }
        // else
        // {
        //     memset(test_payload, 0, sizeof(test_payload));
        //     sprintf(test_payload,"\n Config FAIL,ERRCODE = [%d]",ret);
        //     sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // }        
        // sAPI_TaskSleep(100);
        ret = sAPI_MqttConnect(0, NULL, 0, host, 60, 0, usrName, usrPwd);
        if (SC_MQTT_RESULT_SUCCESS == ret)
        {
            
            memset(test_payload, 0, sizeof(test_payload));
            sprintf(test_payload, "\n Connect SUCCESSFUL");
            sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
             
        }
        else
        {
            // memset(test_payload, 0, sizeof(test_payload));
            // sprintf(test_payload, "\n Connect FAIL, ERRCODE = [%d]",ret);
            // sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        }
        
        //read BMS and send update to server periodically
        if(sAPI_GetTicks()>BMS_tick) 
        {
            if(sAPI_GetTicks()-BMS_tick>MQTT_SEND_TIME ) //every SEND_TIME
            {
                uart_read();
                BMS_tick=sAPI_GetTicks();
            }
        }
        else
        {
            BMS_tick=sAPI_GetTicks();
        } 

        //Subcribe MQTT
        ret = sAPI_MqttSub(0,SubHost, strlen(SubHost), 0, 0);
        if (SC_MQTT_RESULT_SUCCESS == ret)
        {
            memset(test_payload, 0, sizeof(test_payload));
            sprintf(test_payload, "\n CONNECTION OK!");
            sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));            
        }
        else
        {
            sAPI_TaskSleep(1000);
            memset(test_payload, 0, sizeof(test_payload));
            sprintf(test_payload,"\n CONNECTION FAIL,ERRCODE = [%d]", ret);
            sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
            break;
        }        
        //wait 1 secs for update from server; If receive, update BMS and send feedback to server  
        MqttReceive(subtopic);
        
        //Connection indicator LED
        if(sAPI_GetTicks() - LED_tick > LED_BLINK_TIME)
        {
            LED_sts^=1;
            sAPI_GpioSetValue(9, LED_sts);        
            LED_tick = sAPI_GetTicks();
        }    



        // memset(test_payload,0,sizeof(test_payload));
        // sprintf(test_payload, "[{\"pc\":\"Test\",\"lid\":\"1\",\"d\":%d}]",99);
        // Publish_array(test_payload);
        
        
        // sAPI_TaskSleep((5) * 200); // sleep 10 sec 
        // //MQTT Disconnect
        // ret = sAPI_MqttDisConnect(0, NULL, 0, 60);
        // //MQTT Release
        // ret = sAPI_MqttRel(0);
        // //MQTT Stop
        // ret = sAPI_MqttStop();
    }
}
