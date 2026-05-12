#include "system.h"              // Librería principal del sistema
#include "adc.h"                 // Librería para manejo del ADC
#include "i2c_master.h"          // Librería para comunicación I2C maestro
#include "ssd1306.h"             // Librería pantalla OLED SSD1306
#include "mq135.h"               // Librería sensor MQ135
#include "light_sensor.h"        // Librería sensor de luz
#include "buzzer.h"              // Librería buzzer
#include "fan_pwm.h"             // Librería PWM ventilador
#include "humidity_sensor.h"     // Librería sensor humedad

#include <stdio.h>               // Librería estándar para sprintf

/*========================================================
    CANALES ADC
========================================================*/

#define LM35_CHANNEL        0u   // Canal ADC LM35
#define MQ135_CHANNEL       1u   // Canal ADC MQ135
#define LIGHT_CHANNEL       2u   // Canal ADC sensor de luz
#define HUMIDITY_CHANNEL    3u   // Canal ADC sensor humedad

/*========================================================
    OLED
========================================================*/

#define OLED_X_OFFSET       8u   // Offset horizontal OLED

/*========================================================
    LEDs DE LUZ
========================================================*/

#define LED1_LAT            LATDbits.LATD0   // Salida LED1
#define LED2_LAT            LATDbits.LATD1   // Salida LED2
#define LED3_LAT            LATDbits.LATD2   // Salida LED3
#define LED4_LAT            LATDbits.LATD3   // Salida LED4
#define LED5_LAT            LATDbits.LATD4   // Salida LED5

#define LED1_TRIS           TRISDbits.TRISD0 // Dirección LED1
#define LED2_TRIS           TRISDbits.TRISD1 // Dirección LED2
#define LED3_TRIS           TRISDbits.TRISD2 // Dirección LED3
#define LED4_TRIS           TRISDbits.TRISD3 // Dirección LED4
#define LED5_TRIS           TRISDbits.TRISD4 // Dirección LED5

/*========================================================
    LED TEMPERATURA
========================================================*/

#define TEMP_LED_LAT        LATDbits.LATD5   // Salida LED temperatura
#define TEMP_LED_TRIS       TRISDbits.TRISD5 // Dirección LED temperatura

#define TEMP_LED_MAX_X10    380u             // Máxima temperatura 38.0°C

/*========================================================
    ADC
========================================================*/

#define ADC_VREF_MV         5000UL           // Referencia ADC 5000mV

/*========================================================
    TIEMPOS
========================================================*/

#define MAIN_LOOP_DELAY_MS  600u
#define STARTUP_LED_DELAY   100u

/*========================================================
    PROTOTIPOS
========================================================*/

<<<<<<< HEAD
static void System_Init(void);
static void Startup_Test(void);
static void Startup_Light_Sequence(void);

static void Read_Sensors(uint16_t *temperature_x10,
                         uint16_t *mq135_adc,
                         uint16_t *light_adc,
                         uint16_t *humidity_adc);
=======
static void System_Init(void); // Inicialización general
static void Startup_Test(void); // Test inicial
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

static uint16_t Read_LM35_Temperature_X10(void); // Leer LM35

<<<<<<< HEAD
static void Light_LEDs_All_Off(void);
static void Light_LEDs_Update(uint8_t leds_count);

static void Temperature_LED_Update(uint16_t temperature_x10);
=======
static void Light_LEDs_Update(uint8_t leds_count); // Actualizar LEDs luz
static void Temperature_LED_Update(uint16_t temperature_x10); // Actualizar LED temperatura
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

static uint8_t Fan_Get_Speed_By_MQ135_And_Humidity(uint16_t mq135_adc, // Obtener velocidad ventilador
                                                   uint16_t humidity_adc);

static uint8_t Process_Components(uint16_t light_adc, // Procesar componentes
                                  uint16_t temperature_x10,
                                  uint16_t mq135_adc,
                                  uint16_t humidity_adc,
                                  uint8_t *light_level,
                                  uint8_t *leds_count);

