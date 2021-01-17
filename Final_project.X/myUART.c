/*
 * File:   myUART.c
 * Author: francesco testa
 *
 * Created on 18 dicembre 2020, 18.41
 */


#include "myUART.h"

void UART_config(int port) {
    switch (port) {
        case 1:
            U2BRG = 11; //    (1843200)/(16*(9600))  set the bound rate (9600) of transmission
            U2MODEbits.UARTEN = 1; //enable UART module 
            U2STAbits.UTXEN = 1; // enable transmission 
            break;
        case 2:
            U1BRG = 11; //    (1843200)/(16*(9600))  set the bound rate (9600) of transmission
            U1MODEbits.UARTEN = 1; //enable UART module 
            U1STAbits.UTXEN = 1; // enable transmission 
            break;
    }

}

// sand a string through UART2

void send_string_UART2(char* msg) {
    if (U2STAbits.UTXBF == 0) { // If buffer is not full

        int i;
        for (i = 0; i < strlen(msg) + 1; i++) {
            U2TXREG = msg[i];
        }
    }
    while (U2STAbits.UTXBF == 1) {
        } // Wait for space in buffer
    
    U2STAbits.OERR = 0; // Reset buffer overrun error

}

