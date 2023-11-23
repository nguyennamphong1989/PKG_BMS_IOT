#include "simcom_api.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#define APP_VER "0.1"

#ifdef SIMCOM_UI_DEMO
sMsgQRef urc_mqtt_msgq_1;

typedef enum
{
    SC_DEMO_FOR_NETWORK = 1,           // API test for network
    SC_DEMO_FOR_SIMCARD = 2,           // API test for SIM Card
    SC_DEMO_FOR_SMS = 3,               // API test for SMS
    SC_DEMO_FOR_UART = 4,              // API test for UART
    SC_DEMO_FOR_USB = 5,               // API test for USB
    SC_DEMO_FOR_GPIO = 6,              // API test for GPIO
    SC_DEMO_FOR_PMU = 7,               // API test for PMU
    SC_DEMO_FOR_I2C = 8,               // API test for I2C
    SC_DEMO_FOR_AUDIO = 9,             // API test for audio
    SC_DEMO_FOR_FILE_SYSTEM = 10,      // API test for File System
    SC_DEMO_FOR_TCP_IP = 11,           // API test for TCP/IP
    SC_DEMO_FOR_HTTP_HTTPS = 12,       // API test for HTTP(s)
    SC_DEMO_FOR_FTP_FTPS = 13,         // API test for FTP(s)
    SC_DEMO_FOR_MQTT_MQTTS = 14,       // API test for MQTT(s)
    SC_DEMO_FOR_SSL = 15,              // API test for SSL
    SC_DEMO_FOR_OTA = 16,              // API test for OTA
    SC_DEMO_FOR_LBS = 17,              // API test for LBS
    SC_DEMO_FOR_NTP = 18,              // API test for NTP
    SC_DEMO_FOR_HTP = 19,              // API test for HTP
    SC_DEMO_FOR_INTERNET_SERVICE = 20, // API test for Internet service
    SC_DEMO_FOR_TTS = 21,              // API test for TTS
    SC_DEMO_FOR_CALL = 22,             // API test for CALL
    SC_DEMO_FOR_WIFI = 23,             // API test for WIFI
#ifdef FEATURE_SIMCOM_GPS
    SC_DEMO_FOR_GNSS = 24, // API test for GNSS
    SC_DEMO_FOR_LCD = 25,  // API test for LCD
#else
    SC_DEMO_FOR_LCD = 24, // API test for LCD
#endif
    SC_DEMO_FOR_RTC = 26,   // API test for RTC
    SC_DEMO_FOR_FLASH = 27, // API test for flash
#ifdef FEATURE_SIMCOM_FS_OLD
    SC_DEMO_FOR_FILE_SYSTEM_OLD = 28, // API test for File System of 1601
#endif
    SC_DEMO_FOR_SPI = 29,
    SC_DEMO_FOR_CAM = 30, // API test for CAM
#ifdef BT_SUPPORT
    SC_DEMO_LE_CLIENT = 31,
#endif
    SC_DEMO_FOR_SPI_NOR = 32,
    SC_DEMO_FOR_APP_DOWNLOAD = 33,

} SC_DEMO_TYPE;

sMsgQRef simcomUI_msgq;
sTaskRef simcomUIProcesser;
static UINT8 simcomUIProcesserStack[1024 * 30];
extern void NetWorkDemo(void);
extern void SMSDemo(void);
extern void FtpsDemo(void);
extern void TcpipDemo(void);
extern void HttpsDemo(void);
extern void UartDemo(void);
extern void GpioDemo(void);
extern void LbsDemo(void);
extern void SslDemo(void);
extern void MqttDemo(void);
extern void NtpDemo(void);
extern void HtpDemo(void);
extern void FsDemo(void);
extern void AudioDemo(void);
extern void TTSDemo(void);
extern void CALLDemo(void);
extern void WIFIDemo(void);
extern void FotaDemo(void);
extern void GNSSDemo(void);
extern void LcdDemo(void);
extern void RTCDemo(void);
extern void SimcardDemo(void);
extern void FlashRWdemo(void);
extern void Fs2Demo(void);
extern void PMUDemo(void);
extern void I2cDemo(void);
extern void SpiDemo(void);
extern void SpiNorDemo(void);
extern void CamDemo(void);
extern void LEClientDemo(void);
extern void AppDownloadDemo(void);

extern void di_config(void);

