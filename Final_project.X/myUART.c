/*
 * File:   myUART.c
 * Author: francesco testa
 *
 * Created on 18 dicembre 2020, 18.41
 */

#include <string.h>
#include <stdio.h>
#include "myUART.h"
#include "myPWM.h"
#include "xc.h"
#include "myBuffer.h"
#include "timerFunc.h"
#include "buttons.h"
#include "global_&_define.h"

void __attribute__((__interrupt__, __auto_psv__)) _U2RXInterrupt() {
    IFS1bits.U2RXIF = 0; // Reset rx interrupt flag
    int val = U2RXREG; // Read from rx register
    writeBuf(&UARTbuf, val); // Save value in buffer
}

void UART_config(int port) {
    switch (port) {
        case UART_1:
            U1BRG = 11; //    (1843200)/(16*(9600))  set the bound rate (9600) of transmission
            U1MODEbits.UARTEN = 1; //enable UART module 
            U1STAbits.UTXEN = 1; // enable transmission 

            break;
        case UART_2:
            U2BRG = 11; //    (1843200)/(16*(9600))  set the bound rate (9600) of transmission
            U2MODEbits.UARTEN = 1; //enable UART module 
            U2STAbits.UTXEN = 1; // enable transmission 
            U2STAbits.URXISEL = 2; // 3/4 UART Buffer generates interrupt 
            IEC1bits.U2RXIE = 1; // Enable rx interrupt for UART

            break;

    }

}

// sand a string through UART2

void send_string_UART2(char* msg) {
    // If buffer is not full

    int i;
    for (i = 0; i < strlen(msg) + 1; i++) {
        if (U2STAbits.UTXBF == 0) {
            U2TXREG = msg[i];
        }
    }
    while (U2STAbits.UTXBF == 1) {
    } // Wait for space in buffer

    U2STAbits.OERR = 0; // Reset buffer overrun error

}



