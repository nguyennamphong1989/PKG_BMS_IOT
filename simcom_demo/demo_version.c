#include "simcom_api.h"
#include "simcom_system.h"

unsigned int sc_getimei(void)
{
    int ret = 0;
    char imei_value[16];
    ret = sAPI_SysGetImei(imei_value);
    sAPI_Debug("imei_value : %s", imei_value);
    return ret;
}

void sc_getversioninfo(void)
{
    SIMComVersion simcominfo;
    sAPI_SysGetVersion (&simcominfo);
    sAPI_Debug("manufacture_namestr: %s ; Module_modelstr: %s ; Revision: %s ; GCAP: %s ;", 
        simcominfo.manufacture_namestr, simcominfo.Module_modelstr, simcominfo.Revision, simcominfo.GCAP);
    sAPI_Debug("cgmr: %s ; internal_verstr: %s ; version_tagstr: %s ; SDK_Version: %s ;", 
        simcominfo.cgmr, simcominfo.internal_verstr, simcominfo.version_tagstr, simcominfo.SDK_Version);
    return;
}

void sc_getcusversion(void)
{
    CUSVersion CUSinfo;
    sAPI_SysGetCusVersion (&CUSinfo);
    sAPI_Debug("cusversion: %s",CUSinfo.cusversion);
    return;
}

void sc_getrfversion(void)
{
    RFVersion RFinfo;
    sAPI_SysGetRFVersion (&RFinfo);
    sAPI_Debug("rfversion1: %s ; rfversion2:%s ", RFinfo.rfversion1, RFinfo.rfversion1);
    return;
}


void sAPP_VersionDemo(void)
{
    if(0 != sc_getimei())
    {
        sAPI_Debug("get IMEI err");
    }
    
    sc_getversioninfo();

    sc_getcusversion();

    sc_getrfversion();

    return;
}

