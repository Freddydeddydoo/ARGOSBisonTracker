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

#ifndef KIM_H
#define KIM_H


// -------------------------------------------------------------------------- //
// Includes
// -------------------------------------------------------------------------- //
#include <xc.h> 
#include <p24F16KA101.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart1.h"

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

// -------------------------------------- Function specific for sending AT commands to the KIM --------------------------- //

RetStatusKIMTypeDef send_ATCommand();

// ----------------------------------------------------------------------------------------------------------------------- //

uint8_t response[20];                       //response
uint8_t command[70];                    //
uint8_t pin_onOFF_used = 0;             //not used == 0 , used == 1  

// -------------------------------------- AT COMMANDS ------------------------------------------------------------------ //

//This syntax is used to be compatible with old versions of gcc
const uint8_t AT_ID[6] 		= {'A','T','+','I','D','='};					//!< "AT+ID="
const uint8_t AT_SN[6] 		= {'A','T','+','S','N','='};					//!< "AT+SN="
const uint8_t AT_SW[6] 		= {'A','T','+','S','W','='};					//!< "AT+SW="
const uint8_t AT_PWR[7] 		= {'A','T','+','P','W','R','='};			//!< "AT+PWR="
const uint8_t AT_BAND[8] 		= {'A','T','+','B','A','N','D','='};		//!< "AT+BAND="
const uint8_t AT_FRQ[7] 		= {'A','T','+','F','R','Q','='};			//!< "AT+FRQ="
const uint8_t AT_FW[6] 		= {'A','T','+','F','W','='};					//!< "AT+FW="
const uint8_t AT_TCXOWU[10] 	= {'A','T','+','T','C','X','O','W','U','='};//!< "AT+TCXOWU="		
const uint8_t AT_TX[6] 		= {'A','T','+','T','X','='};					//!< "AT+TX="
const uint8_t AT_REQUEST[3] 	= {'?','\r','\0'};							//!< "?\r\0"

// ----------------------------------------------------------------------------------------------------------------------- //


// -------------------------------------- 'CONSTRUCTORS' ------------------------------------------------------------------- //


//Constructors by name: these values will initialize the pins for the KIM module
// default constructor
void KIM_Init();    

//constructor with specific on/off config
void KIM_Init(uint8_t On_OFF_used); 

// ----------------------------------------------------------------------------------------------------------------------- //

// -------------------------------------- ON/OFF CHECKS ------------------------------------------------------------------ //


//checks if on/off used. returns 0 if true else 1
uint8_t check();


//sets the status of the on/off pin returns 0 if successful 1 otherwise
uint8_t setSleepMode(uint8_t mode);

uint8_t getSleepMode();


// -------------------------------------- Getters/Setters for KIM Module ------------------------------------------------------------------ //

//from the ArduinoKIM github
/** 
 *	"Get functions" are used to read the KIM1 module configuration with AT 
 *	Commands.
 *	All these functions have the same construction.  
 *	An AT command is sent through the serial port to ask the chosen parameter.
 *	The answer is returned.
 * 
 *		get_ID();		To get the ID value
 *		return "+ID:28,7AC8998"
 *		get_PWR();		To get the transmition power value
 *		return "+PWR:750"
 *  
 */

uint8_t get_ID();
uint8_t get_SN();
uint8_t get_FW();
uint8_t get_PWR();
uint8_t get_BAND();
uint8_t get_FRQ();
uint8_t get_TCXOWU();


/** 
 * ~~~ Taken from ArduinoKim ~~~
 *	"Set functions" are used to configure the KIM1 module with AT Commands.
 *	All these functions have the same construction.  
 *	An AT command is sent through the serial port to set a value to the
 *	chosen parameter. RetStatusKIMTypeDef is returned, see RetStatusKIMTypeDef
 * 	details.
 *
 *
 *		set_BAND("B1", 2);		To set the frequency band on B1
 *		set_PWR("1000", 4);		To transmit at 1000 mW
 *		set_PWR("750", 3);		To transmit at 750 mW
 */

RetStatusKIMTypeDef set_ID(uint8_t ID[], uint8_t len);
RetStatusKIMTypeDef set_SN(uint8_t SN[], uint8_t len);
RetStatusKIMTypeDef set_PWR(uint8_t PWR[], uint8_t len);
RetStatusKIMTypeDef set_BAND(uint8_t BAND[], uint8_t len = 2);
RetStatusKIMTypeDef set_FRQ(uint8_t FRQ[], uint8_t len);
RetStatusKIMTypeDef set_TCXOWU(uint8_t TCXOWU[], uint8_t len);

		
RetStatusKIMTypeDef send_data(char data[], uint8_t len);
#endif	/* KIM_H */
