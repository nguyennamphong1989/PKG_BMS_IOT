/*
 * Created on 2021-08-16 14:52:01
 *
 * @Author: yonghang.qin
 * @Date: 2021-08-16 14:52:01
 * @LastEditors: yonghang.qin
 * @LastEditTime: 2021-08-16 17:17:25
 *
 * Copyright (c) 2021 simcom
 */

#ifndef __SIMCOM_WIFI_H__
#define __SIMCOM_WIFI_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define SC_WIFI_INFO_NUMBERS_MAX (30)

typedef struct
{
    unsigned char mac_addr[6];
    unsigned char padding[2];
    int rssi;
    unsigned int channel_number;
} SC_WIFI_INFO_T;

#if 0

/*
 * 原型:      sAPI_WifiSetHandler
 * 功能:  设置WIFI处理函数回调。WIFI的所有操作执行结束后，将通过回调函数通知外部业务。
 *
 * 参数:
 *      handler:    回调函数原型。
 *
 * 返回值：null。
 */
void sAPI_WifiSetHandler(void (*handler)(const void *param));

/*
 * 原型:      sAPI_WifiScanStart
 * 功能:  开启WIFI扫描功能。
 *
 * 参数:
 *      null。
 *
 * 返回值：null。
 */
void sAPI_WifiScanStart(void);

/*
 * 原型:      sAPI_WifiScanStop
 * 功能:  停止WIFI扫描功能。
 *
 * 参数:
 *      null。
 *
 * 返回值：null。
 */
void sAPI_WifiScanStop(void);

#endif

#ifdef __cplusplus
}
#endif

#endif
