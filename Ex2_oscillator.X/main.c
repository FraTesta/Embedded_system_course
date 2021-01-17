/*
 * File:   main.c
 * Author: francesco testa
 *
 * Created on 9 ottobre 2020, 16.46
 */

#define TIMER1 1
#define TIMER2 2

#include "xc.h"

void tmr_wait_ms(int timer, int ms);

int main(void) {
    
    IFS0bits.T1IF = 0; 
    IFS0bits.T2IF = 0;
    
    // 
    TRISBbits.TRISB0 = 0;
    LATBbits.LATB0 = 1;
    tmr_wait_ms(TIMER2, 1000);
    LATBbits.LATB0 = 0;
    tmr_wait_ms(TIMER2, 5000);
    LATBbits.LATB0 = 1;
    tmr_wait_ms(TIMER2, 500);
    LATBbits.LATB0 = 0;
    
    while(1);
    
    return 0;
}

void tmr_wait_ms(int timer, int ms){
double rate_in_ms = (double)ms/1000;

switch(timer){
            case 1:
                T1CONbits.TON = 0; 
                TMR1 = 0;
                T1CONbits.TCS = 0;
                T1CONbits.TCKPS = 3; // because if look at the manual, 64 bits corresponds to set the register to 10 
                                     //binary so basically in decimal it s gonna be 2 
                PR1 = 1843200*rate_in_ms/256;
                T1CONbits.TON = 1;
                
                 //while loop waiting for the flag go up
                while (IFS0bits.T1IF == 0);
          
                
                 IFS0bits.T1IF = 0 ;// reset TxIF to 0 again 
                 T1CONbits.TON = 0; // otherwise another timer cannot start
                break;
                
                
            case 2:
                T2CONbits.TON = 0;
                TMR2 = 0;
                T2CONbits.TCS = 0;
                T2CONbits.TCKPS = 3;
                PR2 = 1843200*rate_in_ms/256;
                T2CONbits.TON = 1;
                
                 while (IFS0bits.T2IF == 0)
                {
                 
                }
                IFS0bits.T2IF = 0 ;// reset TxIF to 0 again 
                T2CONbits.TON = 0;
                break;
    }   


    
}