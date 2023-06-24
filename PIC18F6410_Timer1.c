// *****************************************************************************
// PIC 18F6410 Timer 1 demo with interrupt setup
// Test code that flashes an on board LED at a reasonably accurate 1 Hz
// Demonstrates use of 16 bit Timer 1 for interrupt driven interval generation
// Very slight timing innaccuracy is introduced by interrupt latency and timer
// preload time using the method here, but the interrupt interval is very configurable
// This demo code creates a basis for non-blocking tasks such as ADC reads at set
// intervals, stepping through task sub-stages and no halt to processing.
//
// Hardware:
// PIC 18F6410 with 5V power,20 MHz crystal no PLL in use
// Standard PicKit3 ICSP connections, MCLR to +5V via 10K resistor
// RG3 pin 6 LED1 (red) + 560R resistor
// RG4 pin 8 LED2 (yellow) + 560R resistor (not used here)
//
// Author: Steve Williams 20/06/2023
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
// CONFIG3H:
#pragma config CCP2MX = PORTC   // CCP2 Mux (CCP2 input/output is multiplexed with RC1)
#pragma config LPT1OSC = OFF    // Low Power Timer1 Osc enable (Timer1 configured for higher power operation)
#pragma config MCLRE = OFF      // Master Clear Enable (RG5 input pin enabled; MCLR disabled)
// CONFIG4L:
#pragma config STVREN = ON      // Stack Overflow Reset (Stack full/underflow will cause Reset)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
// CONFIG5L:
#pragma config CP = OFF         // Code Protect 00000-03FFF (Program memory block not code-protected)

#include <xc.h>

#define _XTAL_FREQ 20000000      // Define clock frequency used by xc8 __delay(time) functions

#define LED1ON 0x08         // Mask to set port RG3 output
#define LED1OFF 0xF7        // Mask to clear port RG4 output
#define LED2ON 0x10         // Sets RG4
#define LED2OFF 0xEF        // Clears RG4
#define IPENABLE 0          // 1 = enable two level interrupt priority (high/low)

//Timer1 definitions:
#define T1PRESCALE 0x03                // 2 bits control, 11 = 0x03 = 1:8
#define TIMER1ON 0x01                  // Used to set bit0 T1CON = Timer1 ON

// Timer1 setup values for 50ms interrupt using a preload:
#define TIMER1LOWBYTE 0xEE             // Cycle time @ 20MHz = 0.2us. Interrupt interval = 50ms = 250000 cycles
#define TIMER1HIGHBYTE 0x85            // Prescale x8 = 31250 counts. Preload 65536-31250 = 34286 = 0x85EE

// Task management definitions, define stages of non-blocking task here:
#define NOTASK 0
#define TASKSTART 1
#define TASKSTAGE 2
#define TASKEND 3

// General global variables:
volatile uint8_t timer1Flag = 0;               // Flag is set by Timer 1 ISR every 50ms
uint8_t counts50ms = 0;                        // Counts intervals for ADC task in 50ms increments
uint8_t taskStatus = 0;                        // Stage of task, 0 = not started
uint8_t LEDcounter = 0;                        // Used to time non-blocking LED flash in 50ms increments
uint8_t LEDonTime = 0;                         // If true LED flash routine is called, flashes N x 50ms 

// Function prototypes:
void __interrupt(high_priority) ISRhi(void);   // Note XC8 interrupt setup syntax using __interrupt(optional arguments) + myisr()
void initialise18F6410(void);                  // If using 2 level interrupts with 18F PIC also need a low_priority ISR
void LEDflash(void);


void main(void)
{
  _delay(100);
  initialise18F6410();           // Default config is TRIS setup for TM1637 plus LED, Timer1 set up
  counts50ms = 0;                // Start with timing counts at zero, both ADC read and timer1 flags
  timer1Flag = 0; 
  T1CON |= TIMER1ON;
  while(1)
    {
      if (timer1Flag)
        {
           counts50ms ++;                    // Update task interval and LED timing flags
           LEDcounter ++;                           
           timer1Flag = 0;                   // Clear the 50ms timing flag
        }
      
      if (counts50ms >= 20)                  // Start read process every 50ms x 20 = 1 sec
        { 
           counts50ms = 0;
           taskStatus = TASKSTART;           // Start task
           LEDcounter = 0;                   // Zero the LED time counter, note counts 50ms increments
           LEDonTime = 1;                    // Sets up a 500ms LED flash
        }
      
      switch (taskStatus)                   // The task is managed by taskStatus control variable
      {                                     // Example code section shows framework for non-blocking task code
          case NOTASK:
            break;
                  
          case TASKSTART:
            {
              taskStatus = NOTASK;       // No task action this demo, Insert task code and update taskStatus here
              break;
            }
          
          case TASKSTAGE:
            {
              taskStatus = NOTASK;       // Insert task code and update taskStatus here
              break;
            } 
          
          case TASKEND:
            {
              taskStatus = NOTASK;       // End of task
              break;
            }
      }
             
      if (LEDonTime)                     // Call the LED flash function if a count is set
          LEDflash();   
    }                       //while(1)
}        

//***************************************************************************************
// Interrupt service routine:
//***************************************************************************************

void ISRhi(void)
{ 
    if (PIR1 & 0x01)                  // Check Timer1 interrupt flag bit 0 is set
    {
        PIR1 &= 0xFE;                 // Clear interrupt flag bit 0
                                     // Reset Timer1 preload for 100ms overflow/interrupt(nb running timer)
        TMR1H = TIMER1HIGHBYTE;       // Note some timing inaccuracy due to interrupt latency + reload time
        TMR1L = TIMER1LOWBYTE;        // Correction was therefore applied to low byte to improve accuracy 
        timer1Flag = 1;               
        
    }
}

//*******************************************************************************************
//Functions: 
//*******************************************************************************************

void LEDflash(void)
{
    if (LEDcounter <= LEDonTime)
    {
        PORTG |= LED1ON;                      // LED1 on
    }
    else
    {
        PORTG &= LED1OFF;                     // LED1 off
        LEDonTime = 0;                // Stop the flash
    }
}

void initialise18F6410(void)
{   
    PORTG = 0;                       // Clear the outputs first
    TRISG = 0;                       // All PORTG outputs low
    
    RCON |= IPENABLE;              // Enables 2 level interrupt priority if b7 set
    
    // TIMER1 setup with interrupts:
    T1CON = 0x80;                  // T1CON clear apart from b7, RD16, 16 bit writes enabled, b1 clear = int clk F/4 
    T1CON |= (T1PRESCALE<<4);      // Bits 4-5 set prescale, 01 = 1:2
    T1CON |= 0x04;                 // Bit 2 set enables disables external clock input 
    TMR1L = TIMER1LOWBYTE;         // Set Timer1 preload for 1ms overflow/interrupt
    TMR1H = TIMER1HIGHBYTE; 
    PIE1 = 0x01;                   // Timer 1 interrupt enable b0 set, other interrupts disabled
    PIR1 &= 0xFE;                  // Clear Timer1 interrupt flag bit 0
    INTCON |= 0xC0;                // Enable interrupts, general - bit 7 plus peripheral - bit 6 
}