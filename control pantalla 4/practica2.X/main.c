#include "system.h"
#include "adc.h"
#include "i2c_master.h"
#include "ssd1306.h"

#define LM35_CHANNEL        0

#define LED_LAT             LATDbits.LATD0
#define LED_TRIS            TRISDbits.TRISD0

/*
   Umbral opcional del LED.

   300 equivale a 30.0 grados Celsius.
   250 equivale a 25.0 grados Celsius.
   350 equivale a 35.0 grados Celsius.
*/

#define TEMP_LED_ON_X10     300u

static void System_Init(void);
static void Startup_LED_Test(void);

static uint16_t Read_LM35_Temperature_X10(void);
static void Process_Components(uint16_t temperature_x10);
static void Display_Update(uint16_t adc_value, uint16_t temperature_x10);

static void UInt16_To_String(uint16_t value, char *buffer);
static void Temperature_To_String(uint16_t temperature_x10, char *buffer);

void main(void)
{
    uint16_t adc_value;
    uint16_t temperature_x10;

    System_Init();

    Startup_LED_Test();

    SSD1306_ClearDisplay();

    SSD1306_SetCursor(0, 0);
    SSD1306_WriteString("SENSOR LM35");

    SSD1306_SetCursor(0, 1);
    SSD1306_WriteString("PIC18F4550");

    SSD1306_SetCursor(0, 6);
    SSD1306_WriteString("Temp real en C");

    while (1)
    {
        /*
           Leer ADC directamente para mostrarlo.
        */

        adc_value = ADC_Read(LM35_CHANNEL);

        /*
           Convertir lectura ADC a temperatura con un decimal.
        */

        temperature_x10 = Read_LM35_Temperature_X10();

        /*
           Control opcional de componentes.
           En este caso, LED segun temperatura.
        */

        Process_Components(temperature_x10);

        /*
           Mostrar datos en pantalla OLED.
        */

        Display_Update(adc_value, temperature_x10);

        __delay_ms(500);
    }
}

static void System_Init(void)
{
    /*
       Oscilador interno a 8 MHz.
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
       Inicializar ADC.
       LM35 conectado en RA0 / AN0 / pin fisico 2.
    */

    ADC_Init();

    /*
       Inicializar I2C.
       OLED SSD1306:

       SDA -> RB0 / pin fisico 33
       SCL -> RB1 / pin fisico 34

       Tu archivo i2c_master.c debe conservar SSPADD = 119u.
    */

    I2C_Master_Init(100000UL);

    /*
       Inicializar OLED.
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

    /*
       Leer el LM35 en AN0.
    */

    adc_value = ADC_Read(LM35_CHANNEL);

    /*
       Conversion para LM35 con Vref = 5V:

       ADC de 10 bits:
       0 a 1023 representa 0V a 5V.

       Voltaje en mV:
       V_mV = ADC * 5000 / 1023

       LM35:
       10 mV = 1 grado Celsius

       Temperatura en grados:
       Temp_C = V_mV / 10

       Temperatura con un decimal:
       Temp_x10 = Temp_C * 10

       Como Temp_x10 = V_mV:
       Temp_x10 = ADC * 5000 / 1023
    */

    temperature_x10 = ((uint32_t)adc_value * 5000UL) / 1023UL;

    return (uint16_t)temperature_x10;
}

static void Process_Components(uint16_t temperature_x10)
{
    /*
       Funcion preparada para agregar mas componentes despues.

       En esta version:
       LED ON si temperatura >= 30.0 C
       LED OFF si temperatura < 30.0 C
    */

    if (temperature_x10 >= TEMP_LED_ON_X10)
    {
        LED_LAT = 1;
    }
    else
    {
        LED_LAT = 0;
    }
}

static void Display_Update(uint16_t adc_value, uint16_t temperature_x10)
{
    char adc_text[6];
    char temp_text[8];

    UInt16_To_String(adc_value, adc_text);
    Temperature_To_String(temperature_x10, temp_text);

    SSD1306_ClearLine(3);
    SSD1306_SetCursor(0, 3);
    SSD1306_WriteString("ADC: ");
    SSD1306_WriteString(adc_text);

    SSD1306_ClearLine(4);
    SSD1306_SetCursor(0, 4);
    SSD1306_WriteString("TEMP: ");
    SSD1306_WriteString(temp_text);
    SSD1306_WriteString(" C");

    SSD1306_ClearLine(5);
    SSD1306_SetCursor(0, 5);

    if (temperature_x10 >= TEMP_LED_ON_X10)
    {
        SSD1306_WriteString("LED: ON ");
    }
    else
    {
        SSD1306_WriteString("LED: OFF");
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
        temp[i] = (char)((value % 10) + '0');
        value = value / 10;
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

    integer_part = temperature_x10 / 10;
    decimal_part = temperature_x10 % 10;

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