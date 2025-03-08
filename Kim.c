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


//initialize the KIM Pins used for the machine
void KIM_Init(){
    ON_OFF_KIM_TRIS = 0; //RB0 outputting value to KIM (0 == output, 1 == input)
    ON_OFF_KIM_PIN = 0; //module initially turned off. 
    
    RX_KIM_TRIS = 1;
    TX_KIM_TRIS = 0; 
}


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
    
    //set the UART Bits TODO make sure all the pheripherals are set in UART INIT As well
    RX_KIM_TRIS = 1;
    TX_KIM_TRIS = 0; 
}

//TODO (do get/set sleepmode first)
uint8_t check(){
    setSleepMode(1);
    setSleepMode(0);
    
    char* tmp = get_ID(); //if success, expected response is :: +ID:xx
    
    if(*(tmp) == '+'){
        return 1;
    }
    else{
        return 0;
    }
    
 
}


//TODO (getsleepmode first)
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
                    RecvUart(tmp, 20);

                    
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
        return (mode == 1) ? 0 : 1;
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
    Disp2String(command);

    // Wait and receive response
    for (uint8_t i = 0; i < 10; i++) {
        RecvUart(response, 20); // Receive UART data

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
uint8_t* get_ID() {
    uint8_t i;
    for (i = 0; i < 6; i++)
        command[i] = AT_ID[i];
    for (i = 0; i < 3; i++)
        command[6 + i] = AT_REQUEST[i];
    send_ATCommand();
    return response;
}

// Function to get Serial Number
uint8_t* get_SN() {
    uint8_t i;
    for (i = 0; i < 6; i++)
        command[i] = AT_SN[i];
    for (i = 0; i < 3; i++)
        command[6 + i] = AT_REQUEST[i];
    send_ATCommand();
    return response;
}

// Function to get Firmware Version
uint8_t* get_FW() {
    uint8_t i;
    for (i = 0; i < 6; i++)
        command[i] = AT_FW[i];
    for (i = 0; i < 3; i++)
        command[6 + i] = AT_REQUEST[i];
    send_ATCommand();
    return response;
}

// Function to get Transmit Power
uint8_t* get_PWR() {
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




