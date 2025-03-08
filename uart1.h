/* 
 * File:   uart1.h
 * Author: Fardin Mahid
 *
 * Created on March 8, 2025, 5:00 AM
 */


#include <xc.h> 
#include <p24F16KA101.h>
#include <stdio.h>
#include <stdlib.h>

//UART functions to send to kim
void InitUART1(void);
void sendto_KIM(char* str);
void XmitUART1(char CharNum, unsigned int repeatNo);
void recv_KIM(char* input, uint8_t buf_size);

#ifndef UART1_H
#define	UART1_H



#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* UART1_H */

