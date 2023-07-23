#ifndef SIMCOM_NTP_CLIENT_H_
#define SIMCOM_NTP_CLIENT_H_

#include "simcom_os.h"




typedef enum {
    SC_NTP_OP_SET,
    SC_NTP_OP_GET,
    SC_NTP_OP_EXC,
}SCntpOperationType;

typedef struct SCsysTime_s {
  int tm_sec;	//seconds [0,59]
  int tm_min;	//minutes [0,59]
  int tm_hour;  //hour [0,23]
  int tm_mday;  //day of month [1,31]
  int tm_mon;   //month of year [1,12]
  int tm_year; // since 1970
  int tm_wday; // sunday = 0
}SCsysTime_t;

typedef enum {
    SC_NTP_OK = 0,
    SC_NTP_ERROR,
    SC_NTP_ERROR_INVALID_PARAM,
    SC_NTP_ERROR_TIME_CALCULATED,
    SC_NTP_ERROR_NETWORK_FAIL,
    SC_NTP_ERROR_TIME_ZONE,
    SC_NTP_ERROR_TIME_OUT,
    SC_NTP_END
}SCntpResultType;

typedef SCntpResultType SCntpReturnCode;



#endif

