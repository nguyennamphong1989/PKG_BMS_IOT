#ifndef __SIMCOM_SPI_H__
#define __SIMCOM_SPI_H__


typedef enum
{
    SC_SPI_RC_OK=0,
    SC_SPI_RC_ERROR=-1,
}SC_SPI_ReturnCode;

typedef struct
{
    unsigned char index;   //ssp index
    int clock;  //spi clock
    int mode;   //spi mode
    int csMode; //when the value is 0,the cs config to gpio
}SC_SPI_DEV;

typedef enum
{
    SPI_CLOCK_6MHz = 0,
    SPI_CLOCK_13MHz,
    SPI_CLOCK_26MHz,
    SPI_CLOCK_52MHz,
    SPI_CLOCK_3250KHz,
    SPI_CLOCK_1625KHz,
    SPI_CLOCK_812KHz,
}SC_SPI_CLOCK;

typedef enum
{
    SPI_MODE_PH0_PO0 = 0,
    SPI_MODE_PH0_PO1,
    SPI_MODE_PH1_PO0,
    SPI_MODE_PH1_PO1,
}SC_SPI_MODE;

typedef enum
{
    GPIO_MODE = 0,
    SSP_MODE,
    CUS_CS_MODE,
}CS_MODE;

#endif

