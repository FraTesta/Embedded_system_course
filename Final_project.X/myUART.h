#ifndef XC_HEADER_UART_H
#define	XC_HEADER_UART_H

#include <xc.h> 

#define UART_1 1
#define UART_2 2

//void __attribute__((__interrupt__, __auto_psv__)) _U2RXInterrupt() 

void UART_config(int port);
// sand a string through UART2
void send_string_UART2(char* msg);
void msg_handler(char* msg_type, char* msg_payload);

#endif	/* UART_H */
