/*
 * Created on 2021-08-16 14:48:26
 *
 * @Author: yonghang.qin
 * @Date: 2021-08-16 14:48:26
 * @LastEditors: yonghang.qin
 * @LastEditTime: 2021-09-02 14:08:49
 *
 * @Copyright (c) 2021 simcom
 */

#include "simcom_wifi.h"
#include "simcom_api.h"

enum
{
    SC_WIFI_DEMO_START_SCANNING           = 1,
    SC_WIFI_DEMO_STOP_SCANNING            = 2,
    SC_WIFI_DEMO_MAX                      = 99
};

extern sMsgQRef simcomUI_msgq;
extern void PrintfOptionMenu(INT8* options_list[], int array_size);
extern void PrintfResp(INT8* format);

static void wifi_handle_event(const void *param)
{
    const SC_WIFI_INFO_T *scan_result = (const SC_WIFI_INFO_T *)param;

    char rspBuf[128];
    int size = 0;

    size = snprintf(rspBuf, sizeof(rspBuf),
        "\r\nscan result: mac address(%02x:%02x:%02x:%02x:%02x:%02x), channel(%d), rssi(%d)\r\n",
        scan_result->mac_addr[5], scan_result->mac_addr[4], scan_result->mac_addr[3],
        scan_result->mac_addr[2], scan_result->mac_addr[1], scan_result->mac_addr[0],
        scan_result->channel_number, scan_result->rssi);

    PrintfResp((INT8 *)rspBuf);
}

void WIFIDemo(void)
{
    char flag = 1;
    INT8 *note = "\r\nPlease select an option to test from the items listed below.\r\n";
    INT8 *options_list[] = {
       "1. Start scanning",
       "2. Stop scanning",
       "99. back",
    };

    while (flag)
    {
        PrintfResp(note);
        PrintfOptionMenu(options_list, sizeof(options_list) / sizeof(options_list[0]));

        SIM_MSG_T optionMsg;
        sAPI_MsgQRecv(simcomUI_msgq, &optionMsg, SC_SUSPEND);
        if (SRV_UART != optionMsg.msg_id)
        {
            sAPI_Debug("%s,msg_id is error!!", __func__);
            break;
        }

        unsigned char opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);

        switch (opt)
        {
            case SC_WIFI_DEMO_START_SCANNING:
            {
                sAPI_WifiSetHandler(wifi_handle_event);
                sAPI_WifiScanStart();
                PrintfResp("\r\nPlease wait a moment, scanning...\r\n");
                break;
            }

            case SC_WIFI_DEMO_STOP_SCANNING:
            {
                sAPI_WifiScanStop();
                PrintfResp("\r\nPlease wait a moment, stopping...\r\n");
                break;
            }

            case SC_WIFI_DEMO_MAX:
            {
                flag = 0;
                PrintfResp("\r\nReturn to the previous menu!\r\n");
                break;
            }

            default:
                PrintfResp("\r\nPlease select again:\r\n");
        }
    }
}
