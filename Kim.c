/* 
 * File:   Kim.C
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

#include "Kim.h"

const uint8_t AT_ID[6]         = {'A','T','+','I','D','='};
const uint8_t AT_SN[6]         = {'A','T','+','S','N','='};
const uint8_t AT_SW[6]         = {'A','T','+','S','W','='};
const uint8_t AT_PWR[7]        = {'A','T','+','P','W','R','='};
const uint8_t AT_BAND[8]       = {'A','T','+','B','A','N','D','='};
const uint8_t AT_FRQ[7]        = {'A','T','+','F','R','Q','='};
const uint8_t AT_FW[6]         = {'A','T','+','F','W','='};
const uint8_t AT_TCXOWU[10]    = {'A','T','+','T','C','X','O','W','U','='};
const uint8_t AT_TX[6]         = {'A','T','+','T','X','='};
const uint8_t AT_REQUEST[3]    = {'?','\r','\0'};
uint8_t response[20] = {0};   
uint8_t command[70] = {0};
uint8_t pin_onOFF_used = 0;             //not used == 0 , used == 1  
uint8_t UartSwitch = 2; // an enviorment variable that will switch between uart functions. 1 for UART1, 2 for UART2

/*
//initialize the KIM Pins used for the machine
void KIM_Init(){
    ON_OFF_KIM_TRIS = 0; //RB0 outputting value to KIM (0 == output, 1 == input)
    ON_OFF_KIM_PIN = 0; //module initially turned off. 
    
    RX_KIM_TRIS = 1;
    TX_KIM_TRIS = 0; 
}
 */
	

void KIM_Init(uint8_t On_OFF_used) {
    ON_OFF_KIM_TRIS = 0; // TRISB0 = 0 ? Set as output
    pin_onOFF_used = On_OFF_used;
    if (On_OFF_used) {  
        // If On_OFF_used is 1, turn ON the KIM module
        ON_OFF_KIM_PIN = 1;  // LATB0 = 1
    } else {
        // If On_OFF_used is 0, turn OFF the KIM module
        ON_OFF_KIM_PIN = 0;  // LATB0 = 0
    }
    
    //set the UART Bits 
    RX_KIM_TRIS = 1;
    TX_KIM_TRIS = 0; 
}


uint8_t check(){
//    setSleepMode(1);
//    setSleepMode(0);
    Disp2String("Checking if KIM Successful: press enter after entering AT command\r\n");
    char tmp = get_ID(); //if success, expected response is :: +ID:xx
    
    if((tmp) == '+'){
        return 1;
    }
    else{
        return 0;
    }
    
 
}



//mode == will be like a boolean
uint8_t setSleepMode(uint8_t mode){
    uint8_t bufferdata = 0;
    if(pin_onOFF_used){
        if(getSleepMode()){
            return 1;
        }
        else{
            if(mode == 0){
                bufferdata = 0;
                for(int i = 0; i < 2; i++){
                    uint8_t tmp[20] = {0};
                    bufferdata = 0;
                    recv_KIM(tmp, 20);

                    
                    for (int j = 0; j < 20; j++) {
                        bufferdata += tmp[j];
                    }

                    
                    if(bufferdata != 0){
                        return 1;
                    }
                }
                return 0;
            }   
        }
    }else{
        return (mode == 1) ? 0 : 1; //todo. Need to understand how this works so that we can put it to sleep in the final product.
                                    //We need to figure out how to turn off UART1 I'm not sure If its just UART1.UEN = 0 I don't even 
                                    // know if that will save any battery? We might not even need sleepmode tbh Something to discuss
    }
    return 1;
}


uint8_t getSleepMode(){
    if(pin_onOFF_used){
        if(ON_OFF_KIM_PIN){
            return 0;
        }
        else{
            return 1;
        }
    }
    else{
        return 0;
    }
}

// IMPORTANT FUNCTION

RetStatusKIMTypeDef send_ATCommand() {
    // Clear UART buffers

    response[0] = '\0';

    // Send command
    if(UartSwitch == 1){
        sendto_KIM(command);
    }
    else{
        Disp2String(command);
    }

    // Wait and receive response
    for (uint8_t i = 0; i < 10; i++) {
        if(UartSwitch == 1){
           recv_KIM(response, 20); // Receive UART data
        }
        else{
            RecvUart(response, 20);
        }
        if ((response[0] != '\0') && response[2] == 'X') {
            return OK_KIM;
        }
    }

    return TIMEOUT_KIM;
}


//--------------------------------------------------------------//
/*
 * FROM KINEIS INTEGRATION MANUAL v2.0
 *  The AT command must be a sentence terminated by <CR>, while the response will be a sentence
 * terminated by <CR><LF>.
 * Ergo send data with '\r' terminator.
 */
//--------------------------------------------------------------//
RetStatusKIMTypeDef send_data(char data[], uint8_t len){
    
    for(uint8_t i = 0; i < 6; i++)
        command[i] = AT_TX[i]; 
    
    for(uint8_t i = 0; i < len; i++)
        command[6+i] = data[i]; 
    
    command[6+len] = '\r';
    command[7+len] = '\0';
    
    return send_ATCommand();
}


// -------------------------------------- Getters/Setters for KIM Module ------------------------------------------------------------------ //

// Function to get ID
uint8_t get_ID() {
    uint8_t i;
    for (i = 0; i < 6; i++)
        command[i] = AT_ID[i];
    for (i = 0; i < 3; i++)
        command[6 + i] = AT_REQUEST[i];
    send_ATCommand();
    return response;
}

// Function to get Serial Number
uint8_t get_SN() {
    uint8_t i;
    for (i = 0; i < 6; i++)
        command[i] = AT_SN[i];
    for (i = 0; i < 3; i++)
        command[6 + i] = AT_REQUEST[i];
    send_ATCommand();
    return response;
}

// Function to get Firmware Version
uint8_t get_FW() {
    uint8_t i;
    for (i = 0; i < 6; i++)
        command[i] = AT_FW[i];
    for (i = 0; i < 3; i++)
        command[6 + i] = AT_REQUEST[i];
    send_ATCommand();
    return response;
}

// Function to get Transmit Power
uint8_t get_PWR() {
    uint8_t i;
    for (i = 0; i < 7; i++)
        command[i] = AT_PWR[i];
    for (i = 0; i < 3; i++)
        command[7 + i] = AT_REQUEST[i];
    send_ATCommand();
    return response;
}

// Function to set ID
RetStatusKIMTypeDef set_ID(uint8_t ID[], uint8_t len) {
    uint8_t i;
    for (i = 0; i < 6; i++)
        command[i] = AT_ID[i];
    for (i = 0; i < len; i++)
        command[6 + i] = ID[i];
    command[6 + len] = '\r';
    command[7 + len] = '\0';
    return send_ATCommand();
}

// Function to set Serial Number
RetStatusKIMTypeDef set_SN(uint8_t SN[], uint8_t len) {
    uint8_t i;
    for (i = 0; i < 6; i++)
        command[i] = AT_SN[i];
    for (i = 0; i < len; i++)
        command[6 + i] = SN[i];
    command[6 + len] = '\r';
    command[7 + len] = '\0';
    return send_ATCommand();
}

// -------------------------------------- Getters/Setters for KIM Module ------------------------------------------------------------------ //




