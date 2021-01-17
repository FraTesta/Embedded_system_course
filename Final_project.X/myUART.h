#ifndef UART_H
#define	UART_H

#include <xc.h> 
#include <string.h>

void UART_config(int port);
// sand a string through UART2
void send_string_UART2(char* msg);

#endif	/* UART_H */
