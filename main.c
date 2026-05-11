#include "system.h"
#include "adc.h"
#include "i2c_master.h"
#include "ssd1306.h"
#include "mq135.h"
#include "light_sensor.h"
#include "buzzer.h"
#include "fan_pwm.h"
#include "humidity_sensor.h"

#include <stdio.h>

/*
   Canales ADC usados:

   AN0 / RA0 / pin 2 -> LM35
   AN1 / RA1 / pin 3 -> MQ135
   AN2 / RA2 / pin 4 -> Sensor de luz HW-486
   AN3 / RA3 / pin 5 -> Sensor de humedad analogico
*/

#define LM35_CHANNEL        0u
#define MQ135_CHANNEL       1u
#define LIGHT_CHANNEL       2u
#define HUMIDITY_CHANNEL    3u

/*
   Desplazamiento horizontal OLED.
*/

#define OLED_X_OFFSET       8u

/*
   LEDs de intensidad de luz.
*/

#define LED1_LAT            LATDbits.LATD0
#define LED2_LAT            LATDbits.LATD1
#define LED3_LAT            LATDbits.LATD2
#define LED4_LAT            LATDbits.LATD3
#define LED5_LAT            LATDbits.LATD4

#define LED1_TRIS           TRISDbits.TRISD0
#define LED2_TRIS           TRISDbits.TRISD1
#define LED3_TRIS           TRISDbits.TRISD2
#define LED4_TRIS           TRISDbits.TRISD3
#define LED5_TRIS           TRISDbits.TRISD4

/*
   LED temperatura.
*/

#define TEMP_LED_LAT        LATDbits.LATD5
#define TEMP_LED_TRIS       TRISDbits.TRISD5

#define TEMP_LED_MAX_X10    380u

/*
   Referencia ADC.
*/

#define ADC_VREF_MV         5000UL

/*
   Prototipos.
*/

static void System_Init(void);
static void Startup_Test(void);

static uint16_t Read_LM35_Temperature_X10(void);

static void Light_LEDs_Update(uint8_t leds_count);
static void Temperature_LED_Update(uint16_t temperature_x10);

static uint8_t Fan_Get_Speed_By_MQ135_And_Humidity(uint16_t mq135_adc,
                                                   uint16_t humidity_adc);

static uint8_t Process_Components(uint16_t light_adc,
                                  uint16_t temperature_x10,
                                  uint16_t mq135_adc,
                                  uint16_t humidity_adc);

static void Display_Update(uint16_t temperature_x10,
                           uint16_t mq135_adc,
                           uint16_t light_adc,
                           uint16_t humidity_adc,
                           uint8_t fan_speed);

static void UInt16_To_String(uint16_t value, char *buffer);
static void Temperature_To_String(uint16_t temperature_x10, char *buffer);

/*========================================================*/

void main(void)
{
    uint16_t temperature_x10;
    uint16_t mq135_adc;
    uint16_t light_adc_raw;
    uint16_t light_adc;
    uint16_t humidity_adc;
    uint8_t fan_speed;

    System_Init();

    Startup_Test();

    SSD1306_ClearDisplay();

    SSD1306_SetCursor(OLED_X_OFFSET, 0);
    SSD1306_WriteString("SISTEMA AMBIENTAL");

    SSD1306_SetCursor(OLED_X_OFFSET, 1);
    SSD1306_WriteString("LM35 MQ LUZ HUM");

    while (1)
    {
        /*
           LM35.
        */

        temperature_x10 = Read_LM35_Temperature_X10();

        /*
           MQ135.
        */

        mq135_adc = ADC_Read(MQ135_CHANNEL);

        /*
           Sensor de luz.
        */

        light_adc_raw = ADC_Read(LIGHT_CHANNEL);

        light_adc = Light_Process_ADC(light_adc_raw);

        /*
           Sensor humedad.
        */

        humidity_adc = ADC_Read(HUMIDITY_CHANNEL);

        /*
           Actualizar componentes.
        */

        fan_speed = Process_Components(light_adc,
                                       temperature_x10,
                                       mq135_adc,
                                       humidity_adc);

        /*
           Actualizar OLED.
        */

        Display_Update(temperature_x10,
                       mq135_adc,
                       light_adc,
                       humidity_adc,
                       fan_speed);

        __delay_ms(600);
    }
}

/*========================================================*/

