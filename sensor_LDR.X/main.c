#include <xc.h>
#include <stdint.h>

//================ CONFIG BITS =================
#pragma config PLLDIV   = 1
#pragma config CPUDIV   = OSC1_PLL2
#pragma config USBDIV   = 1
#pragma config FOSC     = HS
#pragma config FCMEN    = OFF
#pragma config IESO     = OFF
#pragma config PWRT     = ON
#pragma config BOR      = OFF
#pragma config VREGEN   = OFF
#pragma config WDT      = OFF
#pragma config MCLRE    = ON
#pragma config LPT1OSC  = OFF
#pragma config PBADEN   = OFF
#pragma config CCP2MX   = OFF
#pragma config STVREN   = ON
#pragma config LVP      = OFF
#pragma config XINST    = OFF
#pragma config CP0      = OFF
#pragma config CP1      = OFF
#pragma config CP2      = OFF
#pragma config CP3      = OFF
#pragma config CPB      = OFF
#pragma config WRT0     = OFF
#pragma config WRT1     = OFF
#pragma config WRT2     = OFF
#pragma config WRT3     = OFF
#pragma config WRTB     = OFF
#pragma config WRTC     = OFF
#pragma config WRTD     = OFF
#pragma config EBTR0    = OFF
#pragma config EBTR1    = OFF
#pragma config EBTR2    = OFF
#pragma config EBTR3    = OFF
#pragma config EBTRB    = OFF

#define _XTAL_FREQ 8000000UL

//================ OLED =================
#define SSD1306_ADDR 0x78   // 0x3C << 1

