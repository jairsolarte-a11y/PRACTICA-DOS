#include "adc.h"

/*
   ADC del PIC18F4550

   AN0 / RA0 / pin 2 -> LM35
   AN1 / RA1 / pin 3 -> MQ135
*/

void ADC_Init(void)
{
    TRISAbits.TRISA0 = 1;   // AN0 entrada analogica
    TRISAbits.TRISA1 = 1;   // AN1 entrada analogica

    /*
       ADCON0:
       ADC encendido.
       Canal inicial AN0.
    */

    ADCON0 = 0x01;

    /*
       ADCON1 = 0x0D

       AN0 y AN1 analogicos.
       Los demas pines quedan digitales.
       VREF+ = VDD del PIC, normalmente 5V.
       VREF- = VSS.
    */

    ADCON1 = 0x0D;

    /*
       ADCON2:
       ADFM = 1   -> justificado a la derecha
       ACQT = 101 -> 12 TAD
       ADCS = 010 -> Fosc/32
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

    __delay_us(30);

    ADCON0bits.GO_DONE = 1;

    while (ADCON0bits.GO_DONE);

    return (((uint16_t)ADRESH << 8) | ADRESL);
}