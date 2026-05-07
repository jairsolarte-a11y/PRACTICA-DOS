#include "system.h"
#include "adc.h"
#include "i2c_master.h"
#include "ssd1306.h"
#include "mq135.h"

/*
   Canales ADC
*/

#define LM35_CHANNEL        0u
#define MQ135_CHANNEL       1u

/*
   LED de alarma
*/

#define LED_LAT             LATDbits.LATD0
#define LED_TRIS            TRISDbits.TRISD0

/*
   Si MQ135 >= 601, el aire se considera contaminado
   y se enciende el LED.
*/

#define MQ135_LED_ON_VALUE  601u

/*
   Referencia ADC.
   Si el PIC esta alimentado con 5V y ADCON1 usa VDD como referencia,
   este valor debe ser 5000.
*/

#define ADC_VREF_MV         5000UL

static void System_Init(void);
static void Startup_LED_Test(void);

static uint16_t Read_LM35_Temperature_X10(void);
static void Process_Components(uint16_t mq135_adc);
static void Display_Update(uint16_t temperature_x10, uint16_t mq135_adc);

static void UInt16_To_String(uint16_t value, char *buffer);
static void Temperature_To_String(uint16_t temperature_x10, char *buffer);

void main(void)
{
    uint16_t temperature_x10;
    uint16_t mq135_adc;

    System_Init();

    Startup_LED_Test();

    SSD1306_ClearDisplay();

    SSD1306_SetCursor(0, 0);
    SSD1306_WriteString("SISTEMA AMBIENTAL");

    SSD1306_SetCursor(0, 1);
    SSD1306_WriteString("LM35 + MQ135");

    while (1)
    {
        temperature_x10 = Read_LM35_Temperature_X10();

        mq135_adc = ADC_Read(MQ135_CHANNEL);

        Process_Components(mq135_adc);

        Display_Update(temperature_x10, mq135_adc);

        __delay_ms(600);
    }
}

static void System_Init(void)
{
    /*
       Oscilador interno 8 MHz.
    */

    OSCCON = 0x72;

    /*
       Desactivar comparadores.
    */

    CMCON = 0x07;
    CVRCON = 0x00;

    /*
       LED en RD0 / pin fisico 19.
    */

    LED_TRIS = 0;
    LED_LAT = 0;

    /*
       ADC:
       AN0 -> LM35
       AN1 -> MQ135
    */

    ADC_Init();

    /*
       I2C OLED:
       SDA -> RB0 / pin 33
       SCL -> RB1 / pin 34
    */

    I2C_Master_Init(100000UL);

    /*
       OLED SSD1306.
    */

    SSD1306_Init();
}

static void Startup_LED_Test(void)
{
    uint8_t i;

    for (i = 0; i < 3; i++)
    {
        LED_LAT = 1;
        __delay_ms(150);

        LED_LAT = 0;
        __delay_ms(150);
    }
}

static uint16_t Read_LM35_Temperature_X10(void)
{
    uint16_t adc_value;
    uint32_t temperature_x10;

    adc_value = ADC_Read(LM35_CHANNEL);

    /*
       LM35:
       10 mV = 1 grado Celsius.

       Con ADC_VREF_MV = 5000:
       Voltaje_mV = ADC * 5000 / 1023

       Temperatura con un decimal:
       temperature_x10 = temperatura * 10

       Para LM35:
       temperature_x10 = Voltaje_mV
    */

    temperature_x10 = ((uint32_t)adc_value * ADC_VREF_MV) / 1023UL;

    return (uint16_t)temperature_x10;
}

static void Process_Components(uint16_t mq135_adc)
{
    /*
       LED como alarma de aire contaminado.
    */

    if (mq135_adc >= MQ135_LED_ON_VALUE)
    {
        LED_LAT = 1;
    }
    else
    {
        LED_LAT = 0;
    }
}

static void Display_Update(uint16_t temperature_x10, uint16_t mq135_adc)
{
    char temp_text[8];
    char mq_text[6];
    uint8_t mq_level;

    Temperature_To_String(temperature_x10, temp_text);
    UInt16_To_String(mq135_adc, mq_text);

    mq_level = MQ135_Get_Level(mq135_adc);

    SSD1306_ClearLine(3);
    SSD1306_SetCursor(0, 3);
    SSD1306_WriteString("TEMP: ");
    SSD1306_WriteString(temp_text);
    SSD1306_WriteString(" C");

    SSD1306_ClearLine(4);
    SSD1306_SetCursor(0, 4);
    SSD1306_WriteString("MQ135: ");
    SSD1306_WriteString(mq_text);

    SSD1306_ClearLine(5);
    SSD1306_SetCursor(0, 5);
    SSD1306_WriteString(MQ135_Get_Text(mq_level));

    SSD1306_ClearLine(6);
    SSD1306_SetCursor(0, 6);

    if (mq135_adc >= MQ135_LED_ON_VALUE)
    {
        SSD1306_WriteString("LED ALARMA: ON");
    }
    else
    {
        SSD1306_WriteString("LED ALARMA: OFF");
    }
}

static void UInt16_To_String(uint16_t value, char *buffer)
{
    uint8_t i = 0;
    uint8_t j = 0;
    char temp[6];

    if (value == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (value > 0)
    {
        temp[i] = (char)((value % 10u) + '0');
        value = value / 10u;
        i++;
    }

    while (i > 0)
    {
        i--;
        buffer[j] = temp[i];
        j++;
    }

    buffer[j] = '\0';
}

static void Temperature_To_String(uint16_t temperature_x10, char *buffer)
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