#include <xc.h>
#include <stdint.h>
#define _XTAL_FREQ 8000000

// CONFIGURACIÓN
#pragma config FOSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config PBADEN = OFF

// ================= PROTOTIPOS =================
void IO_Init(void);
void ADC_Init(void);
unsigned int ADC_Read(unsigned char canal);

// ================= MAIN =================
void main(void) {

    IO_Init();
    ADC_Init();

    unsigned int valor_adc;

    while(1) {

        valor_adc = ADC_Read(0);  // Leer AN0

        __delay_ms(1000);
    }
}

// ================= FUNCIONES =================
void IO_Init(void) {

    TRISA = 0xFF; // Entradas (AN0)
    TRISD = 0x00; // Salidas

    LATD = 0x00;
}

// ================= ADC =================
void ADC_Init() {

    ADCON0 = 0x01;  // ADC ON, canal AN0
    ADCON1 = 0x0E;  // AN0 analógico
    ADCON2 = 0b10101010;
}

unsigned int ADC_Read(unsigned char canal) {

    ADCON0 &= 0b11000101;
    ADCON0 |= (canal << 2);

    __delay_us(10);

    GO_nDONE = 1;
    while(GO_nDONE);

    return (ADRESH << 8) | ADRESL;
} 