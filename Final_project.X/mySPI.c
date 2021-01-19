/*
 * File:   mySPI.c
 * Author: francesco testa
 *
 * Created on 24 ottobre 2020, 12.09
 */

#include "mySPI.h"

void spi_config() {
    SPI1CONbits.MSTEN = 1; //enable Master mode
    SPI1CONbits.MODE16 = 0; // set 8-bit mode 
    // remeber that SPIxBUF is a 16 bit register as all other registers of the uC So:
    SPI1CONbits.PPRE = 3; // 1:1
    SPI1CONbits.SPRE = 6; // 2:1
    SPI1STATbits.SPIEN = 1; // enable SPI protocol

}

void spi_put_char(char word) {
    while (SPI1STATbits.SPITBF == 1); // loop to check if the previous character has been sent
    SPI1BUF = word;
}

void spi_put_string(char *str, int position) {
    int i;
    // set cursor position 
    while (SPI1STATbits.SPITBF == 1); // wait until not full
    SPI1BUF = position;
    //send string
    for (i = 0; str[i] != '\0'; i++) {
        while (SPI1STATbits.SPITBF == 1);
        SPI1BUF = str[i];
    }
}

void spi_clean_LCD_firstRow() {
    spi_put_char(0x80);
    int i;
    for (i = 0; i < 16; i++) {
        spi_put_char(' ');
    }
}

void spi_clean_LCD_secondRow() {
    spi_put_char(0xC0);
    int i;
    for (i = 0; i < 16; i++) {
        spi_put_char(' ');
    }
}

void spi_clean_LCD() {
    spi_put_char(0x80);
    int i;
    for (i = 0; i < 16; i++) {
        spi_put_char(' ');
    }
        spi_put_char(0xC0);

    for (i = 0; i < 16; i++) {
        spi_put_char(' ');
    }
}

//per velocizzare l'invio di ogni msg 
void setTags1() {
    spi_put_string("ST:", FIRST_ROW);
    spi_put_string("T:", TEMP_LCD_POSIT);
    spi_put_string("R:", SECOND_ROW);
}

void setTags2() {
    spi_put_string("SA:", FIRST_ROW);
    spi_put_string("R:", SECOND_ROW);
}