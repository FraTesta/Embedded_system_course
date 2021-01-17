/*
 * File:   main.c
 * Author: francesco testa
 *
 * Created on 24 ottobre 2020, 10.55
 */



#include "xc.h"
#include "stdio.h" 
#include "timerFunc.h"
#include "mySPI.h"

void spi_clean_LCD(){
    spi_put_char(0x80);
    int i;
    for (i = 0; i < 16; i++ ){ // remeber that SPIxBUF is 16 bits register 
        spi_put_char(' '); // I write the space character to clean the LCD 
    }
}

// write HELLO WORLD n LCD 
void ex1(){
    tmr_wait_ms(TIMER1,1000); // must do every time we reset the operation SPI
    
    spi_config(); // configure the SPI protocol , also with a secondary prescaler of 2:1
    
    spi_put_string("HALLO WORLD"); // function with sending a string in 
    
    while(1);
}

// print on LCD the number of seconds passed 
void ex2(){
    tmr_wait_ms(TIMER1,1000); // must do every time we reset the operation SPI
    spi_config();
    
    int count = 0;
    char seconds[16];
    
    tmr_setup_period(TIMER2,1000);
    
    while(1){
        
    tmr_wait_period(TIMER2);
    spi_put_char(0x80); // per andare a capo 
    count++;
    sprintf(seconds, "sec = %d", count);
    spi_put_string(seconds);
    
    }
}

//improve the previous ex just by introducing a button that reset the counter every time 
// is pressed 
void ex3(){
    
    TRISEbits.TRISE8 = 1; // set the pin button in read mode
    
    tmr_wait_ms(TIMER1,1000); // must do every time we reset the operation f SPI
    spi_config();
    
    int count = 0;
    char seconds[16];
    
    tmr_setup_period(TIMER2,1000);
    int prevButtonStatus = PORTEbits.RE8; // read the status of the button in order to initialize it 
    int currentButtonStatus;
    
    while(1){
    
    currentButtonStatus = PORTEbits.RE8; 
        
    if(currentButtonStatus == 0 && prevButtonStatus == 2){
        count = 0;
        
    }
    prevButtonStatus = currentButtonStatus;
    tmr_wait_period(TIMER2);
    spi_put_char(0x80); // per andare a capo 
    count++;
    sprintf(seconds, "sec = %d", count);
    spi_clean_LCD(); // to clean the LCD display
    spi_put_string(seconds);
    
    }
}



int main(void) {
    
    ex2();
    return 0;
}