static void OLED_Prepare_Line(uint8_t line);

static void Display_Header(void);

static void Display_Temperature_Line(uint16_t temperature_x10,
                                     uint16_t humidity_adc);

static void Display_MQ135_Line(uint16_t mq135_adc);

static void Display_Light_Line(uint16_t light_adc,
                               uint8_t light_level);

static void Display_Status_Line(uint16_t temperature_x10,
                                uint16_t mq135_adc,
                                uint8_t leds_count,
                                uint8_t fan_speed);

static void Display_Update(uint16_t temperature_x10, // Actualizar OLED
                           uint16_t mq135_adc,
                           uint16_t light_adc,
                           uint16_t humidity_adc,
                           uint8_t light_level,
                           uint8_t leds_count,
                           uint8_t fan_speed);

<<<<<<< HEAD
static void UInt16_To_String(uint16_t value, char *buffer);
static void Temperature_To_String(uint16_t temperature_x10,
                                  char *buffer);
=======
static void UInt16_To_String(uint16_t value, char *buffer); // Convertir entero a texto
static void Temperature_To_String(uint16_t temperature_x10, char *buffer); // Convertir temperatura
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

/*========================================================*/

void main(void) // Función principal
{
<<<<<<< HEAD
    uint16_t temperature_x10;
    uint16_t mq135_adc;
    uint16_t light_adc;
    uint16_t humidity_adc;

    uint8_t fan_speed;
    uint8_t light_level;
    uint8_t leds_count;
=======
    uint16_t temperature_x10; // Variable temperatura
    uint16_t mq135_adc; // Variable MQ135
    uint16_t light_adc_raw; // ADC luz crudo
    uint16_t light_adc; // ADC luz procesado
    uint16_t humidity_adc; // ADC humedad
    uint8_t fan_speed; // Velocidad ventilador
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

    System_Init(); // Inicializar sistema

    Startup_Test(); // Ejecutar test inicial

<<<<<<< HEAD
    Display_Header();
=======
    SSD1306_ClearDisplay(); // Limpiar OLED

    SSD1306_SetCursor(OLED_X_OFFSET, 0); // Cursor línea 0
    SSD1306_WriteString("SISTEMA AMBIENTAL"); // Mostrar título

    SSD1306_SetCursor(OLED_X_OFFSET, 1); // Cursor línea 1
    SSD1306_WriteString("LM35 MQ LUZ HUM"); // Mostrar encabezados
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

    while (1) // Bucle infinito
    {
<<<<<<< HEAD
        Read_Sensors(&temperature_x10,
                     &mq135_adc,
                     &light_adc,
                     &humidity_adc);
=======
        /*
           LM35.
        */

        temperature_x10 = Read_LM35_Temperature_X10(); // Leer temperatura

        /*
           MQ135.
        */

        mq135_adc = ADC_Read(MQ135_CHANNEL); // Leer MQ135

        /*
           Sensor de luz.
        */

        light_adc_raw = ADC_Read(LIGHT_CHANNEL); // Leer luz cruda

        light_adc = Light_Process_ADC(light_adc_raw); // Procesar luz

        /*
           Sensor humedad.
        */

        humidity_adc = ADC_Read(HUMIDITY_CHANNEL); // Leer humedad

        /*
           Actualizar componentes.
        */
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

        fan_speed = Process_Components(light_adc, // Procesar sistema
                                       temperature_x10,
                                       mq135_adc,
                                       humidity_adc,
                                       &light_level,
                                       &leds_count);

        Display_Update(temperature_x10, // Actualizar pantalla
                       mq135_adc,
                       light_adc,
                       humidity_adc,
                       light_level,
                       leds_count,
                       fan_speed);

<<<<<<< HEAD
        __delay_ms(MAIN_LOOP_DELAY_MS);
=======
        __delay_ms(600); // Esperar 600ms
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)
    }
}

