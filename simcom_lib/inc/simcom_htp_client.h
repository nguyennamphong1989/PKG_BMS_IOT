#ifndef  SIMCOM_HTP_CLIENT_H
#define  SIMCOM_HTP_CLIENT_H


#include "simcom_os.h"





typedef enum
{
  SC_HTP_OK = 0,
  SC_HTP_ERROR,
  SC_HTP_UNKNOWN_ERROR, 
  SC_HTP_INVALID_PARAM,  
  SC_HTP_BAD_DATETIME_GOT,  
  SC_HTP_NETWORK_ERROR,  
  SC_HTP_END
}SChtpResultType;

typedef SChtpResultType SChtpReturnCode;

typedef enum {
    SC_HTP_OP_SET = 0,
    SC_HTP_OP_GET,
}SChtpOperationType;








#endif

