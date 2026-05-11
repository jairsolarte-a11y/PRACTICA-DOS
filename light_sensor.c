#include "light_sensor.h"

/*
   Convierte el valor real del ADC al sentido que necesita la practica.

   El sensor HW-486 puede entregar:
   - Mucha luz  -> valor bajo
   - Oscuridad  -> valor alto

   Pero la practica necesita:
   - Muy oscuro -> valor bajo
   - Luz intensa -> valor alto

   Por eso se invierte:
   light_value = 1023 - raw_adc
*/

uint16_t Light_Process_ADC(uint16_t raw_adc)
{
    uint16_t light_value;

#if LIGHT_SENSOR_INVERTED == 1u
    if (raw_adc > 1023u)
    {
        raw_adc = 1023u;
    }

    light_value = 1023u - raw_adc;
#else
    light_value = raw_adc;
#endif

    return light_value;
}

/*
   Escala corregida:

   0-200     Muy oscuro       -> 1 LED
   201-400   Poca luz         -> 2 LEDs
   401-700   Luz media        -> 3 LEDs
   701-900   Mucha luz        -> 4 LEDs
   901-1023  Luz intensa      -> 5 LEDs
*/

uint8_t Light_Get_Level(uint16_t light_value)
{
    if (light_value <= 200u)
    {
        return LIGHT_MUY_OSCURO;
    }
    else if (light_value <= 400u)
    {
        return LIGHT_POCA_LUZ;
    }
    else if (light_value <= 700u)
    {
        return LIGHT_LUZ_MEDIA;
    }
    else if (light_value <= 900u)
    {
        return LIGHT_MUCHA_LUZ;
    }
    else
    {
        return LIGHT_LUZ_INTENSA;
    }
}

uint8_t Light_Get_Leds_Count(uint8_t level)
{
    switch (level)
    {
        case LIGHT_MUY_OSCURO:
            return 1u;

        case LIGHT_POCA_LUZ:
            return 2u;

        case LIGHT_LUZ_MEDIA:
            return 3u;

        case LIGHT_MUCHA_LUZ:
            return 4u;

        case LIGHT_LUZ_INTENSA:
            return 5u;

        default:
            return 0u;
    }
}

const char* Light_Get_Text(uint8_t level)
{
    switch (level)
    {
        case LIGHT_MUY_OSCURO:
            return "MUY OSCURO";

        case LIGHT_POCA_LUZ:
            return "POCA LUZ";

        case LIGHT_LUZ_MEDIA:
            return "LUZ MEDIA";

        case LIGHT_MUCHA_LUZ:
            return "MUCHA LUZ";

        case LIGHT_LUZ_INTENSA:
            return "LUZ INTENSA";

        default:
            return "LUZ ERROR";
    }
}