/*========================================================*/

static void System_Init(void) // Inicialización sistema
{
    OSCCON = 0x72; // Configurar oscilador interno

    CMCON = 0x07; // Desactivar comparadores

    CVRCON = 0x00; // Desactivar referencia comparador

<<<<<<< HEAD
    LED1_TRIS = 0;
    LED2_TRIS = 0;
    LED3_TRIS = 0;
    LED4_TRIS = 0;
    LED5_TRIS = 0;

    TEMP_LED_TRIS = 0;

    Light_LEDs_All_Off();

    TEMP_LED_LAT = 0;

    Buzzer_Init();

    ADC_Init();

    I2C_Master_Init(100000UL);
=======
    /*
       LEDs luz.
    */

    LED1_TRIS = 0; // LED1 salida
    LED2_TRIS = 0; // LED2 salida
    LED3_TRIS = 0; // LED3 salida
    LED4_TRIS = 0; // LED4 salida
    LED5_TRIS = 0; // LED5 salida

    LED1_LAT = 0; // Apagar LED1
    LED2_LAT = 0; // Apagar LED2
    LED3_LAT = 0; // Apagar LED3
    LED4_LAT = 0; // Apagar LED4
    LED5_LAT = 0; // Apagar LED5

    /*
       LED temperatura.
    */

    TEMP_LED_TRIS = 0; // LED temperatura salida

    TEMP_LED_LAT = 0; // Apagar LED temperatura

    /*
       Buzzer.
    */

    Buzzer_Init(); // Inicializar buzzer

    /*
       ADC.
    */

    ADC_Init(); // Inicializar ADC

    /*
       I2C OLED.
    */

    I2C_Master_Init(100000UL); // Inicializar I2C 100kHz
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

    SSD1306_Init(); // Inicializar OLED

<<<<<<< HEAD
    Fan_PWM_Init();
=======
    /*
       PWM ventilador.
    */

    Fan_PWM_Init(); // Inicializar PWM ventilador
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)
}

/*========================================================*/

static void Startup_Test(void) // Test inicial hardware
{
<<<<<<< HEAD
    Startup_Light_Sequence();

    TEMP_LED_LAT = 1;

    __delay_ms(200);

    Buzzer_Set(1u);

    __delay_ms(200);
=======
    LED1_LAT = 1; // Encender LED1
    __delay_ms(100); // Esperar 100ms

    LED2_LAT = 1; // Encender LED2
    __delay_ms(100); // Esperar 100ms

    LED3_LAT = 1; // Encender LED3
    __delay_ms(100); // Esperar 100ms

    LED4_LAT = 1; // Encender LED4
    __delay_ms(100); // Esperar 100ms

    LED5_LAT = 1; // Encender LED5
    __delay_ms(100); // Esperar 100ms

    TEMP_LED_LAT = 1; // Encender LED temperatura
    __delay_ms(200); // Esperar 200ms

    Buzzer_Set(1u); // Encender buzzer
    __delay_ms(200); // Esperar 200ms
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

    Buzzer_Set(0u); // Apagar buzzer

    Fan_Set_Speed(FAN_SPEED_3); // Ventilador máxima velocidad

    __delay_ms(700); // Esperar 700ms

    Fan_Set_Speed(FAN_SPEED_OFF); // Apagar ventilador

<<<<<<< HEAD
    Light_LEDs_All_Off();
=======
    LED1_LAT = 0; // Apagar LED1
    LED2_LAT = 0; // Apagar LED2
    LED3_LAT = 0; // Apagar LED3
    LED4_LAT = 0; // Apagar LED4
    LED5_LAT = 0; // Apagar LED5
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

    TEMP_LED_LAT = 0; // Apagar LED temperatura

    __delay_ms(200); // Esperar 200ms
}

/*========================================================*/

