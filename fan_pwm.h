#ifndef FAN_PWM_H
#define FAN_PWM_H

#include "system.h"

/*
   Ventilador 5V con entrada PWM.

   Conexion:
   PWM ventilador -> RC2 / CCP1 / pin fisico 17
   VCC ventilador -> 5V
   GND ventilador -> GND comun

   Control por MQ135:

   0-400     -> apagado
   401-600   -> velocidad 1
   601-800   -> velocidad 2
   801-1023  -> velocidad 3
*/

#define FAN_SPEED_OFF      0u
#define FAN_SPEED_1        1u
#define FAN_SPEED_2        2u
#define FAN_SPEED_3        3u

void Fan_PWM_Init(void);
void Fan_Set_Duty(uint8_t duty_percent);
void Fan_Set_Speed(uint8_t speed);
uint8_t Fan_Get_Speed_By_MQ135(uint16_t mq135_adc);
void Fan_Update_By_MQ135(uint16_t mq135_adc);

#endif