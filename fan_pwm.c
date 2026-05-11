#include "fan_pwm.h"

/*
   PWM usando CCP1 en RC2.

   Fosc = 8 MHz
   PR2 = 79
   Prescaler Timer2 = 1

   Frecuencia PWM:
   Fpwm = Fosc / (4 * (PR2 + 1) * prescaler)
   Fpwm = 8000000 / (4 * 80 * 1)
   Fpwm = 25000 Hz = 25 kHz

   Esta frecuencia es adecuada para ventiladores con entrada PWM.
*/

#define PWM_PR2_VALUE              79u

/*
   Porcentajes de trabajo para las 3 velocidades.

   Si la velocidad 1 no alcanza a mover el ventilador,
   sube FAN_DUTY_SPEED_1_PERCENT a 45u o 50u.
*/

#define FAN_DUTY_SPEED_1_PERCENT   40u
#define FAN_DUTY_SPEED_2_PERCENT   70u
#define FAN_DUTY_SPEED_3_PERCENT   100u

static uint8_t fan_last_speed = FAN_SPEED_OFF;

void Fan_PWM_Init(void)
{
    /*
       RC2 / CCP1 como salida PWM.
    */

    TRISCbits.TRISC2 = 0;
    LATCbits.LATC2 = 0;

    /*
       Periodo PWM.
    */

    PR2 = PWM_PR2_VALUE;

    /*
       Duty inicial en 0%.
    */

    CCPR1L = 0x00;

    /*
       CCP1 en modo PWM.
       CCP1CON<3:0> = 1100
    */

    CCP1CON = 0b00001100;

    /*
       Timer2 encendido.
       Prescaler 1:1.
    */

    T2CON = 0b00000100;

    Fan_Set_Duty(0u);
}

void Fan_Set_Duty(uint8_t duty_percent)
{
    uint16_t duty_counts;
    uint16_t max_counts;

    if (duty_percent > 100u)
    {
        duty_percent = 100u;
    }

    /*
       En PWM del PIC:
       duty_counts va de 0 hasta 4 * (PR2 + 1).
    */

    max_counts = 4u * ((uint16_t)PWM_PR2_VALUE + 1u);

    duty_counts = (uint16_t)(((uint32_t)duty_percent * max_counts) / 100u);

    /*
       Para 100%, se deja en el maximo permitido menos 1.
    */

    if (duty_counts >= max_counts)
    {
        duty_counts = max_counts - 1u;
    }

    /*
       Bits altos del duty.
    */

    CCPR1L = (uint8_t)(duty_counts >> 2);

    /*
       Bits bajos del duty en CCP1CON<5:4>.
    */

    CCP1CON = (uint8_t)((CCP1CON & 0b11001111) |
                        ((duty_counts & 0x03u) << 4));
}

void Fan_Set_Speed(uint8_t speed)
{
    if (speed == fan_last_speed)
    {
        return;
    }

    switch (speed)
    {
        case FAN_SPEED_OFF:
            Fan_Set_Duty(0u);
            break;

        case FAN_SPEED_1:
            /*
               Impulso de arranque.
               Algunos ventiladores no arrancan directamente con bajo duty.
            */

            if (fan_last_speed == FAN_SPEED_OFF)
            {
                Fan_Set_Duty(FAN_DUTY_SPEED_3_PERCENT);
                __delay_ms(400);
            }

            Fan_Set_Duty(FAN_DUTY_SPEED_1_PERCENT);
            break;

        case FAN_SPEED_2:
            Fan_Set_Duty(FAN_DUTY_SPEED_2_PERCENT);
            break;

        case FAN_SPEED_3:
            Fan_Set_Duty(FAN_DUTY_SPEED_3_PERCENT);
            break;

        default:
            Fan_Set_Duty(0u);
            speed = FAN_SPEED_OFF;
            break;
    }

    fan_last_speed = speed;
}

uint8_t Fan_Get_Speed_By_MQ135(uint16_t mq135_adc)
{
    if (mq135_adc >= 801u)
    {
        return FAN_SPEED_3;
    }
    else if (mq135_adc >= 601u)
    {
        return FAN_SPEED_2;
    }
    else if (mq135_adc >= 401u)
    {
        return FAN_SPEED_1;
    }
    else
    {
        return FAN_SPEED_OFF;
    }
}

void Fan_Update_By_MQ135(uint16_t mq135_adc)
{
    uint8_t speed;

    speed = Fan_Get_Speed_By_MQ135(mq135_adc);

    Fan_Set_Speed(speed);
}
