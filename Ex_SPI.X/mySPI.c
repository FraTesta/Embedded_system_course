/*
 * File:   mySPI.c
 * Author: francesco testa
 *
 * Created on 24 ottobre 2020, 12.09
 */

#include "mySPI.h"

void spi_config(){
    SPI1CONbits.MSTEN = 1; //enable Master mode
    SPI1CONbits.MODE16 = 0; // set 8-bit mode 
    // remeber that SPIxBUF is a 16 bit register as all other registers of the uC So I setted the following prescaler:
    SPI1CONbits.PPRE = 3; // 1:1
    SPI1CONbits.SPRE = 6; // 2:1
    SPI1STATbits.SPIEN = 1; // enable SPI protocol
    
}

void spi_put_char(char word){
    while(SPI1STATbits.SPITBF == 1); // loop to check if the previous character has been sent
    SPI1BUF = word;
}

void spi_put_string(char *str){
    int i;
    for ( i = 0; str[i] != '\0' ; i++){
        while(SPI1STATbits.SPITBF == 1);
        SPI1BUF = str[i];
    }
}