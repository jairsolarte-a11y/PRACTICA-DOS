#ifndef BUZZER_H
#define BUZZER_H

#include "system.h"

/*
   Buzzer para alarma del sensor MQ-135.

   Conexion:
   RD6 / pin fisico 29 -> entrada del buzzer o transistor

   Condicion:
   Buzzer ON si MQ135 esta entre 801 y 1023.
*/

#define MQ135_BUZZER_ON_MIN     801u

void Buzzer_Init(void);
void Buzzer_Set(uint8_t state);
uint8_t Buzzer_Should_Be_On(uint16_t mq135_adc);
void Buzzer_Update_By_MQ135(uint16_t mq135_adc);

#endif