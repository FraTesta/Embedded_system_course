// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_MY_SPI_LCD_H
#define	XC_HEADER_MY_SPI_LCD_H

#define FIRST_ROW 0x80
#define SECOND_ROW 0xC0
#define TEMP_LCD_POSIT 0x87

#include <xc.h> // include processor files - each processor file is guarded.  

void spi_config();
void spi_put_char(char word);
void spi_put_string(char *str, int position);
void spi_clean_LCD_firstRow();
void spi_clean_LCD_secondRow();
void spi_clean_LCD();

void setTags1();
void setTags2();

#endif /* __cplusplus */