#include "simcom_api.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "simcom_call.h"

#define FALSE 0
#define TRUE 1
#define CALL_URC_RECIVE_TIME_OUT 20000

sMsgQRef g_call_demo_msgQ;
extern sMsgQRef simcomUI_msgq;


typedef enum{
    SC_CALL_DEMO_DIAL            = 1,
    SC_CALL_DEMO_ANSWER          = 2,
    SC_CALL_DEMO_END             = 3,
    SC_CALL_DEMO_STATE           = 4,
    SC_CALL_DEMO_AUTO_ANSWER     = 5,
    SC_CALL_DEMO_MAX             = 99
}SC_CALL_DEMO_TYPE;

SC_CALLReturnCode callDialdemo()
{
  SC_CALLReturnCode ret = SC_CALL_SUCESS;
  INT8 rsp_buff[100];
  SIM_MSG_T optionMsg ={0,0,0,NULL};
  SIM_MSG_T msg;
  __attribute__((__unused__)) SC_STATUS status;
  UINT8 dialstring[SC_CALL_MAX_NUMBER_LENGTH];

  INT8 *note = "\r\nPlease input dial number: \r\n";
  PrintfResp(note);
  sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
  if(SRV_UART != optionMsg.msg_id)
  {
    sAPI_Debug("msg_id is error!!\r\n");
  }
  sAPI_Debug("--callDialdemo--, arg3 = [%s]",optionMsg.arg3);
  sAPI_UartWriteString(SC_UART, optionMsg.arg3);
  memset(dialstring, 0, SC_CALL_MAX_NUMBER_LENGTH);
  memcpy(dialstring, optionMsg.arg3, strlen(optionMsg.arg3));

  ret = sAPI_CallDialMsg(dialstring, g_call_demo_msgQ);
  if(ret == SC_CALL_SUCESS)
  {
    memset(&msg,0,sizeof(msg));
    status = sAPI_MsgQRecv(g_call_demo_msgQ,&msg,CALL_URC_RECIVE_TIME_OUT);
    sprintf(rsp_buff,"\r\nsAPI_CallDialMsg:\r\n\tresultCode[%d]\r\n",msg.arg2);
    sAPI_UartWriteString(SC_UART, (UINT8*)rsp_buff);
  }

  return ret;
}

SC_CALLReturnCode callAnswerdemo()
{
  SC_CALLReturnCode ret = SC_CALL_SUCESS;
  INT8 rsp_buff[100];
  SIM_MSG_T msg;
  __attribute__((__unused__)) SC_STATUS status;

  ret = sAPI_CallAnswerMsg(g_call_demo_msgQ);
  if(ret == SC_CALL_SUCESS)
  {
    memset(&msg,0,sizeof(msg));
    status = sAPI_MsgQRecv(g_call_demo_msgQ,&msg,CALL_URC_RECIVE_TIME_OUT);
    sprintf(rsp_buff,"\r\nsAPI_CallAnswerMsg:\r\n\tresultCode[%d]\r\n",msg.arg2);
    sAPI_UartWriteString(SC_UART, (UINT8*)rsp_buff);
  }

  return ret;
}

SC_CALLReturnCode callEnddemo()
{
  SC_CALLReturnCode ret = SC_CALL_SUCESS;
  INT8 rsp_buff[100];
  SIM_MSG_T msg;
  __attribute__((__unused__)) SC_STATUS status;

  ret = sAPI_CallEndMsg(g_call_demo_msgQ);
  if(ret == SC_CALL_SUCESS)
  {
    memset(&msg,0,sizeof(msg));
    status = sAPI_MsgQRecv(g_call_demo_msgQ,&msg,CALL_URC_RECIVE_TIME_OUT);
    sprintf(rsp_buff,"\r\nsAPI_CallEndMsg:\r\n\tresultCode[%d]\r\n",msg.arg2);
    sAPI_UartWriteString(SC_UART, (UINT8*)rsp_buff);
  }

  return ret;
}

