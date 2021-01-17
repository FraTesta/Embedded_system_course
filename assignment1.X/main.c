/*
 * File:   main.c
 * Author: francesco testa
 *
 * Created on 31 ottobre 2020, 12.44
 */


#include "xc.h"
#include "stdio.h"
#include "mySPI.h"
#include "timerFunc.h"

void uart_config(){
    U2BRG = 11; //    (1843200)/(16*(9600))  set the bound rate (9600) of transmission
    U2MODEbits.UARTEN = 1; //enable UART module 
    U2STAbits.UTXEN = 1; // enable transmission 
    
    U2STAbits.URXISEL = 3; // set the full recivedbuffer interrupt mode 
    IEC1bits.U2RXIE = 1; // enable the interrupt 
}

void spi_clean_LCD(){
    spi_put_char(0x80);
    int i;
    for (i = 0; i < 16; i++ ){ 
        spi_put_char(' '); 
    }
}

void spi_clean_LCD_secondRow(){
    spi_put_char(0xC0);
    int i;
    for (i = 0; i < 16; i++ ){  
        spi_put_char(' '); 
    }
}


void __attribute__((__interrupt__,__auto_psv__)) _U2RXInterrupt() // for reset the overflow 
{
    IFS1bits.U2RXIF = 0; // put down the flag interrupt
    U2STAbits.OERR = 0; 
}


int main(void) {
    
    char c;
    char prevC = ' ';
    int LCDcursor = 0; // LCD cursor position counter
    int charCounter = 0; // counts the number of character sent
    char secondRow[16];
    int firstRowIndex[] = {0x80 , 0x81, 0x82 , 0x83, 0x84, 0x85, 0x86 , 0x87, 0x88, 0x89 , 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F};
    
    TRISEbits.TRISE8 = 1; // S5 in read mode
    TRISDbits.TRISD0 = 1; // S6 in read mode 
    
    int prevButtonStatusS5 = PORTEbits.RE8; 
    int currentButtonStatusS5;
    int prevButtonStatusS6 = PORTEbits.RE8; 
    int currentButtonStatusS6;
    
    tmr_wait_ms(TIMER1,1000); // necessary to initialize SPI
    spi_config();
    spi_clean_LCD();
    uart_config();
    
    while(1){
    
        currentButtonStatusS5 = PORTEbits.RE8; 
        currentButtonStatusS6 = PORTEbits.RE8; 
        
        if(currentButtonStatusS5 == 0 && prevButtonStatusS5 == 1){ // check if S5 is pressed
            while( U2STAbits.UTXBF == 1); 
            U2TXREG = charCounter;
        }
        
        if(currentButtonStatusS6 == 0 && prevButtonStatusS6 == 1){ // check if S6 is pressed
        // SECOND ROW MESSAGE       
        spi_clean_LCD_secondRow(); // clean the second row
        spi_put_char(0x80);
        charCounter = 0;        
        }
        
        while( U2STAbits.URXDA == 1){ // check message from UART2
        c = U2RXREG;
         
        if(((c == 'n') || (c == 'r')) && (prevC == '\\')) // check \n or \r characters
        {
            spi_clean_LCD(); 
            LCDcursor = 0;
        }else{
              if(LCDcursor == 15){ // end of row control
                 spi_put_char(0x80); 
                 LCDcursor = 0;  
                 }
          
              spi_put_char(firstRowIndex[LCDcursor]); // in order to be sure that we are writing in the correct position
                                                      // after some row changes 
              spi_put_char(c);
              charCounter++; 
              prevC = c;
              LCDcursor++;
              prevButtonStatusS5 = currentButtonStatusS5;
              prevButtonStatusS6 = currentButtonStatusS6;
            }
        }
    }    
    return 0;
}

