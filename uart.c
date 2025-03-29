/*
 * File:   uart.c
 * Author: psbta
 *
 * Created on October 10, 2023, 4:10 PM
 */


#include "xc.h"
#include "uart.h"
#include "string.h"
#include <stdlib.h>

#define MAX_BAR_WIDTH 20
#define ADC_reso 1023


uint8_t received_char = 0;
uint8_t RXFlag = 0;
uint8_t TXFlag = 0;
// extern uint16_t CNflag; // uncomment if CNflag is implemented to break out of the busy wait for new input
uint8_t CN_event = 0;      
uint8_t TMR2_event = 0;   
uint8_t TMR3_event = 0;    


void InitUART2(void) 
{
	// configures UART2 module on pins RB0 (Tx) and RB1 (Rx) on PIC24F16KA101 
	// Enables UART2 
    
    
    
	U2MODEbits.USIDL = 0;	// Bit13 Continue in Idle
	U2MODEbits.IREN = 0;	// Bit12 No IR translation
	U2MODEbits.RTSMD = 0;	// Bit11 Flow Control Mode Mode
	U2MODEbits.UEN = 00;		// Bits8,9 TX,RX enabled, CTS,RTS not
	U2MODEbits.WAKE = 0;	// Bit7 No Wake up (since we don't sleep here)
	U2MODEbits.LPBACK = 0;	// Bit6 No Loop Back
	U2MODEbits.ABAUD = 0;	// Bit5 No Autobaud (would require sending '55')
	U2MODEbits.RXINV = 0;	// Bit4 Idle state is '1'
	U2MODEbits.BRGH = 1;	// Bit3 16 clocks per bit period
	U2MODEbits.PDSEL = 0;	// Bits1,2 8bit, No Parity
	U2MODEbits.STSEL = 0;	// Bit0 One Stop Bit
    
    if (OSCCONbits.COSC == 0b110)
	{
		U2BRG = 12;	// TARGET: 9600 baud @ 500 kHz FOSC
	}
	else if (OSCCONbits.COSC == 0b101)
	{
		U2BRG = 12;	// TARGET: 300 baud @ 32 kHz FOSC
	}
	else if (OSCCONbits.COSC == 0b000)
	{
		U2BRG = 0; //Baud rate is set to 1 million @ 8 MHz FOSC
        //U2BRG = 103;	// TARGET: 9600 baud @ 8 MHz FOSC
	}
 

    U2STAbits.UTXISEL1 = 0;	//Bit15 Int when Char is transferred (1/2 config!)
    U2STAbits.UTXISEL0 = 1;	//Generate interrupt with last character shifted out of U2TXREG buffer
	U2STAbits.UTXINV = 0;	//Bit14 N/A, IRDA config
	U2STAbits.UTXBRK = 0;	//Bit11 Disabled
	U2STAbits.UTXEN = 0;	//Bit10 TX pins controlled by periph
//	U2STAbits.UTXBF = 0;	//Bit9 *Read Only Bit*
//	U2STAbits.TRMT = 0;		//Bit8 *Read Only bit*
	U2STAbits.URXISEL = 0;	//Bits6,7 Int. on character recieved
	U2STAbits.ADDEN = 0;	//Bit5 Address Detect Disabled
//	U2STAbits.RIDLE = 0;	//Bit4 *Read Only Bit*
//	U2STAbits.PERR = 0;		//Bit3 *Read Only Bit*
//	U2STAbits.FERR = 0;		//Bit2 *Read Only Bit*
//	U2STAbits.OERR = 0;		//Bit1 *Read Only Bit*
//	U2STAbits.URXDA = 0;	//Bit0 *Read Only Bit*

	
    IFS1bits.U2TXIF = 0;	// Clear the Transmit Interrupt Flag
    IPC7bits.U2TXIP = 3; // UART2 TX interrupt has interrupt priority 3-4th highest priority
    
	IEC1bits.U2TXIE = 1;	// Enable Transmit Interrupts
	IFS1bits.U2RXIF = 0;	// Clear the Recieve Interrupt Flag
	IPC7bits.U2RXIP = 4;    //UART2 Rx interrupt has 2nd highest priority
    IEC1bits.U2RXIE = 1;	// Enable Receive Interrupts

	U2MODEbits.UARTEN = 1;	// And turn the peripheral on
    
//	U2STAbits.UTXEN = 1;
}