<<<<<<< HEAD
static void Startup_Light_Sequence(void)
{
    LED1_LAT = 1;
    __delay_ms(STARTUP_LED_DELAY);

    LED2_LAT = 1;
    __delay_ms(STARTUP_LED_DELAY);

    LED3_LAT = 1;
    __delay_ms(STARTUP_LED_DELAY);

    LED4_LAT = 1;
    __delay_ms(STARTUP_LED_DELAY);

    LED5_LAT = 1;
    __delay_ms(STARTUP_LED_DELAY);
}

/*========================================================*/

static void Read_Sensors(uint16_t *temperature_x10,
                         uint16_t *mq135_adc,
                         uint16_t *light_adc,
                         uint16_t *humidity_adc)
{
    uint16_t light_adc_raw;

    *temperature_x10 = Read_LM35_Temperature_X10();

    *mq135_adc = ADC_Read(MQ135_CHANNEL);

    light_adc_raw = ADC_Read(LIGHT_CHANNEL);

    *light_adc = Light_Process_ADC(light_adc_raw);

    *humidity_adc = ADC_Read(HUMIDITY_CHANNEL);
}

/*========================================================*/

static uint16_t Read_LM35_Temperature_X10(void)
=======
static uint16_t Read_LM35_Temperature_X10(void) // Leer LM35
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)
{
    uint16_t adc_value; // Variable ADC

    uint32_t temperature_x10; // Variable temperatura

    adc_value = ADC_Read(LM35_CHANNEL); // Leer ADC LM35

    temperature_x10 =
        ((uint32_t)adc_value * ADC_VREF_MV) / 1023UL; // Convertir ADC a mV

    return (uint16_t)temperature_x10; // Retornar temperatura
}

/*========================================================*/

<<<<<<< HEAD
static void Light_LEDs_All_Off(void)
{
    LED1_LAT = 0;
    LED2_LAT = 0;
    LED3_LAT = 0;
    LED4_LAT = 0;
    LED5_LAT = 0;
}

/*========================================================*/

