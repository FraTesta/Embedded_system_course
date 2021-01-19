#ifndef XC_HEADER_UART_H
#define	XC_HEADER_UART_H

#include <xc.h> 

#define UART1 1
#define UART2 2

void UART_config(int port);
// sand a string through UART2
void send_string_UART2(char* msg);

#endif	/* UART_H */