static void System_Init(void)
{
    OSCCON = 0x72;

    CMCON = 0x07;

    CVRCON = 0x00;

    /*
       LEDs luz.
    */

    LED1_TRIS = 0;
    LED2_TRIS = 0;
    LED3_TRIS = 0;
    LED4_TRIS = 0;
    LED5_TRIS = 0;

    LED1_LAT = 0;
    LED2_LAT = 0;
    LED3_LAT = 0;
    LED4_LAT = 0;
    LED5_LAT = 0;

    /*
       LED temperatura.
    */

    TEMP_LED_TRIS = 0;

    TEMP_LED_LAT = 0;

    /*
       Buzzer.
    */

    Buzzer_Init();

    /*
       ADC.
    */

    ADC_Init();

    /*
       I2C OLED.
    */

    I2C_Master_Init(100000UL);

    SSD1306_Init();

    /*
       PWM ventilador.
    */

    Fan_PWM_Init();
}

/*========================================================*/

static void Startup_Test(void)
{
    LED1_LAT = 1;
    __delay_ms(100);

    LED2_LAT = 1;
    __delay_ms(100);

    LED3_LAT = 1;
    __delay_ms(100);

    LED4_LAT = 1;
    __delay_ms(100);

    LED5_LAT = 1;
    __delay_ms(100);

    TEMP_LED_LAT = 1;
    __delay_ms(200);

    Buzzer_Set(1u);
    __delay_ms(200);

    Buzzer_Set(0u);

    Fan_Set_Speed(FAN_SPEED_3);

    __delay_ms(700);

    Fan_Set_Speed(FAN_SPEED_OFF);

    LED1_LAT = 0;
    LED2_LAT = 0;
    LED3_LAT = 0;
    LED4_LAT = 0;
    LED5_LAT = 0;

    TEMP_LED_LAT = 0;

    __delay_ms(200);
}

/*========================================================*/

static uint16_t Read_LM35_Temperature_X10(void)
{
    uint16_t adc_value;

    uint32_t temperature_x10;

    adc_value = ADC_Read(LM35_CHANNEL);

    temperature_x10 =
        ((uint32_t)adc_value * ADC_VREF_MV) / 1023UL;

    return (uint16_t)temperature_x10;
}

/*========================================================*/

static void Light_LEDs_Update(uint8_t leds_count)
{
    LED1_LAT = 0;
    LED2_LAT = 0;
    LED3_LAT = 0;
    LED4_LAT = 0;
    LED5_LAT = 0;

    if (leds_count >= 1u)
    {
        LED1_LAT = 1;
    }

    if (leds_count >= 2u)
    {
        LED2_LAT = 1;
    }

    if (leds_count >= 3u)
    {
        LED3_LAT = 1;
    }

    if (leds_count >= 4u)
    {
        LED4_LAT = 1;
    }

    if (leds_count >= 5u)
    {
        LED5_LAT = 1;
    }
}

/*========================================================*/

static void Temperature_LED_Update(uint16_t temperature_x10)
{
    if (temperature_x10 <= TEMP_LED_MAX_X10)
    {
        TEMP_LED_LAT = 1;
    }
    else
    {
        TEMP_LED_LAT = 0;
    }
}

/*========================================================*/

static uint8_t Fan_Get_Speed_By_MQ135_And_Humidity(uint16_t mq135_adc,
                                                   uint16_t humidity_adc)
{
    uint8_t fan_speed;

    fan_speed = Fan_Get_Speed_By_MQ135(mq135_adc);

    if (Humidity_Should_Turn_Fan_On(humidity_adc))
    {
        if (fan_speed == FAN_SPEED_OFF)
        {
            fan_speed = FAN_SPEED_1;
        }
    }

    return fan_speed;
}

/*========================================================*/

static uint8_t Process_Components(uint16_t light_adc,
                                  uint16_t temperature_x10,
                                  uint16_t mq135_adc,
                                  uint16_t humidity_adc)
{
    uint8_t light_level;

    uint8_t leds_count;

    uint8_t fan_speed;

    light_level = Light_Get_Level(light_adc);

    leds_count = Light_Get_Leds_Count(light_level);

    Light_LEDs_Update(leds_count);

    Temperature_LED_Update(temperature_x10);

    Buzzer_Update_By_MQ135(mq135_adc);

    fan_speed =
        Fan_Get_Speed_By_MQ135_And_Humidity(mq135_adc,
                                            humidity_adc);

    Fan_Set_Speed(fan_speed);

    return fan_speed;
}

/*========================================================*/

