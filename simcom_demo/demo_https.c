#include "simcom_api.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

typedef enum{
    SC_HTTPS_DEMO_INIT         = 1,
    SC_HTTPS_DEMO_PARA         = 2,
    SC_HTTPS_DEMO_DATA         = 3,
    SC_HTTPS_DEMO_ACTION       = 4,
    SC_HTTPS_DEMO_HEAD         = 5,
    SC_HTTPS_DEMO_READ         = 6,
    SC_HTTPS_DEMO_POSTFILE     = 7,
    SC_HTTPS_DEMO_TERM         = 8,

    SC_HTTPS_DEMO_MAX          = 99
}SC_HTTPS_DEMO_TYPE;

typedef enum
{
  PARAM_NUM,
  PARAM_STR
}PARAM_TYPE;

typedef struct
{
  const char *prompt;
  const char *name;
  PARAM_TYPE type;
  void *ppara;
  UINT32 size;
}param_element;

extern sMsgQRef simcomUI_msgq;
extern void PrintfOptionMenu(INT8* options_list[], int array_size);
extern void PrintfResp(INT8* format);
sMsgQRef httpsUIResp_msgq=NULL;

static int handle_params(const param_element *elements, int num)
{
  int i;
  int result=0;
  SIM_MSG_T optionMsg ={0,0,0,NULL};
  for (i=0; i<num; i++)
  {
    PrintfResp((INT8*)elements[i].prompt);

    sAPI_MsgQRecv(simcomUI_msgq,&optionMsg,SC_SUSPEND);
    if(SRV_UART != optionMsg.msg_id)
    {
        sAPI_Debug("%s,msg_id is error!!",__func__);
        return -1;
    }

    PrintfResp("\r\n");
    PrintfResp((INT8*)elements[i].name);
    PrintfResp("=");
    PrintfResp((INT8*)optionMsg.arg3);
    PrintfResp("\r\n");

    sAPI_Debug("arg3 = [%s]",optionMsg.arg3);
    switch (elements[i].type)
    {
      case PARAM_NUM:

        *((INT32 *)elements[i].ppara) = atoi(optionMsg.arg3);

        break;

      case PARAM_STR:

        strncpy(elements[i].ppara, (char *)optionMsg.arg3, elements[i].size);

        break;

      default:

        result = -1;
        break;
    }

    if (result!=0)
    {
      break;
    }
  }

  sAPI_Free(optionMsg.arg3);

  return result;
}
#define MAX_PARA_STR_SIZE 256
static struct
{
  char type[MAX_PARA_STR_SIZE];
  char value[MAX_PARA_STR_SIZE];
}http_para;

static struct
{
  char data[MAX_PARA_STR_SIZE];
  INT32 len;
}http_data;

static struct
{
  INT32 method;
}http_action;

static struct
{
  INT32 type;
  INT32 offset;
  INT32 size;
}http_read;

static struct
{
  char filename[MAX_PARA_STR_SIZE];
  INT32 dir;
}http_postfile;

static const param_element http_para_params[]=
{
  {.prompt="\r\ninput type:(URL,CONNECTTO,CONTENT,USERDATA,ACCEPT,SSLCFG,RECVTO,READMODE)\r\n", .name="type", .type=PARAM_STR, .ppara=&http_para.type, sizeof(http_para.type)},
  {.prompt="\r\ninput value:\r\n", .name="value", .type=PARAM_STR, .ppara=&http_para.value, sizeof(http_para.value)},
};

static const param_element http_data_params[]=
{
  {.prompt="\r\ninput data:\r\n", .name="data", .type=PARAM_STR, .ppara=&http_data.data, sizeof(http_data.data)},
};

static const param_element http_action_params[]=
{
  {.prompt="\r\ninput method:\r\n", .name="method", .type=PARAM_NUM, .ppara=&http_action.method, sizeof(http_action.method)},
};

static const param_element http_read_params[]=
{
  {.prompt="\r\ninput type:(0: command get 1: command set)\r\n", .name="type", .type=PARAM_NUM, .ppara=&http_read.type, sizeof(http_read.type)},
  {.prompt="\r\ninput offset:\r\n", .name="offset", .type=PARAM_NUM, .ppara=&http_read.offset, sizeof(http_read.offset)},
  {.prompt="\r\ninput size:\r\n", .name="size", .type=PARAM_NUM, .ppara=&http_read.size, sizeof(http_read.size)},
};

