#include "humidity_sensor.h"
#include <stdio.h>

/*
   Convierte ADC a porcentaje
*/

uint8_t Humidity_Get_Percentage(uint16_t humidity_adc)
{
    return (uint8_t)(((uint32_t)humidity_adc * 100UL) / 1023UL);
}

/*
   Activa ventilador si humedad >= 60%
*/

uint8_t Humidity_Should_Turn_Fan_On(uint16_t humidity_adc)
{
    uint8_t porcentaje;

    porcentaje = Humidity_Get_Percentage(humidity_adc);

 if (porcentaje >= HUMIDITY_FAN_ON_PERCENT)
    {
        return 1u;
    }
    else
    {
        return 0u;
    }
}

/*
   Texto para OLED/LCD

   Ejemplo:
   HUM 45%
*/

const char* Humidity_Get_Text(uint16_t humidity_adc)
{
    static char text[15];

    uint8_t porcentaje;

    porcentaje = Humidity_Get_Percentage(humidity_adc);

    sprintf(text, "HUM %u%%", porcentaje);

    return text;
}