static void Light_LEDs_Update(uint8_t leds_count)
{
    Light_LEDs_All_Off();
=======
static void Light_LEDs_Update(uint8_t leds_count) // Actualizar LEDs
{
    LED1_LAT = 0; // Apagar LED1
    LED2_LAT = 0; // Apagar LED2
    LED3_LAT = 0; // Apagar LED3
    LED4_LAT = 0; // Apagar LED4
    LED5_LAT = 0; // Apagar LED5
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

    if (leds_count >= 1u) // Si LEDs >=1
    {
        LED1_LAT = 1; // Encender LED1
    }

    if (leds_count >= 2u) // Si LEDs >=2
    {
        LED2_LAT = 1; // Encender LED2
    }

    if (leds_count >= 3u) // Si LEDs >=3
    {
        LED3_LAT = 1; // Encender LED3
    }

    if (leds_count >= 4u) // Si LEDs >=4
    {
        LED4_LAT = 1; // Encender LED4
    }

    if (leds_count >= 5u) // Si LEDs >=5
    {
        LED5_LAT = 1; // Encender LED5
    }
    
}
 /*========================================================*/

static void Temperature_LED_Update(uint16_t temperature_x10) // Actualizar LED temperatura
{
    if (temperature_x10 <= TEMP_LED_MAX_X10) // Si temperatura <= 38.0°C
    {
        TEMP_LED_LAT = 1; // Encender LED temperatura
    }
    else // Si temperatura > 38.0°C
    {
        TEMP_LED_LAT = 0; // Apagar LED temperatura
    }
}

/*========================================================*/

static uint8_t Fan_Get_Speed_By_MQ135_And_Humidity(uint16_t mq135_adc, // Obtener velocidad ventilador
                                                   uint16_t humidity_adc)
{
    uint8_t fan_speed; // Variable velocidad ventilador

    fan_speed = Fan_Get_Speed_By_MQ135(mq135_adc); // Obtener velocidad según MQ135

    if (Humidity_Should_Turn_Fan_On(humidity_adc)) // Si humedad requiere ventilador
    {
        if (fan_speed == FAN_SPEED_OFF) // Si ventilador apagado
        {
            fan_speed = FAN_SPEED_1; // Colocar velocidad mínima
        }
    }

    return fan_speed; // Retornar velocidad ventilador
}

/*========================================================*/

static uint8_t Process_Components(uint16_t light_adc, // Procesar componentes
                                  uint16_t temperature_x10,
                                  uint16_t mq135_adc,
                                  uint16_t humidity_adc,
                                  uint8_t *light_level,
                                  uint8_t *leds_count)
{
<<<<<<< HEAD
    uint8_t fan_speed;

    *light_level = Light_Get_Level(light_adc);

    *leds_count = Light_Get_Leds_Count(*light_level);

    Light_LEDs_Update(*leds_count);
=======
    uint8_t light_level; // Nivel luz

    uint8_t leds_count; // Cantidad LEDs

    uint8_t fan_speed; // Velocidad ventilador

    light_level = Light_Get_Level(light_adc); // Obtener nivel luz

    leds_count = Light_Get_Leds_Count(light_level); // Obtener cantidad LEDs

    Light_LEDs_Update(leds_count); // Actualizar LEDs
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

    Temperature_LED_Update(temperature_x10); // Actualizar LED temperatura

    Buzzer_Update_By_MQ135(mq135_adc); // Actualizar buzzer

    fan_speed =
        Fan_Get_Speed_By_MQ135_And_Humidity(mq135_adc, // Obtener velocidad ventilador
                                            humidity_adc);

    Fan_Set_Speed(fan_speed); // Aplicar velocidad ventilador

    return fan_speed; // Retornar velocidad ventilador
}

/*========================================================*/

<<<<<<< HEAD
static void OLED_Prepare_Line(uint8_t line)
{
    SSD1306_ClearLine(line);

    SSD1306_SetCursor(OLED_X_OFFSET, line);
}

/*========================================================*/

static void Display_Header(void)
{
    SSD1306_ClearDisplay();

    OLED_Prepare_Line(0);
    SSD1306_WriteString("SISTEMA AMBIENTAL");

    OLED_Prepare_Line(1);
    SSD1306_WriteString("LM35 MQ LUZ HUM");
}

/*========================================================*/

static void Display_Temperature_Line(uint16_t temperature_x10,
                                     uint16_t humidity_adc)
=======
static void Display_Update(uint16_t temperature_x10, // Actualizar OLED
                           uint16_t mq135_adc,
                           uint16_t light_adc,
                           uint16_t humidity_adc,
                           uint8_t fan_speed)
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)
{
    char temp_text[8]; // Texto temperatura

<<<<<<< HEAD
    Temperature_To_String(temperature_x10,
                          temp_text);

    OLED_Prepare_Line(2);

    SSD1306_WriteString("T:");
    SSD1306_WriteString(temp_text);
    SSD1306_WriteString("C ");
    SSD1306_WriteString(Humidity_Get_Text(humidity_adc));
}

/*========================================================*/

