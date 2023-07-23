/** 
* @file         simcom_gps.h 
* @brief        SIMCom GNSS API
* @author       HT
* @date         2019/9/13
* @version      V1.0.0 
* @par Copyright (c):  
*               SIMCom Co.Ltd 2003-2019
* @par History: 1:Create         
*   
*/

#ifndef SIMCOMGPS_H
#define SIMCOMGPS_H


/****************************************************************************
    Define enum  
*****************************************************************************/
typedef enum
{
    SC_GNSS_RETURN_CODE_OK                =  0,
    SC_GNSS_RETURN_CODE_ERROR             = -1,

    SC_GNSS_CODE_SOCKRT_ERROR             = 101,
    SC_GNSS_CODE_GET_SERVER_ERROR         = 102,
    SC_GNSS_CODE_CONNECT_ERROR            = 103,
    SC_GNSS_CODE_WRITE_SOCKET_ERROR       = 104,
    SC_GNSS_CODE_READ_SOCKET_ERROR        = 105

}SC_Gnss_Return_Code;

typedef enum
{
    SC_GNSS_POWER_OFF = 0,
    SC_GNSS_POWER_ON =  1
}SC_Gnss_Power_Status;

typedef enum
{
    SC_GNSS_AP_FLASH_OFF = 0,
    SC_GNSS_AP_FLASH_ON =  1
}SC_Gnss_Ap_Flash_Status;

typedef enum
{
    SC_GNSS_STOP_OUTPUT_NMEA_DATA  = 0,
    SC_GNSS_START_OUTPUT_NMEA_DATA =  1
}SC_Gnss_Output_Control;

typedef enum
{
    SC_GNSS_NMEA_DATA_GET_BY_PORT = 0,   //get data by NMEA port
    SC_GNSS_NMEA_DATA_GET_BY_URC  =  1   //get data by URC report
}SC_Gnss_Nmea_Data_Get;

typedef enum
{
    SC_GNSS_START_HOT        = 0,  /*0:hot 1:warm 2:cold*/
    SC_GNSS_START_WARM       = 1,
    SC_GNSS_START_COLD       = 2,
	SC_GNSS_START_MAX
}SC_Gnss_Start_Mode;

typedef enum
{
    //SC_GNSS_BAUD_RATE_4800         = 4800,
    SC_GNSS_BAUD_RATE_9600         = 9600,
    //SC_GNSS_BAUD_RATE_19200        = 19200,
    //SC_GNSS_BAUD_RATE_38400        = 38400,
    //SC_GNSS_BAUD_RATE_57600        = 57600,
    SC_GNSS_BAUD_RATE_115200       = 115200,
    SC_GNSS_BAUD_RATE_230400       = 230400,
    SC_GNSS_BAUD_RATE_MAX
}SC_Gnss_Baud_Rate;

typedef enum
{
#ifdef FEATURE_SIMCOM_GPS_OVERSEAS
    SC_GNSS_MODE_GPS_L1_SBAS_QZSS                  = 1,
    SC_GNSS_MODE_BDS                               = 2,
    SC_GNSS_MODE_GPS_GLONASS_GALILEO_SBAS_QZSS     = 3,
    SC_GNSS_MODE_GPS_BDS_GALILEO_SBAS_QZSS         = 4,
#else
    SC_GNSS_MODE_GPS_BDS_QZSS       = 1,
    SC_GNSS_MODE_BDS                = 2,
    SC_GNSS_MODE_GPS_QZSS           = 3,
    //SC_GNSS_MODE_GLONASS            = 4,
    //SC_GNSS_MODE_GPS_GLONASS        = 5,
    //SC_GNSS_MODE_BDS_GLONASS        = 6,
    //SC_GNSS_MODE_GPS_BDS_GLONASS    = 7,
#endif
    SC_GNSS_MODE_MAX
}SC_Gnss_Mode;

typedef enum
{
    SC_GNSS_NMEA_UPDATE_RATE_1HZ        = 1,
    SC_GNSS_NMEA_UPDATE_RATE_2HZ        = 2,
    SC_GNSS_NMEA_UPDATE_RATE_5HZ        = 5,
    SC_GNSS_NMEA_UPDATE_RATE_10HZ       = 10,
	SC_GNSS_NMEA_RATE_MAX         
}SC_Gnss_Nmea_Rate;

/****************************************************************************
    Define struct 
*****************************************************************************/



#endif