void Disp2String(char *str) //Displays String of characters
{
    unsigned int i;
    for (i=0; i<= strlen(str); i++)
    {
        XmitUART2(str[i],1);
    }

    return;
}

void XmitUART2(char CharNum, unsigned int repeatNo)
{	
	U2STAbits.UTXEN = 1; // We start the UART transmission
    
	while(repeatNo!=0) 
	{
		while(U2STAbits.UTXBF==1)   // We loop here because if the UART buffer is not 0, we do not write to it, so we loop indefinitely until we can.
		{
		}	
		U2TXREG=CharNum;            // We put CharNum here because this is what we want to transmit. Its in the transmit register.
		repeatNo--;                //
	}
	while(U2STAbits.TRMT==0)        // We check while the transmit shift register
	{                               // is not empty,and a transmission is in progress or queued, we cannot transmit so we loop indefinitely until we can.
	}

    U2STAbits.UTXEN = 0;
}

/************************************************************************
 * Receive a buf_size number of characters over UART
 * Description: This function allows you to receive buf_size number of characters from UART,
 * provided the UART module has been initialized correctly. The function currently returns
 * if the "enter" key (ASCII 0x0D) is received. The function does not handle receiving
 * the DELETE or BACKSPACE keys meaningfully. 
 * 
 * Note: there is commented-out skeleton code that could be (re)implemented to allow the function
 * return early without the ENTER key given an interrupt-set global flag. 
 ************************************************************************/
void RecvUart(char* input, uint8_t buf_size)
{	
    uint16_t i = 0;
    char last_char;
    // wait for enter key
    while (last_char != 0x0D) {
        if (RXFlag == 1) {
            // only store alphanumeric characters
            if (received_char >= 32 && received_char <= 126) {
                if (i > buf_size-2) {
                    Disp2String("\ntoo long\n\r");
                    RXFlag = 0;
                    return;
                }
                input[i] = received_char;
                i++;
                XmitUART2(received_char,1); // loop back display
                U2STAbits.OERR = 0;
            }
            last_char = received_char;
            RXFlag = 0;
        }
        // wait for next character
        
        /*
        
        if (CN_event | TMR3_event) { // this allows breaking out of the busy wait if CN interrupts are enabled...
            break;
         }
        */
        // if (CNflag == 1) { // this allows breaking out of the busy wait if CN interrupts are enabled...
        //     add logic here
        // }
    }
}

uint8_t received_char_UART1 = 0;



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

    // Set Baud Rate for KIM (9600 baud)
    if (OSCCONbits.COSC == 0b110)
    {
        U1BRG = 103; // 9600 baud @ 8 Mhz FOSC
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
    
}

void sendto_KIM(char* str){
    unsigned int i;
    for (i=0; i< strlen(str); i++)
    {
        XmitUART1(str[i],1);
    }

    return;
    
}

void XmitUART1(unsigned char CharNum, unsigned int repeatNo){
    
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
   
    while (1) {  
        if (RXFlag == 1) {
            if (received_char_UART1 == '\r' || received_char_UART1 == '\n') {  // Stop if KIM sends CR/LF
                break;
            }

            input[i++] = received_char_UART1;  // Store character in buffer
           RXFlag= 0;  
        }
    }

    input[i] = '\0';  
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    
    while (U1STAbits.URXDA) {  // While data is available in the buffer
        received_char_UART1 = U1RXREG;  // Read the received character from UART
       RXFlag= 1;  // Set flag to indicate new data
    }

    IFS0bits.U1RXIF = 0;  
}

//void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
//	IFS0bits.U1TXIF = 0;
//    
//    TXFlag = 1;
//
//}
/************************************************************************
 * Receive a single (alphanumeric) character over UART
 * Description: This function allows you to receive a single character, which will only be 
 * "accepted" (returned) after receiving the ENTER key (0x0D). 
 * While receiving characters, the program is designed to send back the received character.
 * To display this, it sends a BACKSPACE (0x08) to clear the previous character from the 
 * receiving terminal, before sending the new current character. 
 * 
 * Note: there is commented-out skeleton code that could be (re)implemented to allow the function
 * return early without the ENTER key given an interrupt-set global flag. 
 ************************************************************************/
