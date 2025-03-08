/* 
 * File:   main.c
 * Author: Fardin Mahid
 *
 * Created on January 24, 2025, 7:10 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <p24F16KA101.h>
#include <xc.h>
#include <libpic30.h>
#include "Kim.h"
#include "clkChange.h"
#include "uart.h"
#include "uart1.h"

#define SLEEP_TIME 90000

/*
 * 
 */

//sending data:
//UartInit2 for user readablility. will not be used in final product.
const char data[] = "ABCDEF1234567";

//TODO Get rid of Disp2String() in final build. Just used for testing 
int main(int argc, char** argv) {
    //setup UART, KIM and CLK
    //we will have both of them be 4800 baud to keep it consistent.
    
    AD1PCFG = 0xFFFF;// set so ALL pins are digital.
    
    if (check()) {
        Disp2String("KIM Connection: OK\n");
    } else {
        Disp2String("KIM Connection: FAILED\n");
    }
    InitUART2();
    InitUART1();
    KIM_Init();
    newClk(12); 
    
    //getters to check if it actually works.
    while(1){
        
        Disp2String("KIM -- Sending Data\n"); 
        if(send_data(data, sizeof(data)-1) == OK_KIM){
           Disp2String("Message Sent!\n"); 
        }
        else{
           Disp2String("Error!\n");  
        }
        Disp2String("KIM -- Turn OFF\n");
        
//      Will be needed later to put the KIM to sleep for whatever reason. 
//        setSleepMode(1);                        //set sleep mode
//        __delay_ms(SLEEP_TIME);                 // delay stuff
//        setSleepMode(0);                        //reset sleep mode to clear buffer 
        
    }
    
    return (EXIT_SUCCESS);
}