SC_CALLReturnCode callStatedemo()
{
  SC_CALLReturnCode ret = SC_CALL_SUCESS;
  INT8 rsp_buff[100];
  UINT8 callstate = 9;

  callstate = sAPI_CallStateMsg();

  sprintf(rsp_buff,"\r\nsAPI_CallStateMsg:\r\n\tcallstate[%d]\r\n",callstate);
  sAPI_UartWriteString(SC_UART, (UINT8*)rsp_buff);

  return ret;
}

SC_CALLReturnCode callAutoAnswerdemo()
{
  SC_CALLReturnCode ret = SC_CALL_SUCESS;
  INT8 rsp_buff[100] = { 0 };
  SIM_MSG_T optionMsg ={0,0,0,NULL};
  SIM_MSG_T msg;
  INT32 seconds = 0;

  INT8 *note = "\r\nPlease input delay seconds:(0-255) \r\n";
  PrintfResp(note);
  sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
  if(SRV_UART != optionMsg.msg_id)
  {
      sAPI_Debug("msg_id is error!!\r\n");
  }
  sAPI_UartWriteString(SC_UART, optionMsg.arg3);
  seconds = atoi(optionMsg.arg3);
  sAPI_Debug("seconds [%d]",seconds);

  ret = sAPI_CallAutoAnswer(seconds, g_call_demo_msgQ);
  if(ret == SC_CALL_SUCESS)
  {
    memset(&msg,0,sizeof(msg));
    sAPI_MsgQRecv(g_call_demo_msgQ,&msg,CALL_URC_RECIVE_TIME_OUT);
    sprintf(rsp_buff,"\r\nsAPI_CallAutoAnswer:\r\n\tresultCode[%d]\r\n",msg.arg2);
    sAPI_UartWriteString(SC_UART, (UINT8*)rsp_buff);
  }

  return ret;
}

void CALLDemo(void)
{
    SC_CALLReturnCode ret;
    int resp_buff_len = 120;
    __attribute__((__unused__)) SC_STATUS status;
    SIM_MSG_T msg;
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    INT8 rsp_buff[resp_buff_len];
    UINT32 opt = 0;

    INT8 *note = "\r\nPlease select an option to test from the items listed below.\r\n";
    INT8 *options_list[] = {
        "1. Dial",
        "2. Answer",
        "3. End",
        "4. State",
        "5. Auto Answer",
        "99. Back"
    };

    status = sAPI_MsgQCreate(&g_call_demo_msgQ, "g_call_demo_msgQ", sizeof(SIM_MSG_T), 4, SC_FIFO);
    if(status != SC_SUCCESS)
    {
      sAPI_Debug("ERROR: message queue creat err!\n");
    }

    while(1)
    {
        memset(rsp_buff, 0, resp_buff_len);
        PrintfResp(note);
        PrintfOptionMenu(options_list,sizeof(options_list)/sizeof(options_list[0]));
        sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
        if(SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!",__func__);
            break;
        }

        sAPI_Debug("--CALLDemo--, arg3 = [%s]",optionMsg.arg3);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);

        switch(opt)
        {
            case SC_CALL_DEMO_DIAL:
            {
                ret = callDialdemo();
                break;
            }
            case SC_CALL_DEMO_ANSWER:
            {
                ret = callAnswerdemo();
                break;
            }
            case SC_CALL_DEMO_END:
            {
                ret = callEnddemo();
                break;
            }
            case SC_CALL_DEMO_STATE:
            {
                ret = callStatedemo();
                break;
            }
            case SC_CALL_DEMO_AUTO_ANSWER:
            {
                ret = callAutoAnswerdemo();
                break;
            }
            case SC_CALL_DEMO_MAX:
            {
                sAPI_Debug("[CALL] Return to the previous menu!");
                PrintfResp("\r\nReturn to the previous menu!\r\n");
                sAPI_MsgQDelete(g_call_demo_msgQ);
                return;
            }
            default :
                break;
        }
    }
}
