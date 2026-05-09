#include "system.h"
#include "adc.h"
#include "i2c_master.h"
#include "ssd1306.h"
#include "mq135.h"
#include "light_sensor.h"
#include "buzzer.h"

/*
   Canales ADC usados:

   AN0 / RA0 / pin 2 -> LM35
   AN1 / RA1 / pin 3 -> MQ135
   AN2 / RA2 / pin 4 -> Sensor de luz HW-486
*/

#define LM35_CHANNEL        0u
#define MQ135_CHANNEL       1u
#define LIGHT_CHANNEL       2u

/*
   Desplazamiento horizontal del texto en la OLED.
*/

#define OLED_X_OFFSET       8u

/*
   LEDs fisicos para representar intensidad de luz.

   LED 1 -> RD0 / pin 19
   LED 2 -> RD1 / pin 20
   LED 3 -> RD2 / pin 21
   LED 4 -> RD3 / pin 22
   LED 5 -> RD4 / pin 27
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
   LED indicador de temperatura.

   LED_TEMP -> RD5 / pin fisico 28

   Condicion:
   ON  si temperatura esta entre 0 C y 38 C
   OFF si temperatura es mayor a 38 C
*/

#define TEMP_LED_LAT        LATDbits.LATD5
#define TEMP_LED_TRIS       TRISDbits.TRISD5

/*
   La temperatura se maneja multiplicada por 10.

   38.0 C = 380
*/

#define TEMP_LED_MAX_X10    380u

/*
   Referencia ADC.

   Si el PIC18F4550 esta alimentado con 5V
   y el ADC usa VDD como referencia, este valor debe ser 5000.
*/

#define ADC_VREF_MV         5000UL

/*
   Prototipos de funciones internas
*/

static void System_Init(void);
static void Startup_LED_Test(void);

static uint16_t Read_LM35_Temperature_X10(void);

static void Light_LEDs_Update(uint8_t leds_count);
static void Temperature_LED_Update(uint16_t temperature_x10);
static void Process_Components(uint16_t light_adc,
                               uint16_t temperature_x10,
                               uint16_t mq135_adc);

static void Display_Update(uint16_t temperature_x10,
                           uint16_t mq135_adc,
                           uint16_t light_adc);

static void UInt16_To_String(uint16_t value, char *buffer);
static void Temperature_To_String(uint16_t temperature_x10, char *buffer);

/*
   Funcion principal
*/

void main(void)
{
    uint16_t temperature_x10;
    uint16_t mq135_adc;
    uint16_t light_adc_raw;
    uint16_t light_adc;

    System_Init();

    Startup_LED_Test();

    SSD1306_ClearDisplay();

    SSD1306_SetCursor(OLED_X_OFFSET, 0);
    SSD1306_WriteString("SISTEMA AMBIENTAL");

    SSD1306_SetCursor(OLED_X_OFFSET, 1);
    SSD1306_WriteString("LM35 MQ135 LUZ");

    while (1)
    {
        /*
           Leer temperatura del LM35.
        */

        temperature_x10 = Read_LM35_Temperature_X10();

        /*
           Leer sensor MQ135.
        */

        mq135_adc = ADC_Read(MQ135_CHANNEL);

        /*
           Leer sensor de luz HW-486.

           Primero se lee el valor real del ADC.
           Luego se procesa con Light_Process_ADC().
        */

        light_adc_raw = ADC_Read(LIGHT_CHANNEL);
        light_adc = Light_Process_ADC(light_adc_raw);

        /*
           Actualizar LEDs fisicos y buzzer.
        */

        Process_Components(light_adc, temperature_x10, mq135_adc);

        /*
           Actualizar OLED.
        */

        Display_Update(temperature_x10, mq135_adc, light_adc);

        __delay_ms(600);
    }
}

/*
   Inicializacion general del sistema
*/

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
       Configurar LEDs de intensidad de luz como salidas.
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
       Configurar LED de temperatura como salida.
    */

    TEMP_LED_TRIS = 0;
    TEMP_LED_LAT = 0;

    /*
       Inicializar buzzer.
       Buzzer -> RD6 / pin fisico 29.
    */

    Buzzer_Init();

    /*
       Inicializar ADC.

       AN0 -> LM35
       AN1 -> MQ135
       AN2 -> sensor de luz
    */

    ADC_Init();

    /*
       Inicializar I2C.

       OLED SSD1306:
       SDA -> RB0 / pin 33
       SCL -> RB1 / pin 34

       Recuerda conservar en i2c_master.c:
       SSPADD = 119u;
    */

    I2C_Master_Init(100000UL);

    /*
       Inicializar OLED.
    */

    SSD1306_Init();
}

/*
   Prueba inicial de LEDs y buzzer
*/

static void Startup_LED_Test(void)
{
    /*
       Prueba de los 5 LEDs de luz.
    */

    LED1_LAT = 1;
    __delay_ms(120);

    LED2_LAT = 1;
    __delay_ms(120);

    LED3_LAT = 1;
    __delay_ms(120);

    LED4_LAT = 1;
    __delay_ms(120);

    LED5_LAT = 1;
    __delay_ms(120);

    /*
       Prueba del LED de temperatura.
    */

    TEMP_LED_LAT = 1;
    __delay_ms(200);

    /*
       Prueba del buzzer.
    */

    Buzzer_Set(1u);
    __delay_ms(250);
    Buzzer_Set(0u);

    LED1_LAT = 0;
    LED2_LAT = 0;
    LED3_LAT = 0;
    LED4_LAT = 0;
    LED5_LAT = 0;
    TEMP_LED_LAT = 0;

    __delay_ms(200);
}