//================ PROTOTIPOS =================
void I2C_Init(void);
void I2C_Wait(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(uint8_t data);

void OLED_Command(uint8_t cmd);
void OLED_Data(uint8_t data);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_SetCursor(uint8_t page, uint8_t col);
void OLED_Char(char c);
void OLED_String(const char *str);

void ADC_Init(void);
uint16_t ADC_Read(uint8_t channel);

void intToStr(uint16_t num, char *str);
void showValue(uint16_t adc, uint8_t percent);

//================ FUENTE 5x7 =================
const uint8_t font_space[5]   = {0x00,0x00,0x00,0x00,0x00};
const uint8_t font_percent[5] = {0x62,0x64,0x08,0x13,0x23};
const uint8_t font_colon[5]   = {0x00,0x36,0x36,0x00,0x00};

const uint8_t font_0[5] = {0x3E,0x51,0x49,0x45,0x3E};
const uint8_t font_1[5] = {0x00,0x42,0x7F,0x40,0x00};
const uint8_t font_2[5] = {0x42,0x61,0x51,0x49,0x46};
const uint8_t font_3[5] = {0x21,0x41,0x45,0x4B,0x31};
const uint8_t font_4[5] = {0x18,0x14,0x12,0x7F,0x10};
const uint8_t font_5[5] = {0x27,0x45,0x45,0x45,0x39};
const uint8_t font_6[5] = {0x3C,0x4A,0x49,0x49,0x30};
const uint8_t font_7[5] = {0x01,0x71,0x09,0x05,0x03};
const uint8_t font_8[5] = {0x36,0x49,0x49,0x49,0x36};
const uint8_t font_9[5] = {0x06,0x49,0x49,0x29,0x1E};

const uint8_t font_A[5] = {0x7E,0x11,0x11,0x11,0x7E};
const uint8_t font_C[5] = {0x3E,0x41,0x41,0x41,0x22};
const uint8_t font_D[5] = {0x7F,0x41,0x41,0x22,0x1C};
const uint8_t font_L[5] = {0x7F,0x40,0x40,0x40,0x40};
const uint8_t font_R[5] = {0x7F,0x09,0x19,0x29,0x46};
const uint8_t font_U[5] = {0x3F,0x40,0x40,0x40,0x3F};
const uint8_t font_Z[5] = {0x61,0x51,0x49,0x45,0x43};

//================ I2C =================
void I2C_Init(void){
    TRISC |= 0x18;      // RC3 y RC4 como entradas

    SSPSTAT = 0x80;
    SSPCON1 = 0x28;     // I2C Master
    SSPCON2 = 0x00;
    SSPADD  = 19;       // ~100kHz con Fosc=8MHz
    PIR1bits.SSPIF = 0;
}

void I2C_Wait(void){
    while ((SSPCON2 & 0x1F) || (SSPSTATbits.R_W));
}

void I2C_Start(void){
    I2C_Wait();
    SSPCON2bits.SEN = 1;
    while(SSPCON2bits.SEN);
}

void I2C_Stop(void){
    I2C_Wait();
    SSPCON2bits.PEN = 1;
    while(SSPCON2bits.PEN);
}

void I2C_Write(uint8_t data){
    I2C_Wait();
    SSPBUF = data;
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
}

//================ OLED =================
void OLED_Command(uint8_t cmd){
    I2C_Start();
    I2C_Write(SSD1306_ADDR);
    I2C_Write(0x00);
    I2C_Write(cmd);
    I2C_Stop();
}

void OLED_Data(uint8_t data){
    I2C_Start();
    I2C_Write(SSD1306_ADDR);
    I2C_Write(0x40);
    I2C_Write(data);
    I2C_Stop();
}

void OLED_Init(void){
    __delay_ms(100);

    OLED_Command(0xAE);
    OLED_Command(0xD5); OLED_Command(0x80);
    OLED_Command(0xA8); OLED_Command(0x3F);
    OLED_Command(0xD3); OLED_Command(0x00);
    OLED_Command(0x40);
    OLED_Command(0x8D); OLED_Command(0x14);
    OLED_Command(0x20); OLED_Command(0x02);
    OLED_Command(0xA1);
    OLED_Command(0xC8);
    OLED_Command(0xDA); OLED_Command(0x12);
    OLED_Command(0x81); OLED_Command(0x7F);
    OLED_Command(0xD9); OLED_Command(0xF1);
    OLED_Command(0xDB); OLED_Command(0x40);
    OLED_Command(0xA4);
    OLED_Command(0xA6);
    OLED_Command(0xAF);

    OLED_Clear();
}

void OLED_SetCursor(uint8_t page, uint8_t col){
    OLED_Command(0xB0 + page);
    OLED_Command(0x00 + (col & 0x0F));
    OLED_Command(0x10 + ((col >> 4) & 0x0F));
}

void OLED_Clear(void){
    uint8_t page, col;
    for(page = 0; page < 8; page++){
        OLED_SetCursor(page, 0);
        for(col = 0; col < 128; col++){
            OLED_Data(0x00);
        }
    }
}

void OLED_Char(char c){
    const uint8_t *ptr = font_space;
    uint8_t i;

    switch(c){
        case ' ': ptr = font_space; break;
        case '%': ptr = font_percent; break;
        case ':': ptr = font_colon; break;
        case '0': ptr = font_0; break;
        case '1': ptr = font_1; break;
        case '2': ptr = font_2; break;
        case '3': ptr = font_3; break;
        case '4': ptr = font_4; break;
        case '5': ptr = font_5; break;
        case '6': ptr = font_6; break;
        case '7': ptr = font_7; break;
        case '8': ptr = font_8; break;
        case '9': ptr = font_9; break;
        case 'A': ptr = font_A; break;
        case 'C': ptr = font_C; break;
        case 'D': ptr = font_D; break;
        case 'L': ptr = font_L; break;
        case 'R': ptr = font_R; break;
        case 'U': ptr = font_U; break;
        case 'Z': ptr = font_Z; break;
        default: ptr = font_space; break;
    }

    for(i = 0; i < 5; i++){
        OLED_Data(ptr[i]);
    }
    OLED_Data(0x00);
}

void OLED_String(const char *str){
    while(*str){
        OLED_Char(*str++);
    }
}

//================ ADC =================
void ADC_Init(void){
    ADCON1 = 0x0E;      // AN0 analogico
    ADCON2 = 0xA9;      // Justificado a la derecha
    ADCON0 = 0x01;      // ADC ON, canal 0
    TRISA |= 0x01;      // RA0 como entrada
}

uint16_t ADC_Read(uint8_t channel){
    channel &= 0x0F;

    ADCON0 &= 0xC3;
    ADCON0 |= (channel << 2);

    __delay_us(20);

    ADCON0bits.GO_DONE = 1;
    while(ADCON0bits.GO_DONE);

    return (((uint16_t)ADRESH << 8) | ADRESL);
}

//================ UTILIDADES =================
void intToStr(uint16_t num, char *str){
    char temp[6];
    uint8_t i = 0, j = 0;

    if(num == 0){
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while(num > 0){
        temp[i++] = (num % 10) + '0';
        num /= 10;
    }

    while(i > 0){
        str[j++] = temp[--i];
    }
    str[j] = '\0';
}

void showValue(uint16_t adc, uint8_t percent){
    char buf[6];

    OLED_Clear();

    OLED_SetCursor(0, 0);
    OLED_String("LDR");

    OLED_SetCursor(2, 0);
    OLED_String("ADC:");
    intToStr(adc, buf);
    OLED_String(buf);

    OLED_SetCursor(4, 0);
    OLED_String("LUZ:");
    intToStr(percent, buf);
    OLED_String(buf);
    OLED_String("%");
}

//================ MAIN =================
void main(void){
    uint16_t adcValue;
    uint8_t percent;

    ADC_Init();
    I2C_Init();
    OLED_Init();

    while(1){
        adcValue = ADC_Read(0);
        percent = (uint8_t)((adcValue * 100UL) / 1023UL);

        showValue(adcValue, percent);
        __delay_ms(300);
    }
}

