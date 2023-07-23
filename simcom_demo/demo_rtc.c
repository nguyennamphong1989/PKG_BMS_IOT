#include "simcom_api.h"
#include "simcom_gpio.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

//#define RTC_GPIO_TEST

enum SC_RTC_OPTION
{
    SC_RTC_SET_TIME             = 1,
    SC_RTC_GET_TIME             = 2,
    SC_RTC_SET_ALARM            = 3,
    SC_RTC_GET_ALARM            = 4,
    SC_RTC_ENABLE_ALARM         = 5,
    SC_RTC_REGISTER_ALARM_CB    = 6,
    SC_RTC_ALARM_TEST           = 7,
    SC_RTC_BACK = 99
};

extern sMsgQRef simcomUI_msgq;
extern SIM_MSG_T GetParamFromUart(void);

AlCallback cbfunc(void)
{
#ifdef RTC_GPIO_TEST
    static unsigned int i = 0;

    if(i%2 == 0)
        sAPI_GpioSetValue(SC_MODULE_GPIO_10, 1);
    else
        sAPI_GpioSetValue(SC_MODULE_GPIO_10, 0);

    i++;
#endif
    sAPI_Debug("alarm callback !");
}

void setAlarmMin(t_rtc alarmval, int min)
{
    t_rtc tmpVal = alarmval;
    int tmp = 0;
    char buff[100] = {0};

    tmp = tmpVal.tm_min + min;
    tmpVal.tm_min = tmp % 60;

    tmp = tmpVal.tm_hour + (tmp/60);
    tmpVal.tm_hour = tmp % 60;

    tmp = tmpVal.tm_mday + (tmp/60);
    tmpVal.tm_mday = tmp % 24;

    sAPI_RtcSetAlarm(&tmpVal);

    sprintf(buff, "\r\nALARM: set time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d \r\n", tmpVal.tm_year, tmpVal.tm_mon, tmpVal.tm_mday,
        tmpVal.tm_hour, tmpVal.tm_min, tmpVal.tm_sec);
    PrintfResp(buff);
}

static int gAlarm = 0;
AlCallback alarmTest(void)
{
    t_rtc timeval;
    t_rtc alarmval;
    unsigned int cnr = 0;
    unsigned int ar = 0;
    char buff[100] = {0};

    /* get rtc time */
    sAPI_GetRealTimeClock(&timeval);

    /* get alarm time */
    sAPI_RtcGetAlarm(&alarmval);

    /* set alarm time */
    if(gAlarm)
    {
        setAlarmMin(alarmval, gAlarm);
        sAPI_RtcEnableAlarm(1);
    }

    /* debug */
    sprintf(buff, "\r\nRTC: time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d \r\n", timeval.tm_year, timeval.tm_mon, timeval.tm_mday,
        timeval.tm_hour, timeval.tm_min, timeval.tm_sec);
    PrintfResp(buff);

    sprintf(buff, "\r\nALARM: time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d \r\n", alarmval.tm_year, \
        alarmval.tm_mon, alarmval.tm_mday, alarmval.tm_hour, alarmval.tm_min, alarmval.tm_sec);
    PrintfResp(buff);
}

