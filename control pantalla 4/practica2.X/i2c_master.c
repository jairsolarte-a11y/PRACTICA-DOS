#include "i2c_master.h"

/*
   I2C hardware MSSP del PIC18F4550:

   SDA -> RB0 / pin 33
   SCL -> RB1 / pin 34
*/

static void I2C_Master_Wait(void)
{
    while ((SSPCON2 & 0x1F) || SSPSTATbits.R_nW);
}

void I2C_Master_Init(uint32_t clock_hz)
{
    TRISBbits.TRISB0 = 1;   // SDA como entrada para el modulo MSSP
    TRISBbits.TRISB1 = 1;   // SCL como entrada para el modulo MSSP

    SSPSTAT = 0x80;         // Slew rate deshabilitado para 100 kHz
    SSPCON1 = 0b00101000;         // MSSP habilitado, modo I2C Master  
    SSPCON2 = 0x00;

    /*
       Formula:
       SCL = Fosc / (4 * (SSPADD + 1))

       Para Fosc = 8 MHz y SCL = 100 kHz:
       SSPADD = 19
    */

    SSPADD = 119u;

    PIR1bits.SSPIF = 0;
}

void I2C_Master_Start(void)
{
    I2C_Master_Wait();
    SSPCON2bits.SEN = 1;
    while (SSPCON2bits.SEN);
}

void I2C_Master_Stop(void)
{
    I2C_Master_Wait();
    SSPCON2bits.PEN = 1;
    while (SSPCON2bits.PEN);
}

uint8_t I2C_Master_Write(uint8_t data)
{
    I2C_Master_Wait();

    PIR1bits.SSPIF = 0;
    SSPBUF = data;

    while (!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;

    /*
       ACKSTAT = 0 -> el esclavo respondio correctamente
       ACKSTAT = 1 -> no hubo respuesta
    */

    return SSPCON2bits.ACKSTAT;
}