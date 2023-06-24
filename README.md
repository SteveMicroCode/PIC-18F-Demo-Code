# PIC-18F-Demo-Code
Collection of example PIC 18F code
No warranty of any sort implied. Demo code only, use at your own risk.

Compiled using Microchip XC8 free version.
Tested on PIC18F6410 @ 20MHz though should work on other family variants. 

Hello World is just an LED flashing program that confirms that the PIC is alive

PIC18F6410_Timer1.c demonstates setup of Timer1 with interrupt based timing. a
1 second interval is generated. Note slight latency errors occur with this method
of timer use. There is a short example state machine framework where non-blocking
application code can be added.