void PrintfResp(INT8 *format)
{
    UINT32 length = strlen(format);
#ifdef SIMCOM_UI_DEMO_TO_UART1_PORT
    sAPI_UartWrite(SC_UART, (UINT8 *)format, length);
#else
    sAPI_UsbVcomWrite((UINT8 *)format, length);
#endif
}

void PrintfOptionMenu(INT8 *options_list[], int array_size)
{
    UINT32 i = 0;
    sAPI_Debug("array_size = [%d]", array_size);
    INT8 menu[80] = {0};
    PrintfResp("\r\n************************************************************\r\n");
    for (i = 0; i < (array_size / 2); i++)
    {
        memset(menu, 0, 80);
        snprintf(menu, 80, "%-30s%-30s", options_list[2 * i], options_list[2 * i + 1]);
        PrintfResp(menu);
        PrintfResp("\r\n");
    }

    if (array_size % 2 != 0)
    {
        memset(menu, 0, 80);
        snprintf(menu, 80, "%s", options_list[array_size - 1]);
        PrintfResp(menu);
        PrintfResp("\r\n");
    }
    PrintfResp("************************************************************\r\n");
}

SIM_MSG_T GetParamFromUart(void)
{
    SIM_MSG_T optionMsg = {0, 0, 0, NULL};
    sAPI_MsgQRecv(simcomUI_msgq, &optionMsg, SC_SUSPEND);

    return optionMsg;
}
/*
void sTask_SimcomUIProcesser(void *arg)
{
    SIM_MSG_T optionMsg = {0, 0, 0, NULL};
    UINT32 opt = 0;
    di_config();
    while (1)
    {
        PrintfResp("Welcome to WPE_RainDL, App version ");
        PrintfResp(APP_VER);
        PrintfResp("\r\n");
        sAPI_MsgQRecv(simcomUI_msgq, &optionMsg, SC_SUSPEND);
        opt = atoi(optionMsg.arg3);
        sAPI_Free(optionMsg.arg3);
    }
}
*/
void sTask_SimcomUIProcesser(void *arg)
{
    SIM_MSG_T optionMsg = {0, 0, 0, NULL};
    UINT32 opt = 0;
    INT8 *note = "Please select an option to test from the items listed below.\n";
    INT8 *options_list[] = {
        "1. NETWORK",
        "2. SIMCARD",
        "3. SMS",
        "4. UART",
#ifndef AT_COMMAND_SUPPORT
        "5. USB",
#endif
        "6. GPIO",
        "7. PMU",
        "8. I2C",
        "9. AUDIO",
        "10. FILE SYSTEM",
        "11. TCPIP",
        "12. HTTP",
        "13. FTP",
        "14. MQTT",
        "15. SSL",
        "16. FOTA",
        "17. LBS",
        "18. NTP",
        "19. HTP",
        "20. INTERNET SERVICE",
        "21. TTS",
        "22. CALL",
        "23. WIFI",
#ifdef FEATURE_SIMCOM_GPS
        "24. GNSS",
        "25. LCD",
#else
        "24. LCD",
#endif
        "26. RTC",
        "27. FLASH",

#ifdef FEATURE_SIMCOM_FS_OLD
        "28. FILE SYSTEM Compatible API",
#endif
        "29. SPI",
        "30. CAM",
        "31. LE CLIENT",
        "32. SPI NOR",
        "33. APP DOWNLOAD"};

    while (1)
    {
        di_config();
    }
}

void sAPP_SimcomUIDemo(void)
{
    SC_STATUS status;
    status = sAPI_MsgQCreate(&simcomUI_msgq, "simcomUI_msgq", sizeof(SIM_MSG_T), 12, SC_FIFO);
    if (SC_SUCCESS != status)
    {
        sAPI_Debug("msgQ create fail");
    }
    status = sAPI_MsgQCreate(&urc_mqtt_msgq_1, "urc_mqtt_msgq_1", (sizeof(SIM_MSG_T)), 1, SC_FIFO); // msgQ for subscribed data transfer
    if (status != SC_SUCCESS)
    {
        sAPI_Debug("message queue creat err!\n");
    }
    status = sAPI_TaskCreate(&simcomUIProcesser, simcomUIProcesserStack, 1024 * 30, 100, "simcomUIProcesser", sTask_SimcomUIProcesser, (void *)0);
    if (SC_SUCCESS != status)
    {
        sAPI_Debug("task create fail");
    }
}
#endif
