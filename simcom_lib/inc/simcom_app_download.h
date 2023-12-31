#ifndef _SIMCOM_FOTA_H_
#define _SIMCOM_FOTA_H_
#include "simcom_os.h"

typedef enum{
    SC_APP_DOWNLOAD_HTTP_MOD,
    SC_APP_DOWNLOAD_FTP_MOD,
    SC_APP_DOWNLOAD_MAX
}SCAppDownloadMod;


typedef enum{
    SC_APP_DOWNLOAD_SUCESSED,
    SC_APP_DOWNLOAD_PDP_ACTIVE_FAIL,
    SC_APP_DOWNLOAD_OPERATION_BUSY,
    SC_APP_DOWNLOAD_CONN_REJECT,  //connect server fail
    SC_APP_DOWNLOAD_DOMAIN_UNKNOWN,
    SC_APP_DOWNLOAD_SEND_ERR,//5
    SC_APP_DOWNLOAD_RECV_ERR,
    SC_APP_DOWNLOAD_PARAM_INVALID,
    SC_APP_DOWNLOAD_UNKNOWN_ERR,
    SC_APP_DOWNLOAD_OPEN_PARTITION_FAIL,
    SC_APP_DOWNLOAD_WRITE_PARTITION_FAIL,//10
    SC_APP_DOWNLOAD_NO_MEMORY,
    SC_APP_DOWNLOAD_FILE_NOT_EXIST,
    SC_APP_DOWNLOAD_FAIL

}SCAppDwonLoadReturnCode;

#define SC_FOTA_URL_MAX_SIZE (256)


typedef struct{
    SCAppDownloadMod mod;
    INT8* url;
    UINT32 recvtimeout;
    
}SCAppDownloadPram;




#endif


