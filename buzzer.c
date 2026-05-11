#include "buzzer.h"

/*
   Buzzer conectado en RD6 / pin fisico 29.
*/

#define BUZZER_LAT      LATDbits.LATD6
#define BUZZER_TRIS     TRISDbits.TRISD6

void Buzzer_Init(void)
{
    BUZZER_TRIS = 0;
    BUZZER_LAT = 0;
}

void Buzzer_Set(uint8_t state)
{
    if (state)
    {
        BUZZER_LAT = 1;
    }
    else
    {
        BUZZER_LAT = 0;
    }
}

uint8_t Buzzer_Should_Be_On(uint16_t mq135_adc)
{
    if (mq135_adc >= MQ135_BUZZER_ON_MIN)
    {
        return 1u;
    }
    else
    {
        return 0u;
    }
}

void Buzzer_Update_By_MQ135(uint16_t mq135_adc)
{
    Buzzer_Set(Buzzer_Should_Be_On(mq135_adc));
}