char RecvUartChar(char last_char)
{	
    
    //char last_char = 'X';
    XmitUART2(last_char,1);
    // wait for enter key
    while (last_char != 0x0D) {
        if (RXFlag == 1) {
            
            // return the last character received if you see ENTER
            if (received_char == 0x0D) {
                RXFlag = 0;
                return last_char;
            }
            
            // only store alphanumeric characters
            if (received_char >= 32 && received_char <= 126) {
                XmitUART2(0x08,1); // send backspace
                last_char = received_char;
                XmitUART2(received_char,1); // loop back display
            }
           
            U2STAbits.OERR = 0;
            RXFlag = 0;
        }
        if (CN_event | TMR3_event) { // this allows breaking out of the busy wait if CN interrupts are enabled...
            break;
         }
        // if (CNflag == 1) { // this allows breaking out of the busy wait if CN interrupts are enabled...
        //     add logic here
        // }
    }
}

char RecvUartChar_NoEnter(char last_char)
{	
    
    //char last_char = 'X';
    XmitUART2(last_char,1);
    // wait for enter key
    while (1) {
        if (RXFlag == 1) {
            // Receive a char directly
            if (received_char >= 32 && received_char <= 126) {
                XmitUART2(0x08,1); // send backspace
                last_char = received_char;
                XmitUART2(received_char,1); // loop back display
                
                
                U2STAbits.OERR = 0;
                RXFlag = 0;
            }
           
            U2STAbits.OERR = 0;
            RXFlag = 0;
            
            return received_char;
        }
        if (TMR2_event | TMR3_event | CN_event) { // this allows breaking out of the busy wait if CN interrupts are enabled...
            break;
         }
        // if (CNflag == 1) { // this allows breaking out of the busy wait if CN interrupts are enabled...
        //     add logic here
        // }
    }
    //return;
}


//
//uint16_t display_bar_graph(uint16_t adc_value, uint16_t lastbarwidth, char which_char) {
//    
//    // Calculate the proportional width of the bar (0 to MAX_BAR_WIDTH)
//    uint16_t bar_width = (adc_value * MAX_BAR_WIDTH) / ADC_reso;
//
//    // Create a string to represent the bar
//    char bar[MAX_BAR_WIDTH + 1];  // +1 for the null-terminator
//    for (uint16_t i = 0; i < bar_width; i++) {
//        bar[i] = which_char;  // Fill the bar with '=' characters
//    }
//    bar[bar_width] = '\0';  // Null-terminate the string
//
//    // Display the bar graph using Disp2String
//    Disp2String(bar);  // Send the bar to UART
//    //Disp2String("\n");  // Optionally add a newline for clarity
//    
//    return bar_width; //returns the bar width that was used last
//}
//
//uint16_t display_dig(uint16_t adc_value, uint16_t lastbarwidth, uint8_t mode) {
//char display_buffer[20];  // Buffer for formatted string
//
//    // Check the mode and format the `adc_value` accordingly
//    if (mode == 0) {
//        // Display in decimal format
//        sprintf(display_buffer, "%u", adc_value);  // Format adc_value as unsigned decimal
//    } else if (mode == 1) {
//        // Display in hexadecimal format
//        sprintf(display_buffer, "0x%X", adc_value);  // Format adc_value as hexadecimal
//    } else {
//        // Invalid mode, return without displaying anything
//        return 0;
//    }
//
//    // Display the formatted string
//    Disp2String(display_buffer);
//
//    // Return the length of the display for use as `lastbarwidth` in subsequent calls. 
//    //This part is unused now that I am flashing the whole screen.
//    return (uint16_t)strlen(display_buffer);
//}




void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void) {

	IFS1bits.U2RXIF = 0;
    
    received_char = U2RXREG;
    
    RXFlag = 1;
    
    //LATBbits.LATB8 ^= 1;
}

//void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void) {
//	IFS1bits.U2TXIF = 0;
//    
//    TXFlag = 1;
//
//}