static void Display_MQ135_Line(uint16_t mq135_adc)
{
    char mq_text[6];

    uint8_t mq_level;

    UInt16_To_String(mq135_adc,
                     mq_text);
=======
    char mq_text[6]; // Texto MQ135

    char light_text[6]; // Texto luz

    char humidity_text[15]; // Texto humedad

    uint8_t mq_level; // Nivel MQ135

    uint8_t light_level; // Nivel luz

    uint8_t leds_count; // Cantidad LEDs

    Temperature_To_String(temperature_x10, temp_text); // Convertir temperatura a texto

    UInt16_To_String(mq135_adc, mq_text); // Convertir MQ135 a texto

    UInt16_To_String(light_adc, light_text); // Convertir luz a texto

    sprintf(humidity_text, // Formatear humedad
            "%s",
            Humidity_Get_Text(humidity_adc));
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

    mq_level = MQ135_Get_Level(mq135_adc); // Obtener nivel MQ135

<<<<<<< HEAD
    OLED_Prepare_Line(3);

    SSD1306_WriteString("MQ:");
    SSD1306_WriteString(mq_text);
    SSD1306_WriteString(" ");
    SSD1306_WriteString(MQ135_Get_Text(mq_level));
}
=======
    light_level = Light_Get_Level(light_adc); // Obtener nivel luz

    leds_count = Light_Get_Leds_Count(light_level); // Obtener cantidad LEDs

    /*
       Linea 2.
    */

    SSD1306_ClearLine(2); // Limpiar línea 2

    SSD1306_SetCursor(OLED_X_OFFSET, 2); // Cursor línea 2

    SSD1306_WriteString("T:"); // Mostrar T:

    SSD1306_WriteString(temp_text); // Mostrar temperatura

    SSD1306_WriteString("C "); // Mostrar unidad

    SSD1306_WriteString(humidity_text); // Mostrar humedad

    /*
       Linea 3.
    */

    SSD1306_ClearLine(3); // Limpiar línea 3

    SSD1306_SetCursor(OLED_X_OFFSET, 3); // Cursor línea 3

    SSD1306_WriteString("MQ:"); // Mostrar MQ

    SSD1306_WriteString(mq_text); // Mostrar valor MQ

    SSD1306_WriteString(" "); // Espacio

    SSD1306_WriteString(MQ135_Get_Text(mq_level)); // Mostrar estado MQ
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

/*========================================================*/

<<<<<<< HEAD
static void Display_Light_Line(uint16_t light_adc,
                               uint8_t light_level)
{
    char light_text[6];

    UInt16_To_String(light_adc,
                     light_text);

    OLED_Prepare_Line(4);

    SSD1306_WriteString("LUZ:");
    SSD1306_WriteString(light_text);

    OLED_Prepare_Line(5);

    SSD1306_WriteString(Light_Get_Text(light_level));
}
=======
    SSD1306_ClearLine(4); // Limpiar línea 4

    SSD1306_SetCursor(OLED_X_OFFSET, 4); // Cursor línea 4

    SSD1306_WriteString("LUZ:"); // Mostrar texto luz

    SSD1306_WriteString(light_text); // Mostrar valor luz

    /*
       Linea 5.
    */

    SSD1306_ClearLine(5); // Limpiar línea 5

    SSD1306_SetCursor(OLED_X_OFFSET, 5); // Cursor línea 5

    SSD1306_WriteString(Light_Get_Text(light_level)); // Mostrar estado luz
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

/*========================================================*/

<<<<<<< HEAD
static void Display_Status_Line(uint16_t temperature_x10,
                                uint16_t mq135_adc,
                                uint8_t leds_count,
                                uint8_t fan_speed)
{
    OLED_Prepare_Line(6);
=======
    SSD1306_ClearLine(6); // Limpiar línea 6

    SSD1306_SetCursor(OLED_X_OFFSET, 6); // Cursor línea 6
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

    SSD1306_WriteString("L:"); // Mostrar LEDs

    SSD1306_WriteChar((char)(leds_count + '0')); // Mostrar cantidad LEDs

    SSD1306_WriteString("/5 "); // Mostrar total LEDs

    if (temperature_x10 <= TEMP_LED_MAX_X10) // Si temperatura correcta
    {
        SSD1306_WriteString("TLED:ON"); // Mostrar LED ON
    }
    else // Si temperatura alta
    {
        SSD1306_WriteString("TLED:OFF"); // Mostrar LED OFF
    }

<<<<<<< HEAD
    OLED_Prepare_Line(7);
=======
    /*
       Linea 7.
    */

    SSD1306_ClearLine(7); // Limpiar línea 7

    SSD1306_SetCursor(OLED_X_OFFSET, 7); // Cursor línea 7
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)

    if (Buzzer_Should_Be_On(mq135_adc)) // Si buzzer activo
    {
        SSD1306_WriteString("BZ:ON "); // Mostrar buzzer ON
    }
    else // Si buzzer apagado
    {
        SSD1306_WriteString("BZ:OFF "); // Mostrar buzzer OFF
    }

    if (fan_speed == FAN_SPEED_OFF) // Si ventilador apagado
    {
        SSD1306_WriteString("FAN:OFF"); // Mostrar ventilador OFF
    }
    else if (fan_speed == FAN_SPEED_1) // Si velocidad 1
    {
        SSD1306_WriteString("FAN:V1"); // Mostrar velocidad 1
    }
    else if (fan_speed == FAN_SPEED_2) // Si velocidad 2
    {
        SSD1306_WriteString("FAN:V2"); // Mostrar velocidad 2
    }
    else // Si velocidad 3
    {
        SSD1306_WriteString("FAN:V3"); // Mostrar velocidad 3
    }
}
 
