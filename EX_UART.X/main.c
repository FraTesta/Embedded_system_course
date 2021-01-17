/*
 * File:   main.c
 * Author: francesco testa
 *
 * Created on 30 ottobre 2020, 15.21
 */


#include "xc.h"
#include "mySPI.h"
#include "stdio.h"
#include "timerFunc.h"

void uart_config(){
    U2BRG = 11; //    (1843200)/(16*(9600))  set the bound rate (9600) of transmission
    U2MODEbits.UARTEN = 1; //enable UART module 
    U2STAbits.UTXEN = 1; // enable transmission 
}

int ex1() { // send a charatter from the UART to SPI
    // for simulation remeber to read how to do it on the notes
    char word;
    
    tmr_wait_ms(TIMER1,1000); // to do every time we reset SPI
    
    spi_config();
    uart_config();
    
    
    while(1){
        if( U2STAbits.URXDA == 1){// nofies if there are characters to be read
            word = U2RXREG; // read the character from the RX register 
            
            spi_put_char(word); // send to SPI LCD
        }
    }
    return 0;
}

int ex2(){ // read a character from UART2 and send it to UART2
    char word;
    tmr_wait_ms(TIMER1,1000);
    
    spi_config();
    uart_config();
    
    while(1){
        if( U2STAbits.URXDA == 1){// nofies if there are characters to be read
            word = U2RXREG; // read the character from the RX register 
            U2TXREG = word; // I put the same message to the TX register in order to send it 
        }
    }
    
}

int ex3(){ // read a character from UART2 and send it to UART2 and SPI
    char word;
    tmr_wait_ms(TIMER1,1000);
    
    spi_config();
    uart_config();
    
    while(1){
        if( U2STAbits.URXDA == 1){// nofies if there are characters to be read
            word = U2RXREG; // read the character from the RX register 
            
            // ATT Here is very important to put what i m reading in a variable 
            // Because otherwise when for ex. I write U2TXREG = U2RXREG the register U2RXREG
            // might be changed from the Uc, So it' always better put what I m reading in a variable.
            
            spi_put_char(word);
            U2TXREG = word; // I put the same message to the TX register in order to send it 
        }
        
        // NB If I wanna send a string I should put a timer here of type tmr_wait_period
    }
    
}

int ex3_improved(){// which check a possible overflow
    char word;
    tmr_wait_ms(TIMER1,1000);
    
    spi_config();
    uart_config();
    
    tmr_setup_period(TIMER2,10);
    
    while(1){
        tmr_wait_ms(TIMER1,1); // this becaouse we want that the uart talking at each second
        
        if(U2STAbits.OERR == 1){// I check a possible overflow
            while( U2STAbits.URXDA == 1){// nofies if there are characters to be read
                word = U2RXREG; // read the character from the RX register 
            
                spi_put_char(word);
                U2TXREG = word; // I put the same message to the TX register in order to send it 
            }
            U2STAbits.OERR = 0; // reset the register 
        
            // or We can avoid to put the "if" and put : while( U2STAbits.UTXBF == 1); before to send the message on UART
            // which attends untill the buffer is empty
            /*
                while( U2STAbits.URXDA == 1){
                word = U2RXREG; 
                while( U2STAbits.UTXBF == 1);
                spi_put_char(word);
                U2TXREG = word; 
            }
             
             */
            tmr_wait_period(TIMER2); // usefull when we send a string
    }
    
  return 0;  
}
int main(void) {
    ex1();
    //ex2();
    //ex3();
    return 0;
}
