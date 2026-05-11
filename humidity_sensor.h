#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H

#include "system.h"

/*
   Sensor de humedad analogico.

   Conexion:
   AO/S -> RA3 / AN3 / pin fisico 5

   Escala:
   0-500     -> ventilador apagado por humedad
   501-1023  -> ventilador encendido por humedad
*/

#define HUMIDITY_FAN_ON_MIN     501u

uint8_t Humidity_Should_Turn_Fan_On(uint16_t humidity_adc);
const char* Humidity_Get_Text(uint16_t humidity_adc);

#endif