#ifndef __SIMCOM__FOTA_H__
#define __SIMCOM__FOTA_H__
#define SC_FOTA_MAX_STRING_LEN 128
typedef int (*sc_fota_callback)(int);

struct SC_FotaApiParam
{
	CHAR host[SC_FOTA_MAX_STRING_LEN]; /*xx.xx.xx.xx:port, or URL*/
	CHAR username[SC_FOTA_MAX_STRING_LEN];
	CHAR password[SC_FOTA_MAX_STRING_LEN];
	UINT8 mode; /*0: ftp, 1: http*/
    sc_fota_callback sc_fota_cb;
    int timeout;
    int ssl_index;//reverd
    int cid;
};



#endif

