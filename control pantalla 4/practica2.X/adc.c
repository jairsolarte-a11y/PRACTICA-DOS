#include "adc.h"

/*
   ADC del PIC18F4550

   AN0 / RA0 / pin 2 -> LM35
   AN1 / RA1 / pin 3 -> MQ135
   AN2 / RA2 / pin 4 -> Sensor de luz HW-486
*/

void ADC_Init(void)
{
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;

    ADCON0 = 0x01;

    /*
       AN0, AN1 y AN2 analogicos.
       Los demas pines quedan digitales.
       VREF+ = VDD = 5V.
       VREF- = GND.
    */

    ADCON1 = 0x0C;

    /*
       Resultado justificado a la derecha.
       Tiempo de adquisicion 12 TAD.
       Reloj ADC Fosc/32.
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