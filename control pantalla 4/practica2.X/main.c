#include "system.h"
#include "adc.h"
#include "i2c_master.h"
#include "ssd1306.h"

#define POT_CHANNEL     0

#define LED_LAT         LATDbits.LATD0
#define LED_TRIS        TRISDbits.TRISD0

#define LED_ADC_MIN     600u
#define LED_ADC_MAX     1023u

static void System_Init(void);
static uint8_t Process_Components(uint16_t adc_value);
static void Display_Update(uint16_t adc_value, uint8_t led_state);
static void UInt16_To_String(uint16_t value, char *buffer);

void main(void)
{
    uint16_t adc_value;
    uint8_t led_state;

    System_Init();

    SSD1306_ClearDisplay();

    SSD1306_SetCursor(0, 0);
    SSD1306_WriteString("PRACTICA ADC");

    SSD1306_SetCursor(0, 1);
    SSD1306_WriteString("PIC18F4550");

    SSD1306_SetCursor(0, 6);
    SSD1306_WriteString("LED ON: 600-1023");

    while (1)
    {
        adc_value = ADC_Read(POT_CHANNEL);

        led_state = Process_Components(adc_value);

        Display_Update(adc_value, led_state);

        __delay_ms(200);
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
       LED conectado en RD0 / pin fisico 19.
    */

    LED_TRIS = 0;
    LED_LAT = 0;

    /*
       Inicializacion del ADC.
       Potenciometro en RA0 / AN0 / pin fisico 2.
    */

    ADC_Init();

    /*
       Inicializacion I2C.
       OLED SSD1306:

       SDA -> RB0 / pin fisico 33
       SCL -> RB1 / pin fisico 34
    */

    I2C_Master_Init(100000UL);

    /*
       Inicializacion de pantalla OLED.
    */

    SSD1306_Init();
}

static uint8_t Process_Components(uint16_t adc_value)
{
    uint8_t led_state;

    /*
       Esta funcion permite agregar despues mas componentes
       que dependan del valor del sensor.

       En esta primera practica:
       LED encendido si ADC esta entre 600 y 1023.
       LED apagado si ADC esta por debajo de 600.
    */

    if ((adc_value >= LED_ADC_MIN) && (adc_value <= LED_ADC_MAX))
    {
        LED_LAT = 1;
        led_state = 1;
    }
    else
    {
        LED_LAT = 0;
        led_state = 0;
    }

    return led_state;
}

static void Display_Update(uint16_t adc_value, uint8_t led_state)
{
    char adc_text[6];

    UInt16_To_String(adc_value, adc_text);

    SSD1306_ClearLine(3);
    SSD1306_SetCursor(0, 3);
    SSD1306_WriteString("ADC: ");
    SSD1306_WriteString(adc_text);

    SSD1306_ClearLine(4);
    SSD1306_SetCursor(0, 4);

    if (led_state)
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
    /*
       Convierte un valor de 0 a 1023 en texto.
       Ejemplo:
       0    -> "0"
       25   -> "25"
       600  -> "600"
       1023 -> "1023"
    */

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
