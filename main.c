#include <xc.h>
#include <stdint.h>
#define _XTAL_FREQ 8000000
// CONFIGURACIÓN

#pragma config FOSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config PBADEN = OFF

// ================== PROTOTIPOS ==================
void IO_Init(void);

// ================== MAIN ==================
void main(void) {

    IO_Init();

    while(1) {
        // Loop principal vacío por ahora
    }
}

// ================== FUNCIONES ==================
void IO_Init(void) {

    TRISA = 0xFF; // Entradas (sensores)
    TRISD = 0x00; // Salidas (actuadores)

    LATD = 0x00;
} 