/*
   Lectura y conversion del LM35
*/

static uint16_t Read_LM35_Temperature_X10(void)
{
    uint16_t adc_value;
    uint32_t temperature_x10;

    adc_value = ADC_Read(LM35_CHANNEL);

    /*
       LM35:
       10 mV = 1 grado Celsius.

       ADC:
       0 - 1023 representa 0V - 5V.

       Voltaje_mV = ADC * 5000 / 1023

       Para mostrar temperatura con un decimal:
       temperature_x10 = temperatura * 10

       En LM35:
       temperature_x10 = Voltaje_mV
    */

    temperature_x10 = ((uint32_t)adc_value * ADC_VREF_MV) / 1023UL;

    return (uint16_t)temperature_x10;
}

/*
   Control de los 5 LEDs de luz
*/

static void Light_LEDs_Update(uint8_t leds_count)
{
    /*
       Apagar todos primero.
    */

    LED1_LAT = 0;
    LED2_LAT = 0;
    LED3_LAT = 0;
    LED4_LAT = 0;
    LED5_LAT = 0;

    /*
       Encendido acumulativo:

       1 LED  -> muy oscuro
       2 LEDs -> poca luz
       3 LEDs -> luz media
       4 LEDs -> mucha luz
       5 LEDs -> luz intensa
    */

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

/*
   Control del LED de temperatura.

   ON  si 0.0 C <= temperatura <= 38.0 C
   OFF si temperatura > 38.0 C
*/

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

/*
   Procesamiento de componentes fisicos
*/

static void Process_Components(uint16_t light_adc,
                               uint16_t temperature_x10,
                               uint16_t mq135_adc)
{
    uint8_t light_level;
    uint8_t leds_count;

    /*
       LEDs del sensor de luz.
    */

    light_level = Light_Get_Level(light_adc);
    leds_count = Light_Get_Leds_Count(light_level);

    Light_LEDs_Update(leds_count);

    /*
       LED de temperatura.
    */

    Temperature_LED_Update(temperature_x10);

    /*
       Buzzer del sensor MQ-135.

       ON si MQ135 esta entre 801 y 1023.
       OFF si MQ135 esta por debajo de 801.
    */

    Buzzer_Update_By_MQ135(mq135_adc);
}

/*
   Actualizacion de la pantalla OLED
*/

static void Display_Update(uint16_t temperature_x10,
                           uint16_t mq135_adc,
                           uint16_t light_adc)
{
    char temp_text[8];
    char mq_text[6];
    char light_text[6];

    uint8_t mq_level;
    uint8_t light_level;
    uint8_t leds_count;

    Temperature_To_String(temperature_x10, temp_text);
    UInt16_To_String(mq135_adc, mq_text);
    UInt16_To_String(light_adc, light_text);

    mq_level = MQ135_Get_Level(mq135_adc);
    light_level = Light_Get_Level(light_adc);
    leds_count = Light_Get_Leds_Count(light_level);

    /*
       Linea 2:
       Temperatura y MQ135
    */

    SSD1306_ClearLine(2);
    SSD1306_SetCursor(OLED_X_OFFSET, 2);
    SSD1306_WriteString("T:");
    SSD1306_WriteString(temp_text);
    SSD1306_WriteString("C MQ:");
    SSD1306_WriteString(mq_text);

    /*
       Linea 3:
       Estado del aire
    */

    SSD1306_ClearLine(3);
    SSD1306_SetCursor(OLED_X_OFFSET, 3);
    SSD1306_WriteString(MQ135_Get_Text(mq_level));

    /*
       Linea 4:
       Valor corregido del sensor de luz
    */

    SSD1306_ClearLine(4);
    SSD1306_SetCursor(OLED_X_OFFSET, 4);
    SSD1306_WriteString("LUZ ADC:");
    SSD1306_WriteString(light_text);

    /*
       Linea 5:
       Estado de iluminacion
    */

    SSD1306_ClearLine(5);
    SSD1306_SetCursor(OLED_X_OFFSET, 5);
    SSD1306_WriteString(Light_Get_Text(light_level));

    /*
       Linea 6:
       LEDs de luz
    */

    SSD1306_ClearLine(6);
    SSD1306_SetCursor(OLED_X_OFFSET, 6);
    SSD1306_WriteString("LEDS LUZ:");
    SSD1306_WriteChar((char)(leds_count + '0'));
    SSD1306_WriteString("/5");

    /*
       Linea 7:
       Estado del LED de temperatura y buzzer
    */

    SSD1306_ClearLine(7);
    SSD1306_SetCursor(OLED_X_OFFSET, 7);

    if (temperature_x10 <= TEMP_LED_MAX_X10)
    {
        SSD1306_WriteString("TLED:ON ");
    }
    else
    {
        SSD1306_WriteString("TLED:OFF");
    }

    if (Buzzer_Should_Be_On(mq135_adc))
    {
        SSD1306_WriteString(" BZ:ON");
    }
    else
    {
        SSD1306_WriteString(" BZ:OFF");
    }
}

/*
   Convertir uint16_t a texto
*/

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

/*
   Convertir temperatura x10 a texto.

   Ejemplo:
   253 -> "25.3"
   300 -> "30.0"
   380 -> "38.0"
*/

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