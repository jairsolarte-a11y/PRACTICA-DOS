#include "mq135.h"

/*
   Escala MQ135 segun valor ADC:

   0-250     Aire limpio
   251-400   Aire aceptable
   401-600   Aire regular
   601-800   Aire contaminado
   801-1023  Aire muy contaminado
*/

uint8_t MQ135_Get_Level(uint16_t adc_value)
{
    if (adc_value <= 250u)
    {
        return MQ135_AIRE_LIMPIO;
    }
    else if (adc_value <= 400u)
    {
        return MQ135_AIRE_ACEPTABLE;
    }
    else if (adc_value <= 600u)
    {
        return MQ135_AIRE_REGULAR;
    }
    else if (adc_value <= 800u)
    {
        return MQ135_AIRE_CONTAMINADO;
    }
    else
    {
        return MQ135_AIRE_MUY_CONTAMINADO;
    }
}

const char* MQ135_Get_Text(uint8_t level)
{
    switch (level)
    {
        case MQ135_AIRE_LIMPIO:
            return "AIRE LIMPIO";

        case MQ135_AIRE_ACEPTABLE:
            return "AIRE ACEPTABLE";

        case MQ135_AIRE_REGULAR:
            return "AIRE REGULAR";

        case MQ135_AIRE_CONTAMINADO:
            return "AIRE CONTAM.";

        case MQ135_AIRE_MUY_CONTAMINADO:
            return "AIRE MUY CONT.";

        default:
            return "AIRE ERROR";
    }
}
