#include "simcom_api.h"


#define NAU8810_SLAVE_ADDR 0x34

enum SC_I2C_OPS
{
    SC_I2C1_OPEN = 1,
    SC_I2C1_WRITE,
    SC_I2C1_READ,
    SC_I2C1_READEX,
    SC_I2C1_WRITEEX,
    SC_I2C2_OPEN,
    SC_I2C2_WRITE,
    SC_I2C2_READ,
    SC_I2C2_READEX,
    SC_I2C2_INIT,
    SC_GPIO_BACK = 99
};

extern SIM_MSG_T GetParamFromUart(void);
extern sMsgQRef simcomUI_msgq;
extern void PrintfOptionMenu(INT8* options_list[], int array_size);
extern void PrintfResp(INT8* format);

void I2cDemo(void)
{
    SIM_MSG_T optionMsg ={0,0,0,NULL};
    UINT32 opt = 0;
    INT8 *note = "\r\nPlease select an option to test from the items listed below.\r\n";
    INT8 *options_list[] = {
        "1. I2C1 Func Open/Close",
        "2. I2C1 Write",
        "3. I2C1 Read",
        "4. I2C1 ReadEx",
        "5. I2C1 WriteEx",

        "6. I2C2 Func Open/Close",
        "7. I2C2 Write",
        "8. I2C2 Read",
        "9. I2C2 ReadEx",
        "10. I2C Config Init",
        "99. back",
    };
    UINT8 regAddr;
    UINT16 regData;
    UINT8 data[2] = {0x01,0x56};
    //static UINT8 i2c1OpenFlag = 0;
    //static UINT8 i2c2OpenFlag = 0;
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
            case SC_I2C1_OPEN:  //Must be opened before using "sAPI_I2CWrite" or "sAPI_I2CRead"
            {
                break;
            }
            case SC_I2C1_WRITE:
            {
                /* *****************only for nau8810 *********************/
                regAddr = 0x41;
                regData = 0x01a5;
                data[0] = ((regAddr<<1) & 0xFE) | ((regData >> 8) & 0x01);
                data[1] = (UINT8)(regData & 0xFF);
                /******************************************************/

                if(SC_I2C_RC_OK == sAPI_I2CWrite(0, NAU8810_SLAVE_ADDR, data[0], &data[1], 1))
                    sAPI_Debug("I2C write reg[0x%x]:0x%x\r\n",regAddr,regData);
                else
                    sAPI_Debug("I2C write error\r\n");

                PrintfResp("\r\noperation successful!\r\n");
                break;
            }

            case SC_I2C1_WRITEEX:
            {
                /* *****************only for nau8810 *********************/
                regAddr = 0x41;
                regData = 0x01a5;
                data[0] = ((regAddr<<1) & 0xFE) | ((regData >> 8) & 0x01);
                data[1] = (UINT8)(regData & 0xFF);
                /******************************************************/

                if(SC_I2C_RC_OK == sAPI_I2CWriteEx(0, NAU8810_SLAVE_ADDR, data[0], &data[1], 1))
                    sAPI_Debug("I2C write reg[0x%x]:0x%x\r\n",regAddr,regData);
                else
                    sAPI_Debug("I2C write error\r\n");

                PrintfResp("\r\noperation successful!\r\n");
                break;
            }

            case SC_I2C1_READ:
            {
                /* *****************only for nau8810 *********************/
                regAddr = 0x41;
                regData = 0x00;
                /******************************************************/

                if(SC_I2C_RC_OK == sAPI_I2CRead(0, NAU8810_SLAVE_ADDR, (regAddr << 1), data, 2))
                {
                    regData = (((UINT16)data[0]) << 8) | data[1];
                    sAPI_Debug("I2C read reg[0x%x]:0x%x\r\n", regAddr, regData);
                }
                else
                    sAPI_Debug("I2C read error\r\n");

                PrintfResp("\r\noperation successful!\r\n");
                break;
            }
            case SC_I2C1_READEX:
            {
                /* *****************only for nau8810 *********************/
                regAddr = 0x41;
                regData = 0x00;
                /******************************************************/

                if(SC_I2C_RC_OK == sAPI_I2CReadEx(0, NAU8810_SLAVE_ADDR, (regAddr << 1), data, 2))
                {
                    regData = (((UINT16)data[0]) << 8) | data[1];
                    sAPI_Debug("I2C read reg[0x%x]:0x%x\r\n", regAddr, regData);
                }
                else
                    sAPI_Debug("I2C read error\r\n");

                PrintfResp("\r\noperation successful!\r\n");
                break;
            }
            case SC_I2C2_OPEN:
            {
                break;
            }
            case SC_I2C2_WRITE:
            {
                /* *****************only for nau8810 *********************/
                regAddr = 0x41;
                regData = 0x01a5;
                data[0] = ((regAddr<<1) & 0xFE) | ((regData >> 8) & 0x01);
                data[1] = (UINT8)(regData & 0xFF);
                /******************************************************/

                if(SC_I2C_RC_OK == sAPI_I2CWrite(1, NAU8810_SLAVE_ADDR, data[0], &data[1], 1))
                    sAPI_Debug("I2C write reg[0x%x]:0x%x\r\n",regAddr,regData);
                else
                    sAPI_Debug("I2C write error\r\n");

                PrintfResp("\r\noperation successful!\r\n");
                break;
            }
            case SC_I2C2_READ:
            {
                /* *****************only for nau8810 *********************/
                regAddr = 0x41;
                regData = 0x00;
                /******************************************************/

                if(SC_I2C_RC_OK == sAPI_I2CRead(1, NAU8810_SLAVE_ADDR, (regAddr << 1), data, 2))
                {
                    regData = (((UINT16)data[0]) << 8) | data[1];
                    sAPI_Debug("I2C read reg[0x%x]:0x%x\r\n", regAddr, regData);
                }
                else
                    sAPI_Debug("I2C read error\r\n");

                PrintfResp("\r\noperation successful!\r\n");
                break;
            }

            case SC_I2C2_READEX:
            {
                /* for special I2C device */
                //sAPI_I2CReadEx
                //PrintfResp("\r\noperation successful!\r\n");
                break;
            }

            case SC_I2C2_INIT:
            {
                /**************** i2c init ****************/
                SC_I2C_DEV i2cDev;
                /*set i2c clock*/
                i2cDev.i2c_clock = SC_I2C_STANDARD_MODE;
                /* set i2c channel */
                i2cDev.i2c_channel = SC_I2C_CHANNEL0;

                if(SC_I2C_RC_OK != sAPI_I2CConfigInit(&i2cDev))
                {
                    PrintfResp("\r\nI2C config init fail!\r\n");
                }
                else
                {
                    PrintfResp("\r\nI2C config init successful!\r\n");
                }
                break;
            }

            case SC_GPIO_BACK:
                return;
        }
    }
}


