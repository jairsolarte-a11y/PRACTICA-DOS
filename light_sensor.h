#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "system.h"

/*
   Sensor de iluminacion HW-486 con LDR.

   Conexion:
   S  -> RA2 / AN2 / pin fisico 4 del PIC18F4550
   +  -> 5V
   -  -> GND

   Este modulo normalmente entrega:
   - ADC bajo  con mucha luz
   - ADC alto  con oscuridad

   Por eso se usa LIGHT_SENSOR_INVERTED = 1.
*/

#define LIGHT_SENSOR_INVERTED   1u

#define LIGHT_MUY_OSCURO        0u
#define LIGHT_POCA_LUZ          1u
#define LIGHT_LUZ_MEDIA         2u
#define LIGHT_MUCHA_LUZ         3u
#define LIGHT_LUZ_INTENSA       4u

uint16_t Light_Process_ADC(uint16_t raw_adc);
uint8_t Light_Get_Level(uint16_t light_value);
uint8_t Light_Get_Leds_Count(uint8_t level);
const char* Light_Get_Text(uint8_t level);

#endif