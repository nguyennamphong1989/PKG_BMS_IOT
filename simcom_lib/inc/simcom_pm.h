/**
* @file         simcom_pm.h
* @brief        SIMCom PMU API
* @author       dengchao
* @date         2020/5/4
* @version      V1.0.0
* @par Copyright (c):
*               SIMCom Co.Ltd 2003-2019
* @par History: 1:Create
*
*/

#ifndef __SIMCOM_PM_H__
#define __SIMCOM_PM_H__


typedef enum {
	SC_ONKEY_PRESSED,
	SC_ONKEY_RELEASED
}ONKEY_STATUS;

typedef enum
{
    POWER_UP_RESET_KEY = 1,
    POWER_UP_POWER_KEY,
    POWER_UP_SOFTWARE_RESET,
    POWER_UP_RTC_ALARM,
}POWER_UP_REASON;

typedef enum
{
    POWER_DOWN_SW_PDOWN_LONGPRESS_POWER_KEY = 1,
    POWER_DOWN_VRTC_DOWN_VRTC_MIN_TH,
    POWER_DOWN_OVERTEMPERATURE,
    POWER_DOWN_VIN_LDO_DOWN_UV_VSYS1_DETECT,
    POWER_DOWN_PMIC_WATCH_DOG_EXPIRY_EVENT,
    POWER_DOWN_LONGPRESS_RESET_KEY,
    POWER_DOWN_VIN_LDO_UP_VSYS_OVER_TH,

}POWER_DOWN_REASON;

typedef void (*PowerKeyIntCallback)(void);
#endif
