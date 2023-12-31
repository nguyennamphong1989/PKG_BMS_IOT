#include "simcom_api.h"
#include "simcom_ping.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

char loggerID[8] = "";
extern void PrintfResp(INT8 *format);
extern sMsgQRef urc_mqtt_msgq_1;
INT8 request[21] = {0x4e,0x57, 0,0x13, 0,0,0,0, 6, 3, 0, 0, 0,0,0,0, 0x68, 0,0,1,0x29};

extern sMsgQRef simcomUI_msgq;
sMsgQRef uart_mqtt_msgq_1;
// INT8 dataBMS[1024] = {0};
char test_payload[200] = {0};
UINT8 parameters_update=0;
BOOL first_start=1;

void GetCSQ()
{
    UINT8 csq;
    UINT8 ret;
    char NetResp[100]={0};
    ret = sAPI_NetworkGetCsq(&csq);
    if(ret == SC_NET_SUCCESS)
    {
        sprintf(NetResp,"\n Get csq success. csq:%d!",csq);
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
void Publish(char *input, char *subtopic)
{
    INT32 ret;
    // if (strlen(loggerID) == 0)
    //     return;
    char *topic = sAPI_Malloc(100);
    memset(topic, 0, 100);
    sprintf(topic, "d/bms_device_test_4/p/UP/1/%s", subtopic);
    sAPI_MqttTopic(0, topic, strlen(topic));
    sAPI_MqttPayload(0, input, strlen(input));
    ret=sAPI_MqttPub(0, 1, 60, 0, 0);
        // if (SC_MQTT_RESULT_SUCCESS == ret)
        // {
        //     memset(test_payload, 0, 50);
        //     sprintf(test_payload, "\n Publish SUCCESSFUL");
        //     sAPI_UartWrite(SC_UART, test_payload,21);
        // }
        // else
        // {
        //     memset(test_payload, 0, 50);
        //     sprintf(test_payload, "\n Publish FAIL, ERRCODE = [%d]",ret);
        //     sAPI_UartWrite(SC_UART, test_payload,33);
        // }      
    sAPI_Free(topic);
}
void Publish_array(char *input)
{
    char *topic = sAPI_Malloc(100);
    memset(topic, 0, 100);
    sprintf(topic, "d/bms_device_test_4/p/UP");
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
    memset(temp, 0, 50);
    sprintf(temp, "\n Queue-nbMsg = %d", nbMsg);
    sAPI_UartWrite(SC_UART3, temp,strlen(temp));

    sAPI_UartWrite(SC_UART, request, 21);
    sAPI_TaskSleep(200);
    sAPI_MsgQPoll(simcomUI_msgq, &nbMsg);
    // print test
    // memset(temp, 0, 50);
    // sprintf(temp, "\n Readall nbMsg = %d", nbMsg);
    // sAPI_UartWrite(SC_UART3, temp,strlen(temp));

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
    memset(temp, 0, 50);
    sprintf(temp, "\n BMS-RspLength = %d", packetlen);
    sAPI_UartWrite(SC_UART3, temp,strlen(temp));
    // sAPI_UartWrite(SC_UART3,"\n buffer:",9);
    // memset(temp, 0, 50);    
    // for(bufferpos=0;bufferpos<packetlen;bufferpos++)
    // {
    //     sprintf(temp,"%02X ", buffer[bufferpos]);
    //     sAPI_UartWrite(SC_UART3,temp,strlen(temp));
    // }


    bufferpos = 0;
    if (buffer[bufferpos] != 0x4e || buffer[bufferpos + 1] != 0x57) // 2 bytes header
        return;
    totallen = (buffer[bufferpos + 2] << 8) + (buffer[bufferpos + 3]) + 2; // 2 byte length
    if (totallen != packetlen)
        return;
    bufferpos += 11;

    while (bufferpos < totallen)
        switch (buffer[bufferpos])
        {
        case 0x79:
            nbBattery = buffer[bufferpos + 1] / 3;
            bufferpos += 2;
            addMsg(payload, "[{\"pc\":\"cells_vol\",\"lid\":\"1\",\"d\":[");
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
                    sprintf(temp,",%d",tmp);
                    addMsg(payload,temp);
                }
                bufferpos += 3;  
            }
            addMsg(payload, "]}");    
            break;
        case 0x80:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 100);            
            sprintf(temp, ",{\"pc\":\"tube_temp\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;
        case 0x81:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 100);    
            sprintf(temp, ",{\"pc\":\"box_temp\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;
        case 0x82:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 100);    
            sprintf(temp, ",{\"pc\":\"bat_temp\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;
        case 0x83:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 100);    
            sprintf(temp, ",{\"pc\":\"bat_vol\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;
        case 0x84:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            if(tmp>(1<<15)) tmp = tmp - (1<<15);
            memset(temp, 0, 100);    
            sprintf(temp, ",{\"pc\":\"bat_current\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;
        case 0x85:
            tmp= buffer[bufferpos + 1] ;
            memset(temp, 0, 100);    
            sprintf(temp, ",{\"pc\":\"bat_percent\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 2;
            break;
        case 0x86:
            bufferpos += 2;
            break;
        case 0x87:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 100);    
            sprintf(temp, ",{\"pc\":\"bat_cycles\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;            
            break;
        case 0x89:        
            tmp = (buffer[bufferpos + 1] << 24) + (buffer[bufferpos + 2] << 16) + (buffer[bufferpos + 3] << 8) + buffer[bufferpos + 4];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"bat_capacity\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 5;
            break;            
        case 0x8a:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 100);    
            sprintf(temp, ",{\"pc\":\"total_strings\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;     
            break;            
        case 0x8b:
            tmp = (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            if (tmp > 0)
            {
                memset(temp, 0, 300);
                addMsg(temp, ",{\"pc\":\"warning\",\"lid\":\"1\",\"d\":\"");
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
                addMsg(temp, "\"}");
                addMsg(payload,temp);
            }
            else
            {
                memset(temp, 0, 300);
                addMsg(temp, ",{\"pc\":\"warning\",\"lid\":\"1\",\"d\":\"NORMAL");
                addMsg(temp, "\"}");
                addMsg(payload,temp);
            }
            bufferpos += 3;
            break;           
        case 0x8c:
            tmp = buffer[bufferpos + 2];
            memset(temp, 0, 300);
            addMsg(temp, ",{\"pc\":\"status_info\",\"lid\":\"1\",\"d\":\"");
            if (tmp & 0x01) addMsg(temp, "Charge MOS ON,");
            else addMsg(temp, "Charge MOS OFF,");
            if (tmp & 0x02) addMsg(temp, "Discharge MOS ON,");
            else addMsg(temp, "Disharge MOS OFF,");    
            if (tmp & 0x04) addMsg(temp, "Balance ON,");
            else addMsg(temp, "Balance OFF,");    
            if (tmp & 0x08) addMsg(temp, "Batt is CONNECTED,");
            else addMsg(temp, "Batt is DISCONNECTED,");
            addMsg(temp, "\"}");
            addMsg(payload,temp);            
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
            sprintf(temp, ",{\"pc\":\"single_overvoltage\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);             
            bufferpos += 3;
            break;        
        case 0x91:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"monomer_overvoltage_recovery\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;          
        case 0x92:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"single_overvoltage_protection_delay\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0x93:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"differential_voltage_protection_value\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;        
        case 0x94:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"discharge_overcurrent_protection_value\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;                  
        case 0x95:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"discharge_overcurrent_delay\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;            
        case 0x96:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"charging_overcurrent_protection_value\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0x97:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"charging_overcurrent_delay\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0x98:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"equalizing_starting_voltage\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0x99:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"equalizing_opening_differential\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0x9a:
            bufferpos += 3;
            break;    
        case 0x9b:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"tube_temp_protection\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;        
        case 0x9c:
            bufferpos += 3;
            break;          
        case 0x9d:
            tmp= (buffer[bufferpos + 1]) ;
            memset(temp, 0, 300);                
            sprintf(temp,",{\"pc\":\"active_equalization_switch\",\"lid\":\"1\",\"d\":%d}",tmp); //0 - OFF, 1 - ON
             addMsg(payload,temp);        
            bufferpos += 2;
            break;         
        case 0x9e:
             bufferpos += 3;
            break;        
        case 0x9f:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"box_temp_protection\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0xa0:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"battery_box_recovery_temp\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;        
        case 0xa1:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"battery_temp_difference\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp); 
            bufferpos += 3;
            break;         
        case 0xa2:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"charging_high_temp_protect\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;           
        case 0xa3:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"high_temp_protect_bat_charge\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;           
        case 0xa4:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"high_temp_protect_bat_discharge\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break; 
        case 0xa5:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            if (tmp>(1<<15)) tmp = tmp-(1<<16);                        
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"charge_cryo_protect\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;        
        case 0xa6:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];   
            if (tmp>(1<<15)) tmp = tmp-(1<<16);         
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"recover_val_charge_cryoprotect\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;        
        case 0xa7:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            if (tmp>(1<<15)) tmp = tmp-(1<<16);    
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"discharge_cryo_protect\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;          
        case 0xa8:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            if (tmp>(1<<15)) tmp = tmp-(1<<16);    
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"discharge_low_temp_protect_recovery\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;
        case 0xa9:
            tmp= buffer[bufferpos + 1];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"strings_settings\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);         
            bufferpos += 2;
            break;                   
        case 0xaa:
            tmp = (buffer[bufferpos + 1] << 24) + (buffer[bufferpos + 2] << 16) + (buffer[bufferpos + 3] << 8) + buffer[bufferpos + 4];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"battery_capacity_settings\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 5;
            break;         
        case 0xab:
            tmp = buffer[bufferpos + 1];
            memset(temp, 0, 300); 
            sprintf(temp, ",{\"pc\":\"charging_mos_switch\",\"lid\":\"1\",\"d\":%d}",tmp); //0 - OFF, 1 - ON
            addMsg(payload,temp);
            bufferpos += 2;
            break; 
        case 0xac:
            tmp = buffer[bufferpos + 1];
            memset(temp, 0, 300); 
            sprintf(temp, ",{\"pc\":\"discharge_mos_switch\",\"lid\":\"1\",\"d\":%d}",tmp); //0 - OFF, 1 - ON
            addMsg(payload,temp);
            bufferpos += 2;
            break;         
        case 0xad:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"current_calibration\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;        
        case 0xae:
            bufferpos += 2;
            break;          
        case 0xaf:
            tmp = buffer[bufferpos + 1];
            memset(temp, 0, 300);
            printf(temp, ",{\"pc\":\"battery_type\",\"lid\":\"1\",\"d\":%d}",tmp); //0: lithium iron phosphate, 1:ternary, 2:lithium titanate
            addMsg(payload,temp);
            bufferpos += 2;
            break;                   
        case 0xb0:
            tmp= (buffer[bufferpos + 1] << 8) + buffer[bufferpos + 2];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"sleep_wait_time\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 3;
            break;               
        case 0xb1:
            tmp= buffer[bufferpos + 1];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"low_cap_alarm_val\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 2;
            break;         
        case 0xb2:
            memset(temp, 0, 300);    
            addMsg(temp, ",{\"pc\":\"modify_password\",\"lid\":\"1\",\"d\":\"");
            memcpy(temp + 40, buffer + bufferpos + 1, 10);
            addMsg(temp, "\"}");
            addMsg(payload,temp);            
            bufferpos += 11;
            break;
        case 0xb3:
            tmp = buffer[bufferpos + 1];
            memset(temp, 0, 300); 
            sprintf(temp, ",{\"pc\":\"special_charger_switch\",\"lid\":\"1\",\"d\":%d}",tmp); //0 - OFF, 1 - ON
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
            sprintf(temp, ",{\"pc\":\"uptime\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 5;
            break;
        case 0xb7:
            memset(temp, 0, 300);    
            addMsg(temp, ",{\"pc\":\"version_num\",\"lid\":\"1\",\"d\":\"");
            memcpy(temp + 36, buffer + bufferpos + 1, 15);
            addMsg(temp, "\"}");
            addMsg(payload,temp);
            bufferpos += 16;
            break;
        case 0xb8:
            tmp = buffer[bufferpos + 1];
            memset(temp, 0, 300); 
            sprintf(temp, ",{\"pc\":\"start_current_calibration\",\"lid\":\"1\",\"d\":%d}",tmp); //0 - OFF, 1 - ON
            addMsg(payload,temp);
            bufferpos += 2;
            break;          
        case 0xb9:
            tmp = (buffer[bufferpos + 1] << 24) + (buffer[bufferpos + 2] << 16) + (buffer[bufferpos + 3] << 8) + buffer[bufferpos + 4];
            memset(temp, 0, 300);    
            sprintf(temp, ",{\"pc\":\"actual_capacity\",\"lid\":\"1\",\"d\":%d}",tmp);
            addMsg(payload,temp);
            bufferpos += 5;
            break;
        case 0xba:
            memset(temp, 0, 300);    
            addMsg(temp, ",{\"pc\":\"factory_id\",\"lid\":\"1\",\"d\":\"");
            memcpy(temp + 35, buffer + bufferpos + 1, 24);
            addMsg(temp, "\"}");
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
            sprintf(temp, ",{\"pc\":\"protocol_version\",\"lid\":\"1\",\"d\":%d}",tmp); 
            addMsg(payload,temp);
            bufferpos += 2;            
            break;
        default:
            bufferpos++;
            break;
        }
    SCsysTime_t t;
    sAPI_GetSysLocalTime(&t);
            
    memset(temp, 0, 300);    
    sprintf(temp, ",{\"pc\":\"logger_status\",\"lid\":\"1\",\"d\":\"%d %d:%d:%d %d-%d-%d\"}]",sAPI_ReadVbat(), t.tm_hour, t.tm_min, t.tm_sec, t.tm_mday, t.tm_mon, t.tm_year);
    addMsg(payload,temp);
    Publish_array(payload);
            
    //print test        
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

    if(strncmp(topic,"total_undervoltage",strlen(topic))==0)
    {
        code = 0x8f;
        size = 3;
        frame_data=sAPI_Malloc(size);
        frame=sAPI_Malloc(20 + size);
        memset(frame,0,20 + size);
        memset(frame_data,0,size);

        value= atoi(payload);
        UINT8 lowByte = (UINT8) (value & 0xff);
        UINT8 highByte = (UINT8) ((value >> 8) & 0xff);

        frame_data[0] = code;
        frame_data[1] = highByte;
        frame_data[2] = lowByte;

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

        // // Print test
        // sAPI_UartWrite(SC_UART,"\r\n",2);
        // for(INT8 i=0; i<20+size; i++)
        // {
        //     sprintf(tmp,"%02X ", frame[i]);
        //     sAPI_UartWrite(SC_UART,tmp,strlen(tmp));
        // }
        // Send to BMS
        sAPI_UartWrite(SC_UART,frame,(20 + size));

    }
    if(strncmp(topic,"total_overvoltage",strlen(topic))==0)
    {

    }
    if(strncmp(topic,"single_overvoltage",strlen(topic))==0)
    {
        code = 0x90;
        size = 3;
        frame_data=sAPI_Malloc(size);
        frame=sAPI_Malloc(20 + size);
        memset(frame,0,20 + size);
        memset(frame_data,0,size);

        value= atoi(payload);
        UINT8 lowByte = (UINT8) (value & 0xff);
        UINT8 highByte = (UINT8) ((value >> 8) & 0xff);

        frame_data[0] = code;
        frame_data[1] = highByte;
        frame_data[2] = lowByte;

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

        // // Print test
        // sAPI_UartWrite(SC_UART,"\r\n",2);
        // for(INT8 i=0; i<20+size; i++)
        // {
        //     sprintf(tmp,"%02X ", frame[i]);
        //     sAPI_UartWrite(SC_UART,tmp,strlen(tmp));
        // }
        // Send to BMS
        sAPI_UartWrite(SC_UART,frame,(20 + size));        
    }
    if(strncmp(topic,"monomer_overvoltage_recovery",strlen(topic))==0)
    {
        code = 0x91;
    }
    if(strncmp(topic,"single_overvoltage_protection_delay",strlen(topic))==0)
    {
        code = 0x92;
    }            
    if(strncmp(topic,"active_equalization_switch",strlen(topic))==0)
    {
        code = 0x9A;
        frame_header[3] = 0x14;
        size = 2;
        frame_data=sAPI_Malloc(size);
        frame=sAPI_Malloc(20 + size);
        memset(frame,0,20 + size);
        memset(frame_data,0,size);
        UINT8 lowByte;
        UINT8 highByte;
        
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
        // sAPI_UartWrite(SC_UART,"\r\n",2);
        // for(INT8 i=0; i<20+size; i++)
        // {
        //     sprintf(tmp,"%02X ", frame[i]);
        //     sAPI_UartWrite(SC_UART,tmp,strlen(tmp));
        // }
        // Send to BMS
        sAPI_UartWrite(SC_UART,frame,(20 + size));         
    }    
    if(strncmp(topic,"charging_mos_switch",strlen(topic))==0)
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
        
        memset(mqtt_rep,0,sizeof(mqtt_rep));
        sprintf(mqtt_rep, "[{\"pc\":\"charging_mos_switch\",\"lid\":\"1\",\"d\":%d}]",value);
        Publish_array(mqtt_rep);

    }
    if(strncmp(topic,"discharge_mos_switch",strlen(topic))==0)
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

        memset(mqtt_rep,0,sizeof(mqtt_rep));
        sprintf(mqtt_rep, "[{\"pc\":\"discharge_mos_switch\",\"lid\":\"1\",\"d\":%d}]",value);
        Publish_array(mqtt_rep);

    }
    sAPI_Free(tmp);
    sAPI_Free(frame);
    sAPI_Free(mqtt_rep);
    sAPI_TaskSleep(200); // wait BMS finish sending response
}
void test( INT32 ret, SIM_MSG_T optionMsg, char *client_id, char *usrName,  char *usrPwd, char *host, char *SubHost)
{
        //Query signal quality
        GetCSQ();
        //get mode selection(CNMP)
        GetCnmp();
        // Get Creg + Cgreg 
        GetCreg();
        GetCGreg();
        // Get UE info
        GetCpsi();
        // MQTT Start
        ret = sAPI_MqttStart(-1);
        if (SC_MQTT_RESULT_SUCCESS == ret)
        {
            memset(test_payload, 0, 50);
            sprintf(test_payload,"\n MQTT Start Successful!");
            sAPI_UartWrite(SC_UART, test_payload,30);
        }
        else
        {
            memset(test_payload, 0, 50);
            sprintf(test_payload,"\n Start FAIL,ERRCODE = [%d]", ret);
            sAPI_UartWrite(SC_UART, test_payload,34);
        }
        sAPI_TaskSleep(100);
        //MQTT Init
        ret = sAPI_MqttAccq(0, NULL, 0, client_id, 0, urc_mqtt_msgq_1);
        if (SC_MQTT_RESULT_SUCCESS == ret)
        {
            memset(test_payload, 0, 50);
            sprintf(test_payload, "\n Init SUCCESSFUL");
            sAPI_UartWrite(SC_UART, test_payload,21);
        }
        else
        {
            memset(test_payload, 0, 50);
            sprintf(test_payload, "\n Init FAIL, ERRCODE = [%d]",ret);
            sAPI_UartWrite(SC_UART, test_payload,33);
        }
        sAPI_TaskSleep(100);
        //PdP context config
        ret = sAPI_MqttCfg(0, NULL, 0, 0, 0);
        if(SC_MQTT_RESULT_SUCCESS == ret)
        {
            PrintfResp("\n MQTT config Successful!");
        }
        else
        {
            memset(test_payload, 0, 50);
            sprintf(test_payload,"\n config FAIL,ERRCODE = [%d]",ret);
            sAPI_UartWrite(SC_UART, test_payload,34);
        }
        //MQTT Connect
        ret = sAPI_MqttConnect(0, NULL, 0, host, 60, 0, usrName, usrPwd);
        if (SC_MQTT_RESULT_SUCCESS == ret)
        {
            memset(test_payload, 0, 50);
            sprintf(test_payload, "\n Connect SUCCESSFUL");
            sAPI_UartWrite(SC_UART, test_payload,21);
        }
        else
        {
            memset(test_payload, 0, 50);
            sprintf(test_payload, "\n Connect FAIL, ERRCODE = [%d]",ret);
            sAPI_UartWrite(SC_UART, test_payload,33);
        }
        // //MQTT Publish
        // sAPI_MqttTopic(0, "d/bms_device_test_4/p/UP/1/cell_1_vol", 37);
        // sAPI_MqttPayload(0, "{\"d\":999}", 9);
        // ret=sAPI_MqttPub(0, 1, 60, 0, 0);
        if (SC_MQTT_RESULT_SUCCESS == ret)
        {
            memset(test_payload, 0, 50);
            sprintf(test_payload, "\n Publish SUCCESSFUL");
            sAPI_UartWrite(SC_UART, test_payload,21);
        }
        else
        {
            memset(test_payload, 0, 50);
            sprintf(test_payload, "\n Publish FAIL, ERRCODE = [%d]",ret);
            sAPI_UartWrite(SC_UART, test_payload,33);
        }
        //MQTT Subcribe
        ret = sAPI_MqttSub(0,SubHost, strlen(SubHost), 0, 0);
        if (SC_MQTT_RESULT_SUCCESS == ret)
        {
            // sAPI_Debug("subscribe SUCCESS");
            PrintfResp("\r\nMQTT subscribe Successful 99!\r\n");
        }
        else
        {
            memset(test_payload, 0, 50);
            sprintf(test_payload,"\nsubscribe FAIL,ERRCODE = [%d]", ret);
            sAPI_UartWrite(SC_UART, test_payload,30);
        }

        memset(test_payload,0,sizeof(test_payload));
        sprintf(test_payload, "[{\"pc\":\"bat_capacity\",\"lid\":\"1\",\"d\":88888}]");
        Publish_array(test_payload);

        sAPI_TaskSleep((10) * 200); // sleep 10 sec before disconn
        //MQTT Disconnect
        ret = sAPI_MqttDisConnect(0, NULL, 0, 60);
        //MQTT Release
        ret = sAPI_MqttRel(0);
        //MQTT Stop
        ret = sAPI_MqttStop();
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
        memset(topic_buff,0,200);
        char *payload_buff = sAPI_Malloc(300);
        memset(payload_buff,0,200);
        /*=======================================================================================================================
         *
         *   NOTE: if this data reception cycle too long may cause data loss(data processing slower than data receive from server)
         *
         **=======================================================================================================================*/
        // recv the subscribed topic data, from the message queue: urc_mqtt_msgq_1, it is set buy sAPI_MqttAccq                                        
        memset(test_payload, 0, 50);
        sprintf(test_payload, "\n Waiting for Update from Server!");
        sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload)); 
        sAPI_MsgQRecv(urc_mqtt_msgq_1, &msgQ_data_recv, (10*200));//timeout 10 secs 
                                                              
        if (!((SC_SRV_MQTT != msgQ_data_recv.msg_id) || (0 != msgQ_data_recv.arg1) || (NULL == msgQ_data_recv.arg3))) // correct msg received 
        {                                                                                                                        
            sub_data = (SCmqttData *)(msgQ_data_recv.arg3);                                                                                             
            // sAPI_UartWrite(SC_UART, (UINT8 *)"recieve topic: ", strlen("recieve topic: "));
            // sAPI_UartWrite(SC_UART, (UINT8 *)sub_data->topic_P, sub_data->topic_len);
            // sAPI_UartWrite(SC_UART, (UINT8 *)"\r\n", 2);
            // sAPI_UartWrite(SC_UART, (UINT8 *)"recieve payload: ", strlen("recieve payload: "));
            // sAPI_UartWrite(SC_UART, (UINT8 *)sub_data->payload_P, sub_data->payload_len);         
            // sAPI_UartWrite(SC_UART, (UINT8 *)"\r\n", 2);                                            
             /*these msg pointer must be free after using, don not change the free order*/   
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

            memset(test_payload, 0, 50);
            sprintf(test_payload, "\n Command Received from Server!");
            sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
            pos=0;
            memset(payload,0,200);
            while(pos < payload_buff_len)
            {
                if((payload_buff[pos]=='\"')&&(payload_buff[pos+1]=='d')&&(payload_buff[pos+2]=='\"'))
                {
                    payload_len=0;
                    while(payload_buff[pos + 4 + payload_len]!=',')
                    {
                        payload[payload_len] = payload_buff [pos + 4 + payload_len];
                        payload_len+=1;
                    }
                    //print test
                    // memset(tmp,0,20);
                    // sprintf(tmp,"\r\npayload: %s",payload);
                    // sAPI_UartWrite(SC_UART3, tmp, strlen(tmp));
                    // memset(tmp,0,20);
                    // sprintf(tmp,"\r\nlen: %d",payload_len);
                    // sAPI_UartWrite(SC_UART3, tmp, strlen(tmp));

                     break;
                }
                pos+=1;
            }
            
            pos = topic_buff_len-1;
            memset(topic,0,200);
            while (pos>0)
            {
                if(topic_buff[pos]=='/')
                {
                    topic_len=topic_buff_len-1-pos;
                    memcpy(topic,topic_buff+pos+1,topic_len);

                    //print test
                    // memset(tmp,0,50);
                    // sprintf(tmp,"\r\ntopic: %s",topic);
                    // sAPI_UartWrite(SC_UART3, tmp, strlen(tmp));
                    // memset(tmp,0,10);
                    // sprintf(tmp,"\r\nlen: %d",topic_len);
                    // sAPI_UartWrite(SC_UART3, tmp, strlen(tmp));

                    break;    
                }
                pos-=1;
            }
            sAPI_Free(topic_buff);
            sAPI_Free(payload_buff);
            //Update BMS parameter and send MQTT update
            uart_send(topic,payload);
            sAPI_Free(topic);
            sAPI_Free(payload);
        }                                                              
}
void di_config(void)
{
    INT32 BMS_tick;
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

    memset(usrName, 0, sizeof(usrName));
    snprintf(usrName, 100, "%s", "bms_device_test_4");
    memset(usrPwd, 0, sizeof(usrPwd));
    snprintf(usrPwd, 100, "%s", "mkp27102022");
    memset(client_id, 0, sizeof(client_id));
    snprintf(client_id, 100, "%s", "bms_device_test_4");
    memset(host, 0, sizeof(host));
    // sprintf(host, "%s", "tcp://171.244.143.71:1883");
    // sprintf(host, "%s", "tcp://test.mosquitto.org:1883");
    sprintf(host, "%s", "tcp://smarthome.test.makipos.net:1883");
    
    //Subcribe
    memset(SubHost, 0, sizeof(SubHost));
    sprintf(SubHost, "%s", "d/bms_device_test_4/s/#");           
    memset(subtopic, 0, sizeof(subtopic));
    sprintf(subtopic, "%s", "d/bms_device_test_4/s");
    
    
    
    // STATUS LIGHT
    sAPI_GpioSetDirection(9, 1);
    sAPI_GpioSetValue(9, 1);
    // LIVE LIGHT
    // sAPI_GpioSetDirection(13, 1);
    // sAPI_GpioSetValue(13, 1);
    sAPI_TaskSleep(600); 


    memset(test_payload, 0, 50);
    sprintf(test_payload, "\n>>>> bms_device_test_4 Power-On");
    sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
 
    sAPI_NetworkSetCtzu(1);
    sAPI_NetworkInit();  

    
    BMS_tick= sAPI_GetTicks();


    while (1)
    {
        SCsysTime_t t;
        sAPI_GetSysLocalTime(&t);
        memset(test_payload, 0, 50);
        sprintf(test_payload, "\n>>>> bms_device_test_4 %02d:%02d:%02d %d-%d-%d",t.tm_hour, t.tm_min, t.tm_sec, t.tm_mday, t.tm_mon, t.tm_year);
        sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // Get CSQ
        GetCSQ();
 
        ret = sAPI_MqttStart(-1);
        // if (SC_MQTT_RESULT_SUCCESS == ret)
        // {
        //     memset(test_payload, 0, 50);
        //     sprintf(test_payload,"\n MQTT Start Successful!");
        //     sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // }
        // else
        // {
        //     memset(test_payload, 0, 50);
        //     sprintf(test_payload,"\n Start FAIL,ERRCODE = [%d]", ret);
        //     sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // }
        // sAPI_TaskSleep(100);        
        ret = sAPI_MqttAccq(0, NULL, 0, client_id, 0, urc_mqtt_msgq_1);
        // if (SC_MQTT_RESULT_SUCCESS == ret)
        // {
        //     memset(test_payload, 0, 50);
        //     sprintf(test_payload, "\n Init SUCCESSFUL");
        //     sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // }
        // else
        // {
        //     memset(test_payload, 0, 50);
        //     sprintf(test_payload, "\n Init FAIL, ERRCODE = [%d]",ret);
        //     sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // }        
        // sAPI_TaskSleep(100);
        ret = sAPI_MqttCfg(0, NULL, 0, 0, 0);
        // if(SC_MQTT_RESULT_SUCCESS == ret)
        // {
        //     memset(test_payload, 0, 50);
        //     sprintf(test_payload, "\n Config SUCCESSFUL");
        //     sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // }
        // else
        // {
        //     memset(test_payload, 0, 50);
        //     sprintf(test_payload,"\n Config FAIL,ERRCODE = [%d]",ret);
        //     sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        // }        
        // sAPI_TaskSleep(100);
        ret = sAPI_MqttConnect(0, NULL, 0, host, 60, 0, usrName, usrPwd);
        if (SC_MQTT_RESULT_SUCCESS == ret)
        {
            memset(test_payload, 0, 50);
            sprintf(test_payload, "\n Connect SUCCESSFUL");
            sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
            uart_read();
            sAPI_GpioSetValue(9, 0);
            sAPI_TaskSleep(10);
            sAPI_GpioSetValue(9, 1);        
        }
        else
        {
            // memset(test_payload, 0, 50);
            // sprintf(test_payload, "\n Connect FAIL, ERRCODE = [%d]",ret);
            // sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
        }
        ret = sAPI_MqttSub(0,SubHost, strlen(SubHost), 0, 0);
        if (SC_MQTT_RESULT_SUCCESS == ret)
        {
            memset(test_payload, 0, 50);
            sprintf(test_payload, "\n CONNECTION OK!");
            sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));            
        }
        else
        {
            sAPI_TaskSleep(1000);
            memset(test_payload, 0, 50);
            sprintf(test_payload,"\n CONNECTION FAIL,ERRCODE = [%d]", ret);
            sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));
            break;
        }        
        MqttReceive(subtopic);//wait 10 secs for update from server 
        LED_sts^=1;
        sAPI_GpioSetValue(9, LED_sts);        

        if(sAPI_GetTicks()>BMS_tick) //read BMS and send update to server every 1 min
        {
            if(sAPI_GetTicks()-BMS_tick>(60*200)) //1 mins
            {
                uart_read();
                BMS_tick = sAPI_GetTicks();
                memset(test_payload, 0, 50);
                sprintf(test_payload, "\n Finish Reading BMS!");
                sAPI_UartWrite(SC_UART3, test_payload,strlen(test_payload));                  
            }
        }
        else
        {
            BMS_tick=sAPI_GetTicks();
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
