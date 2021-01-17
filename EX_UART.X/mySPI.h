// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#define TIMER1 1
#define TIMER2 2

#include <xc.h> // include processor files - each processor file is guarded.  

void spi_config();
void spi_put_char(char word);
void spi_put_string(char *str);

#endif /* __cplusplus */