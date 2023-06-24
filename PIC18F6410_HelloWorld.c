// *****************************************************************************
// PIC 18F6410 Hello World
// Test code that flashes two on board LEDs
//
// Hardware:
// PIC 18F6410 with 5V power,20 MHz crystal no PLL in use
// Standard PicKit3 ICSP connections, MCLR to +5V via 10K resistor
// RG3 pin 6 LED1 (red) + 560R resistor
// RG4 pin 8 LED2 (yellow) + 560R resistor
//
// Author: Steve Williams 18/06/2023
// *****************************************************************************


// PIC18F6410 Configuration Bit Settings generated using MPLAB Production|Set Configuration
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1H:
#pragma config OSC = HS         // Oscillator (HS oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switch Over Mode (Oscillator Switchover mode disabled)
// CONFIG2L:
#pragma config PWRT = OFF       // Power Up Timer (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown Out Detect (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Voltage (VBOR set to 2.1V)
// CONFIG2H:
#pragma config WDT = OFF        // Watchdog Timer (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Postscaler (1:32768)
// CONFIG3L:
// CONFIG3H:
#pragma config CCP2MX = PORTC   // CCP2 Mux (CCP2 input/output is multiplexed with RC1)
#pragma config LPT1OSC = OFF    // Low Power Timer1 Osc enable (Timer1 configured for higher power operation)
#pragma config MCLRE = OFF      // Master Clear Enable (RG5 input pin enabled; MCLR disabled)
// CONFIG4L:
#pragma config STVREN = ON      // Stack Overflow Reset (Stack full/underflow will cause Reset)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
// CONFIG5L:
#pragma config CP = OFF         // Code Protect 00000-03FFF (Program memory block not code-protected)
// CONFIG7L:

#include <xc.h>

#define _XTAL_FREQ 20000000      // Define clock frequency used by xc8 __delay(time) functions

#define LED1ON 0x08         // Mask to set port RG3 output
#define LED1OFF 0xF7        // Mask to clear port RG4 output
#define LED2ON 0x10         // Sets RG4
#define LED2OFF 0xEF        // Clears RG4

void main(void)
{   
    PORTG = 0;               // Clear the outputs first
    TRISG = 0;               // Port G set up so all bits are outputs and are low (RG0 .. 4)
    while(1)
    {
      __delay_ms(1000);            // Output alternating short(0.1 sec) then long(1sec) LED flashes
      PORTG |= LED1ON;             // LED1 to indicate PIC is alive ... "Hello World"
      __delay_ms(100);             // This delay is 100 msec, uses inline compiler function
      PORTG &= LED1OFF;
      __delay_ms(500);             // 0.5 sec
      PORTG |= LED1ON;
      __delay_ms(1000);            // 1 sec
      PORTG &= LED1OFF;
        
      // Now LED2:
      __delay_ms(1000);            // Output alternating short(0.1 sec) then long(1sec) LED flashes
      PORTG |= LED2ON;
      __delay_ms(100);             // Delay 0.1 sec
      PORTG &= LED2OFF;
      __delay_ms(500);             // 0.5 sec
      PORTG |= LED2ON;
      __delay_ms(1000);            // 1 sec
      PORTG &= LED2OFF;
    }
}





