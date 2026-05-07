#include "adc.h"

/*
   ADC del PIC18F4550.

   Para esta practica:
   LM35 OUT -> RA0 / AN0 / pin fisico 2

   LM35:
   10 mV por cada grado Celsius.
*/

void ADC_Init(void)
{
    /*
       RA0 / AN0 como entrada analogica.
    */

    TRISAbits.TRISA0 = 1;

    /*
       ADCON0:
       ADC encendido.
       Canal inicial AN0.
    */

    ADCON0 = 0x01;

    /*
       ADCON1:
       AN0 analogico.
       Los demas pines analogicos quedan digitales.
       Vref+ = VDD
       Vref- = VSS
    */

    ADCON1 = 0x0E;

    /*
       ADCON2:
       ADFM = 1  -> resultado justificado a la derecha
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

    /*
       Seleccionar canal ADC.
    */

    ADCON0 &= 0xC3;
    ADCON0 |= (uint8_t)(channel << 2);

    /*
       Tiempo de adquisicion.
    */

    __delay_us(30);

    /*
       Iniciar conversion.
    */

    ADCON0bits.GO_DONE = 1;

    /*
       Esperar fin de conversion.
    */

    while (ADCON0bits.GO_DONE);

    /*
       Retornar resultado de 10 bits.
    */

    return (((uint16_t)ADRESH << 8) | ADRESL);
}