static const param_element http_postfile_params[]=
{
  {.prompt="\r\ninput filename:\r\n", .name="filename", .type=PARAM_STR, .ppara=&http_postfile.filename, sizeof(http_postfile.filename)},
  {.prompt="\r\ninput dir:\r\n", .name="dir", .type=PARAM_NUM, .ppara=&http_postfile.dir, sizeof(http_postfile.dir)},
};

static void show_httphead_urc(void)
{
    SIM_MSG_T msgQ_data_recv = {SIM_MSG_INIT, 0, -1, NULL};
    SChttpApiTrans *sub_data = NULL;

    sAPI_MsgQRecv(httpsUIResp_msgq, &msgQ_data_recv, SC_SUSPEND);

    sub_data = (SChttpApiTrans *)(msgQ_data_recv.arg3);

    if (sub_data->data != NULL && sub_data->dataLen > 0)
    {
        sAPI_UartWrite(SC_UART, (UINT8 *)sub_data->data, sub_data->dataLen);
    }

    sAPI_Free(sub_data->data);
    sAPI_Free(sub_data);
}

static void show_httpread_urc(int type)
{
    #define HTTPS_READ_FAIL ("http read fail")
    SIM_MSG_T msgQ_data_recv = {SIM_MSG_INIT, 0, -1, NULL};
    SChttpApiTrans *sub_data = NULL;
    char rspBuf[50];
    int size = 0;

    sAPI_MsgQRecv(httpsUIResp_msgq, &msgQ_data_recv, SC_SUSPEND);
    sub_data = (SChttpApiTrans *)(msgQ_data_recv.arg3);

    if (type == 0)
    {
        size = snprintf(rspBuf, sizeof(rspBuf), "\r\ncontent-length=%d\r\n", sub_data->action_content_len);
        sAPI_UartWrite(SC_UART, (UINT8*)rspBuf, size);
    }
    else
    {
        sAPI_UartWrite(SC_UART, (UINT8*)sub_data->data, sub_data->dataLen);
        sAPI_Free(sub_data->data);
    }

    sAPI_Free(sub_data);
}

static void show_httpaction_urc(void)
{
    sAPI_Debug("msgQ = %p", httpsUIResp_msgq);
    SIM_MSG_T msgQ_data_recv = {SIM_MSG_INIT, 0, -1, NULL};
    sAPI_MsgQRecv(httpsUIResp_msgq, &msgQ_data_recv, SC_SUSPEND);

    SChttpApiTrans *sub_data = (SChttpApiTrans *)(msgQ_data_recv.arg3);

    char str[256];
    sprintf(str, "\r\nstatus-code=%d content-length=%d\r\n", sub_data->status_code, sub_data->action_content_len);
    sAPI_UartWrite(SC_UART, (UINT8 *)str, strlen(str));

    sAPI_Free(sub_data->data);
    sAPI_Free(sub_data);
}

