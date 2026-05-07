#ifndef MQ135_H
#define MQ135_H

#include "system.h"

#define MQ135_AIRE_LIMPIO          0u
#define MQ135_AIRE_ACEPTABLE       1u
#define MQ135_AIRE_REGULAR         2u
#define MQ135_AIRE_CONTAMINADO     3u
#define MQ135_AIRE_MUY_CONTAMINADO 4u

uint8_t MQ135_Get_Level(uint16_t adc_value);
const char* MQ135_Get_Text(uint8_t level);

#endif