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
    ON_OFF_KIM_PIN = 0; //module initially turned off 
    
    RX_KIM_TRIS = 1;
    TX_KIM_TRIS = 0; 
}



