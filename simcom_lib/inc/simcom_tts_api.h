#ifndef _SIMCOM_TTS_API_H_
#define _SIMCOM_TTS_API_H_

#include "simcom_os.h"

 typedef enum
 {
    NO_WORKING,
    TTS_WORKING,
    STATE_MAX,
}TTSPlayerStatus;

typedef enum
{
    SC_TTS_START,
    SC_TTS_STOP,
    SC_TTS_STATUS_MAX,
}TTSStatus;

typedef void (*sAPI_TTSStatussCb)(TTSStatus flag);
#endif



