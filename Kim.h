/* 
 * File:   Kim.h
 * Author: Fardin Mahid, Capstone Group ARGOS Bison Tracker 2024
 * 
 * Handles Kim communication through UART to 
 * PIC 24 F 16 KA101 microcontroller. 
 * 
 * Heavily References Kines ArduinoKim open source library:
 * https://github.com/Kineis/ArduinoKim
 * 
 * Created on January 27, 2025
 */

//master clear
#ifndef KIM_H
#define KIM_H
#include <stdbool.h>

#if defined(__PIC24F16KA101__)   //< PIC24F16KA101

// Define ON/OFF control pin for KIM module
#define ON_OFF_KIM_PIN  LATBbits.LATB0  // RB0 output
#define ON_OFF_KIM_TRIS TRISBbits.TRISB0 // Set RB0 as output

// UART Communication Pins
#define RX_KIM          PORTBbits.RB2  // Read from RB2
#define TX_KIM          LATBbits.LATB7  // Write to RB7

// TRIS Register for setting TX/RX as input/output
#define RX_KIM_TRIS     TRISBbits.TRISB2 // Set RB2 as input
#define TX_KIM_TRIS     TRISBbits.TRISB7 // Set RB7 as output


#endif

typedef enum {
	ERROR_KIM           	= 0x00,    	//!< Error returned by KIM
	OK_KIM              	= 0x01,   	//!< OK return by KIM
	UNKNOWN_ERROR_KIM   	= 0x02,   	//!< Something unknown returned by KIM
	TIMEOUT_KIM   			= 0x04   	//!< Timeout, nothing returned by KIM
} RetStatusKIMTypeDef;

//now to convert the class



#endif	/* KIM_H */
