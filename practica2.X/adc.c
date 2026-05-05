#include "adc.h"

/*
   ADC del PIC18F4550.
   Para esta practica se usa AN0 / RA0 / pin 2.
*/

void ADC_Init(void)
{
    TRISAbits.TRISA0 = 1;   // RA0 / AN0 como entrada

    /*
       ADCON0:
       ADON = 1
       Canal inicial = AN0
    */

    ADCON0 = 0x01;

    /*
       ADCON1:
       PCFG = 1110
       AN0 analogico.
       Los demas pines analogicos quedan digitales.
       Vref+ = VDD
       Vref- = VSS
    */

    ADCON1 = 0x0E;

    /*
       ADCON2:
       ADFM = 1, resultado justificado a la derecha
       ACQT = 101, tiempo de adquisicion 12 TAD
       ADCS = 010, reloj ADC Fosc/32
    */

    ADCON2 = 0xAA;
}

uint16_t ADC_Read(uint8_t channel)
{
    if (channel > 13)
    {
        channel = 0;
    }

    ADCON0 &= 0xC3;
    ADCON0 |= (uint8_t)(channel << 2);

    __delay_us(25);

    ADCON0bits.GO_DONE = 1;

    while (ADCON0bits.GO_DONE);

    return (((uint16_t)ADRESH << 8) | ADRESL);
}