void HttpsDemo(void)
{
    SC_STATUS status;
    SC_HTTP_RETURNCODE ret = SC_HTTPS_SUCCESS;

    SIM_MSG_T optionMsg ={0,0,0,NULL};
    UINT32 opt = 0;
    INT8 *resp = NULL;
    INT8 *note = "\r\nPlease select an option to test from the items listed below.\r\n";
    INT8 *options_list[] = {
        "1. Http session initialize",
        "2. Set parameters",
        "3. Set post data",
        "4. Send http request",
        "5. Read http response header",
        "6. Read http response content",
        "7. Send file for http request",
        "8. Http session terminate",
        "99. Back to main menu",
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

        switch(opt)
        {
            case SC_HTTPS_DEMO_INIT:
                {
                    if(NULL == httpsUIResp_msgq)
                    {
                        //SC_STATUS status;
                        status = sAPI_MsgQCreate(&httpsUIResp_msgq, "httpsUIResp_msgq", sizeof(SIM_MSG_T), 8, SC_FIFO);
                        if(SC_SUCCESS != status)
                        {
                            sAPI_Debug("msgQ create fail");
                            resp = "\r\nHTTP Init Fail!\r\n";
                            sAPI_UartWrite(SC_UART,(UINT8*)resp,strlen(resp));
                            break;
                        }
                    }

                    ret = sAPI_HttpInit(1,httpsUIResp_msgq);
                    if(ret == SC_HTTPS_SUCCESS)
                    {
                        PrintfResp("\r\nhttps init success\r\n");
                    }
                    else
                    {
                        PrintfResp("\r\nhttps init fail\r\n");
                    }

                    break;
                }
            case SC_HTTPS_DEMO_PARA:
                {
                    if (handle_params(http_para_params, sizeof(http_para_params)/sizeof(http_para_params[0])) == 0)
                    {
                        sAPI_Debug("http_para.type=%s", http_para.type);
                        sAPI_Debug("http_para.value=%s", http_para.value);
                        ret = sAPI_HttpPara(http_para.type, http_para.value);
                        if(ret == SC_HTTPS_SUCCESS)
                        {
                            PrintfResp("\r\nhttps set parameters success\r\n");
                        }
                        else
                        {
                            PrintfResp("\r\nhttps set parameters fail\r\n");
                        }
                    }

                    break;
                }
            case SC_HTTPS_DEMO_DATA:
                {
                    if (handle_params(http_data_params, sizeof(http_data_params)/sizeof(http_data_params[0])) == 0)
                    {
                        sAPI_Debug("http_data.buffer=%s", http_data.data);
						http_data.len = strlen(http_data.data);
                        sAPI_Debug("http_data.len=%d", http_data.len);
                        ret = sAPI_HttpData(http_data.data, http_data.len);
                        if(ret == SC_HTTPS_SUCCESS)
                        {
                            PrintfResp("\r\nhttps set data success\r\n");
                        }
                        else
                        {
                            PrintfResp("\r\nhttps set data fail\r\n");
                        }
                    }

                    break;
                }
            case SC_HTTPS_DEMO_ACTION:
                {
                    if (handle_params(http_action_params, sizeof(http_action_params)/sizeof(http_action_params[0])) == 0)
                    {
                        sAPI_Debug("http_acton.method=%d", http_action.method);
                        ret = sAPI_HttpAction(http_action.method);
                        if(ret == SC_HTTPS_SUCCESS)
                        {
                            PrintfResp("\r\nhttps action success\r\n");
                            show_httpaction_urc();
                        }
                        else
                        {
                            PrintfResp("\r\nhttps action fail\r\n");
                        }
                    }

                    break;
                }
            case SC_HTTPS_DEMO_HEAD:
                {
                    ret = sAPI_HttpHead();
                    if(ret == SC_HTTPS_SUCCESS)
                    {
                        PrintfResp("\r\nhttps read head success\r\n");
                        show_httphead_urc();
                    }
                    else
                    {
                        PrintfResp("\r\nhttps read head fail\r\n");
                    }

                    break;
                }
            case SC_HTTPS_DEMO_READ:
                {
                    if (handle_params(http_read_params, sizeof(http_read_params)/sizeof(http_read_params[0])) == 0)
                    {
                        sAPI_Debug("http_read.type=%d", http_read.type);
                        sAPI_Debug("http_read.offset=%d", http_read.offset);
                        sAPI_Debug("http_read.size=%d", http_read.size);
                        if (http_read.size < 0)
                        {
                            PrintfResp("\r\nhttps read fail\r\n");
                            break;
                        }
                        ret = sAPI_HttpRead(http_read.type, http_read.offset, http_read.size);
                        if(ret == SC_HTTPS_SUCCESS)
                        {
                            PrintfResp("\r\nhttps read success\r\n");
                            show_httpread_urc(http_read.type);
                        }
                        else
                        {
                            PrintfResp("\r\nhttps read fail\r\n");
                        }
                    }

                    break;
                }
            case SC_HTTPS_DEMO_POSTFILE:
                {
                    if (handle_params(http_postfile_params, sizeof(http_postfile_params)/sizeof(http_postfile_params[0])) == 0)
                    {
                        sAPI_Debug("http_postfile.filename=%s", http_postfile.filename);
                        sAPI_Debug("http_postfile.dir=%d", http_postfile.dir);
                        ret = sAPI_HttpPostfile(http_postfile.filename, http_postfile.dir);
                        if(ret == SC_HTTPS_SUCCESS)
                        {
                            PrintfResp("\r\nhttps post file success\r\n");
                            show_httpaction_urc();
                        }
                        else
                        {
                            PrintfResp("\r\nhttps post file fail\r\n");
                        }
                    }

                    break;
                }
            case SC_HTTPS_DEMO_TERM:
                {
                    ret = sAPI_HttpTerm();
                    if(ret == SC_HTTPS_SUCCESS)
                    {
                        PrintfResp("\r\nhttp term success\r\n");
                    }
                    else
                    {
                        PrintfResp("\r\nhttp term fail\r\n");
                    }
                    break;
                }

            case SC_HTTPS_DEMO_MAX:
                {
                    sAPI_Debug("Return to the previous menu!");
                    PrintfResp("\r\nReturn to the previous menu!\r\n");
                    ret = sAPI_HttpTerm();
                    return;
                }

            default :
                break;

        }

    }
}