void RTCDemo(void)
{
    SC_STATUS status = SC_SUCCESS;
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    int opt,ret;
    t_rtc timeval;
    char buff[100] = {0};

    INT8 *note = "\r\nPlease select an option to test from the items listed below.\r\n";
    INT8 *options_list[] = {
        "1. Set time",
        "2. get time",
        "3. Set alarm",
        "4. Get alarm",
        "5. enable alarm",
        "6. register alarm cb",
        "7. rtc alarm test",
        "99. back",
    };


#ifdef RTC_GPIO_TEST
    SC_GPIOReturnCode ret1;

    SC_GPIOConfiguration pinConfig;
    pinConfig.initLv = 0;
    pinConfig.isr =  NULL;
    pinConfig.pinDir = SC_GPIO_OUT_PIN;
    pinConfig.pinEd = SC_GPIO_NO_EDGE;
    pinConfig.wu = NULL;
    pinConfig.pinPull = SC_GPIO_PULLUP_ENABLE;//pull_up

    ret1 = sAPI_GpioConfig(SC_MODULE_GPIO_10, pinConfig);
    if(ret1 == SC_GPIORC_OK)
    {
        PrintfResp("\r\nConfig GPIO_10 successed !\r\n");
    }
    else
    {
        PrintfResp("\r\nConfig GPIO_10 failed !\r\n");
    }
#endif
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
            case SC_RTC_SET_TIME:
            {
                    CHAR timeStr[21];
                    int monthLen[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                    int retval = -1;
                    int timezone = 0;

                    PrintfResp("\r\nPlease input time\r\n");
                    PrintfResp("\r\n   such as: 21/03/08,09:36:00 \r\n");
                    optionMsg = GetParamFromUart();
                    strcpy(timeStr,optionMsg.arg3);
                    sAPI_Free(optionMsg.arg3);

                    retval = sscanf((char* )timeStr, "%2d/%2d/%2d,%2d:%2d:%2d", &timeval.tm_year,&timeval.tm_mon, \
                                    &timeval.tm_mday,&timeval.tm_hour,&timeval.tm_min,&timeval.tm_sec);
                    timeval.tm_year = timeval.tm_year >= 70 ? (1900 + timeval.tm_year) : (2000 + timeval.tm_year);
                    if(retval < 6)
                    {
                        PrintfResp("\r\nPlease input correct value\r\n");
                        break;
                    }

                    if(((timeval.tm_year%4 == 0) && (timeval.tm_year%100 != 0)) || (timeval.tm_year%400 == 0))
                        monthLen[1] += 1;

                    if(((timeval.tm_year >= 1970) && (timeval.tm_year < 2070)) && ((timeval.tm_mon > 0) && (timeval.tm_mon <= 12)) && ((timeval.tm_mday > 0) && (timeval.tm_mday <= monthLen[timeval.tm_mon-1]))   \
                        && ((timeval.tm_hour >= 0 ) && (timeval.tm_hour < 24 )) && ((timeval.tm_min >= 0 ) && (timeval.tm_min < 60)) && ((timeval.tm_sec >= 0 ) && (timeval.tm_sec < 60))   \
                        && ((timezone >= -96 ) && (timezone <= 96 )))
                    {
                        ret = sAPI_SetRealTimeClock(&timeval);
                        if(ret < 0)
                            PrintfResp("\r\n set time failed 1 !\r\n");
                        else
                            PrintfResp("\r\n set time successed !\r\n");
                    }
                    else
                    {
                        PrintfResp("\r\n set time failed 2 !\r\n");
                    }

                    break;
            }

            case SC_RTC_GET_TIME:
            {
                    sAPI_GetRealTimeClock(&timeval);
                    sprintf(buff,"\r\n Get RTC time: %d/%02d/%02d,%02d:%02d:%02d \r\n", timeval.tm_year, timeval.tm_mon, timeval.tm_mday,
                        timeval.tm_hour, timeval.tm_min, timeval.tm_sec);
                    PrintfResp(buff);
                    break;
            }

            case SC_RTC_SET_ALARM:
            {
                    CHAR timeStr[21];
                    int monthLen[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                    int retval = -1;
                    int timezone = 0;

                    PrintfResp("\r\nPlease input time\r\n");
                    PrintfResp("\r\n   such as: 21/03/08,09:36:00 \r\n");
                    optionMsg = GetParamFromUart();
                    strcpy(timeStr,optionMsg.arg3);
                    sAPI_Free(optionMsg.arg3);

                    retval = sscanf((char* )timeStr, "%2d/%2d/%2d,%2d:%2d:%2d", &timeval.tm_year,&timeval.tm_mon, \
                                    &timeval.tm_mday,&timeval.tm_hour,&timeval.tm_min,&timeval.tm_sec);
                    timeval.tm_year = timeval.tm_year >= 70 ? (1900 + timeval.tm_year) : (2000 + timeval.tm_year);
                    if(retval < 6)
                    {
                        PrintfResp("\r\nPlease input correct value\r\n");
                        break;
                    }

                    if(((timeval.tm_year%4 == 0) && (timeval.tm_year%100 != 0)) || (timeval.tm_year%400 == 0))
                        monthLen[1] += 1;

                    sAPI_RtcSetAlarm(&timeval);
                    sprintf(buff,"\r\n set Alarm  %d \r\n",0);
                    PrintfResp(buff);
                    break;
            }

            case SC_RTC_GET_ALARM:
            {
                    sAPI_RtcGetAlarm(&timeval);
                    sprintf(buff,"\r\nAlarmTime: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d \r\n",timeval.tm_year, \
                    timeval.tm_mon, timeval.tm_mday, timeval.tm_hour ,timeval.tm_min,timeval.tm_sec);
                    PrintfResp(buff);
                    break;
            }

            case SC_RTC_ENABLE_ALARM:
            {
                    CHAR timeStr[3];
                    int onoff = -1;
                    PrintfResp("\r\nPlease input:\r\n");
                    PrintfResp("\r\n 1:enable   0:disable\r\n");
                    optionMsg = GetParamFromUart();
                    onoff = atoi(optionMsg.arg3);
                    sAPI_Free(optionMsg.arg3);
                    if(onoff < 0)
                    {
                        PrintfResp("\r\ninput error\r\n");
                        break;
                    }
                    sAPI_RtcEnableAlarm(onoff);
                    break;
            }

            case SC_RTC_REGISTER_ALARM_CB:
            {
                    sAPI_RtcRegisterCB(cbfunc);
                    sprintf(buff,"\r\n set Alarm cb ret = %d \r\n",2);
                    PrintfResp(buff);
                    break;
            }

            case SC_RTC_ALARM_TEST:
            {
                    CHAR timeStr[3];
                    PrintfResp("\r\nPlease input alarm space(/min):\r\n");
                    optionMsg = GetParamFromUart();
                    gAlarm = atoi(optionMsg.arg3);
                    sAPI_Free(optionMsg.arg3);

                    sAPI_GetRealTimeClock(&timeval);
                    timeval.tm_sec = 0;
                    setAlarmMin(timeval, 1);
                    sAPI_RtcRegisterCB(alarmTest);
                    if(gAlarm)
                        sAPI_RtcEnableAlarm(1);
                    else
                        sAPI_RtcEnableAlarm(0);
                    break;
            }

            case 99:
            {
                    return;
            }
        }
    }
}