/*========================================================*/

static void Display_Update(uint16_t temperature_x10,
                           uint16_t mq135_adc,
                           uint16_t light_adc,
                           uint16_t humidity_adc,
                           uint8_t light_level,
                           uint8_t leds_count,
                           uint8_t fan_speed)
{
    Display_Temperature_Line(temperature_x10,
                             humidity_adc);

    Display_MQ135_Line(mq135_adc);

    Display_Light_Line(light_adc,
                       light_level);

    Display_Status_Line(temperature_x10,
                        mq135_adc,
                        leds_count,
                        fan_speed);
}

/*========================================================*/

<<<<<<< HEAD
static void UInt16_To_String(uint16_t value,
                             char *buffer)
=======
static void UInt16_To_String(uint16_t value, char *buffer) // Convertir entero a texto
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)
{
    uint8_t i = 0; // Índice temporal
    uint8_t j = 0; // Índice buffer

    char temp[6]; // Buffer temporal

    if (value == 0u) // Si valor es cero
    {
        buffer[0] = '0'; // Colocar cero
        buffer[1] = '\0'; // Fin cadena

        return; // Salir función
    }

    while (value > 0u) // Mientras valor > 0
    {
        temp[i] = (char)((value % 10u) + '0'); // Obtener dígito
        value = value / 10u; // Dividir entre 10
        i++; // Incrementar índice
    }

    while (i > 0u) // Invertir cadena
    {
        i--; // Decrementar índice

        buffer[j] = temp[i]; // Copiar carácter

        j++; // Incrementar índice
    }

    buffer[j] = '\0'; // Fin cadena
}

/*========================================================*/

static void Temperature_To_String(uint16_t temperature_x10, // Convertir temperatura a texto
                                  char *buffer)
{
    uint16_t integer_part; // Parte entera

    uint16_t decimal_part; // Parte decimal

    char integer_text[6]; // Texto entero

    uint8_t i = 0; // Índice

    integer_part = temperature_x10 / 10u; // Obtener parte entera
    decimal_part = temperature_x10 % 10u; // Obtener decimal

    UInt16_To_String(integer_part, integer_text); // Convertir entero

<<<<<<< HEAD
    UInt16_To_String(integer_part,
                     integer_text);

    while (integer_text[i] != '\0')
=======
    while (integer_text[i] != '\0') // Copiar texto entero
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)
    {
        buffer[i] = integer_text[i]; // Copiar carácter
        i++; // Incrementar índice
    }

    buffer[i] = '.'; // Agregar punto decimal
    i++; // Incrementar índice

    buffer[i] = (char)(decimal_part + '0'); // Agregar decimal
    i++; // Incrementar índice

<<<<<<< HEAD
    buffer[i] = (char)(decimal_part + '0');

    i++;

    buffer[i] = '\0';
}
=======
    buffer[i] = '\0'; // Final cadena
} 
>>>>>>> 8c81825 (Se agregan comentarios linea por linea y documentacion completa del sistema ambiental)