static void Display_Update(uint16_t temperature_x10,
                           uint16_t mq135_adc,
                           uint16_t light_adc,
                           uint16_t humidity_adc,
                           uint8_t fan_speed)
{
    char temp_text[8];

    char mq_text[6];

    char light_text[6];

    char humidity_text[15];

    uint8_t mq_level;

    uint8_t light_level;

    uint8_t leds_count;

    Temperature_To_String(temperature_x10, temp_text);

    UInt16_To_String(mq135_adc, mq_text);

    UInt16_To_String(light_adc, light_text);

    sprintf(humidity_text,
            "%s",
            Humidity_Get_Text(humidity_adc));

    mq_level = MQ135_Get_Level(mq135_adc);

    light_level = Light_Get_Level(light_adc);

    leds_count = Light_Get_Leds_Count(light_level);

    /*
       Linea 2.
    */

    SSD1306_ClearLine(2);

    SSD1306_SetCursor(OLED_X_OFFSET, 2);

    SSD1306_WriteString("T:");

    SSD1306_WriteString(temp_text);

    SSD1306_WriteString("C ");

    SSD1306_WriteString(humidity_text);

    /*
       Linea 3.
    */

    SSD1306_ClearLine(3);

    SSD1306_SetCursor(OLED_X_OFFSET, 3);

    SSD1306_WriteString("MQ:");

    SSD1306_WriteString(mq_text);

    SSD1306_WriteString(" ");

    SSD1306_WriteString(MQ135_Get_Text(mq_level));

    /*
       Linea 4.
    */

    SSD1306_ClearLine(4);

    SSD1306_SetCursor(OLED_X_OFFSET, 4);

    SSD1306_WriteString("LUZ:");

    SSD1306_WriteString(light_text);

    /*
       Linea 5.
    */

    SSD1306_ClearLine(5);

    SSD1306_SetCursor(OLED_X_OFFSET, 5);

    SSD1306_WriteString(Light_Get_Text(light_level));

    /*
       Linea 6.
    */

    SSD1306_ClearLine(6);

    SSD1306_SetCursor(OLED_X_OFFSET, 6);

    SSD1306_WriteString("L:");

    SSD1306_WriteChar((char)(leds_count + '0'));

    SSD1306_WriteString("/5 ");

    if (temperature_x10 <= TEMP_LED_MAX_X10)
    {
        SSD1306_WriteString("TLED:ON");
    }
    else
    {
        SSD1306_WriteString("TLED:OFF");
    }

    /*
       Linea 7.
    */

    SSD1306_ClearLine(7);

    SSD1306_SetCursor(OLED_X_OFFSET, 7);

    if (Buzzer_Should_Be_On(mq135_adc))
    {
        SSD1306_WriteString("BZ:ON ");
    }
    else
    {
        SSD1306_WriteString("BZ:OFF ");
    }

    if (fan_speed == FAN_SPEED_OFF)
    {
        SSD1306_WriteString("FAN:OFF");
    }
    else if (fan_speed == FAN_SPEED_1)
    {
        SSD1306_WriteString("FAN:V1");
    }
    else if (fan_speed == FAN_SPEED_2)
    {
        SSD1306_WriteString("FAN:V2");
    }
    else
    {
        SSD1306_WriteString("FAN:V3");
    }
}

/*========================================================*/

static void UInt16_To_String(uint16_t value, char *buffer)
{
    uint8_t i = 0;

    uint8_t j = 0;

    char temp[6];

    if (value == 0u)
    {
        buffer[0] = '0';

        buffer[1] = '\0';

        return;
    }

    while (value > 0u)
    {
        temp[i] = (char)((value % 10u) + '0');

        value = value / 10u;

        i++;
    }

    while (i > 0u)
    {
        i--;

        buffer[j] = temp[i];

        j++;
    }

    buffer[j] = '\0';
}

/*========================================================*/

static void Temperature_To_String(uint16_t temperature_x10,
                                  char *buffer)
{
    uint16_t integer_part;

    uint16_t decimal_part;

    char integer_text[6];

    uint8_t i = 0;

    integer_part = temperature_x10 / 10u;

    decimal_part = temperature_x10 % 10u;

    UInt16_To_String(integer_part, integer_text);

    while (integer_text[i] != '\0')
    {
        buffer[i] = integer_text[i];

        i++;
    }

    buffer[i] = '.';

    i++;

    buffer[i] = (char)(decimal_part + '0');

    i++;

    buffer[i] = '\0';
}  