/*
 * File:   uart1.c
 * Author: Fardin Mahid
 *
 * 
 */



#include "xc.h"
#include "uart1.h"
#include "string.h"
#include <stdlib.h>


uint8_t received_char = 0;
uint8_t RXFlag = 0;
uint8_t TXFlag = 0;
void InitUART1(void) 
{
    // Configure UART1 for KIM Module (TX = RB7, RX = RB2)
    
    U1MODEbits.USIDL = 0;  // Continue operation in Idle mode
    U1MODEbits.IREN = 0;   // No IR translation
    U1MODEbits.RTSMD = 0;  // Simplex Mode (no flow control)
    U1MODEbits.UEN = 0b00; // TX, RX enabled (no CTS/RTS)
    U1MODEbits.WAKE = 0;   // No wake-up (since we don't sleep here)
    U1MODEbits.LPBACK = 0; // No loopback
    U1MODEbits.ABAUD = 0;  // No autobaud detection
    U1MODEbits.RXINV = 0;  // Normal RX polarity
    U1MODEbits.BRGH = 1;   // High-speed mode
    U1MODEbits.PDSEL = 0;  // 8-bit data, no parity
    U1MODEbits.STSEL = 0;  // 1 stop bit

    // Set Baud Rate for KIM (4800 baud)
    if (OSCCONbits.COSC == 0b110)
    {
        U1BRG = 12; // 4800 baud @ 500 kHz FOSC
    }
    else if (OSCCONbits.COSC == 0b101)
    {
        U1BRG = 12; // 300 baud @ 32 kHz FOSC
    }
    else if (OSCCONbits.COSC == 0b000)
    {
        U1BRG = 12; // 4800 baud @ 8 MHz FOSC
    }

    // Interrupt Configurations
    U1STAbits.UTXISEL1 = 0; // Interrupt when Char is transferred
    U1STAbits.UTXISEL0 = 1; // Generate interrupt with last character shifted out
    U1STAbits.URXISEL = 0;  // Interrupt on character received
    U1STAbits.UTXINV = 0;   // Normal TX polarity
    U1STAbits.UTXBRK = 0;   // Disable Break mode

    IFS0bits.U1TXIF = 0;  // Clear TX Interrupt Flag
    IPC3bits.U1TXIP = 3;  // Set UART1 TX interrupt priority
    IEC0bits.U1TXIE = 1;  // Enable TX Interrupt

    IFS0bits.U1RXIF = 0;  // Clear RX Interrupt Flag
    IPC2bits.U1RXIP = 4;  // Set UART1 RX interrupt priority
    IEC0bits.U1RXIE = 1;  // Enable RX Interrupt

    U1MODEbits.UARTEN = 1; // Enable UART1

//  U1STAbits.UTXEN = 1; // Enable TX (if needed in main loop)
    
}

void sendto_KIM(char* str){
    unsigned int i;
    for (i=0; i<= strlen(str); i++)
    {
        XmitUART1(str[i],1);
    }

    return;
    
}

void XmitUART1(char CharNum, unsigned int repeatNo){
    U1STAbits.UTXEN = 1; // We start the UART transmission
	while(repeatNo!=0) 
	{
		while(U1STAbits.UTXBF==1)   // We loop here because if the UART buffer is not 0, we do not write to it, so we loop indefinitely until we can.
		{
		}	
		U1TXREG=CharNum;            // We put CharNum here because this is what we want to transmit. Its in the transmit register.
		repeatNo--;                //
	}
	while(U1STAbits.TRMT==0)        // We check while the transmit shift register
	{                               // is not empty,and a transmission is in progress or queued, we cannot transmit so we loop indefinitely until we can.
	}

    U1STAbits.UTXEN = 0;
    
}


void recv_KIM(char* input, uint8_t buf_size)
{	
    uint16_t i = 0;
   
    while (i < buf_size - 1) {  
        if (RXFlag == 1) {
            if (received_char == '\r' || received_char == '\n') {  // Stop if KIM sends CR/LF
                break;
            }

            input[i++] = received_char;  // Store character in buffer
            RXFlag = 0;  
        }
    }

    input[i] = '\0';  
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    if (U1STAbits.OERR) {  // Check for Overrun Error
        U1STAbits.OERR = 0;  // Clear the error
    }

    while (U1STAbits.URXDA) {  // While data is available in the buffer
        received_char = U1RXREG;  // Read the received character from UART
        RXFlag = 1;  // Set flag to indicate new data
    }

    IFS0bits.U1RXIF = 0;  
}


