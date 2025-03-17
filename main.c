/* 
 * File:   main.c
 * Author: Fardin Mahid
 *
 * Created on January 24, 2025, 7:10 AM
 */

// FBS
#pragma config BWRP = OFF               // Table Write Protect Boot (Boot segment may be written)
#pragma config BSS = OFF                // Boot segment Protect (No boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Code Flash Write Protection bit (General segment may be written)
#pragma config GCP = OFF                // General Segment Code Flash Code Protection bit (No protection)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC oscillator (FRC))
#pragma config IESO = OFF               // Internal External Switch Over bit (Internal External Switchover mode disabled (Two-Speed Start-up disabled))

// FOSC
#pragma config POSCMOD = NONE           // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = ON            // CLKO Enable Configuration bit (CLKO output disabled; pin functions as port I/O)
#pragma config POSCFREQ = HS            // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency greater than 8 MHz)
#pragma config SOSCSEL = SOSCHP         // SOSC Power Selection Configuration bits (Secondary oscillator configured for high-power operation)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor Selection (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (WDT prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected; windowed WDT disabled)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))

// FPOR
#pragma config BOREN = BOR3             // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware; SBOREN bit disabled)
#pragma config PWRTEN = ON              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Default location for SCL1/SDA1 pins)
#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (1.8V))
#pragma config MCLRE = ON               // MCLR Pin Enable bit (MCLR pin enabled; RA5 input pin disabled)

// FICD
#pragma config ICS = PGx2               // ICD Pin Placement Select bits (PGC2/PGD2 are used for programming and debugging the device)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses LPRC as reference clock)
#pragma config RTCOSC = SOSC            // RTCC Reference Clock Select bit (RTCC uses SOSC as reference clock)
#pragma config DSBOREN = ON             // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = ON             // Deep Sleep Watchdog Timer Enable bit (DSWDT enabled)



#include <stdio.h>
#include <stdlib.h>
#include <p24F16KA101.h>
#include <xc.h>
//#include <libpic30.h>
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
char data[] = "ABCDEF1234567";

//TODO Get rid of Disp2String() in final build. Just used for testing 

///int argc, char** argv
int main() {
    //setup UART, KIM and CLK
    //we will have both of them be 4800 baud to keep it consistent.
    
    AD1PCFG = 0xFFFF;// set so ALL pins are digital.
    newClk(500); 
    
    InitUART2();
    InitUART1();
    KIM_Init(0);

    
    if (check()) {
        Disp2String("KIM Connection: OK\n");
    } else {
        Disp2String("KIM Connection: FAILED\n");
        return (EXIT_SUCCESS);
    }
    
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
        
        for(int i = 0; i < SLEEP_TIME; i++){}
//        setSleepMode(0);                        //reset sleep mode to clear buffer 
        
    }
    
    return (EXIT_SUCCESS);
}

