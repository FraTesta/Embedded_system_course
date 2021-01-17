/*
 * File:   timerDelay.c
 * Author: francesco testa
 *
 * Created on 12 ottobre 2020, 10.54
 */


#include "xc.h"
#include "timerDelay.h"

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

void tmr_setup_period(int timer, int ms)
{
    float rate_in_ms = (float)ms/1000; // set 
    // the maximum number of PRx register is 65535
    // if ms = 5000 (in question 3) ->  5000/1000 = 5 
    // -> 1843200 * 5 = 9216000 So i could use a prescale of 16 -> 9216000 / 16 = 576000 but it is over
    // so I m gonna chose 64 bits of precale 
    
    switch(timer){
            case 1:
                T1CONbits.TON = 0; //disable the timer 1
                TMR1 = 0; // clean all register of the timer 1
                T1CONbits.TCS = 0; // set that i don't wanna use TCS functionality
                T1CONbits.TCKPS = 2; // because if look at the manual, 64 bits corresponds to set the register to 10 
                                     //binary so basically in decimal it s gonna be 2 
                PR1 = 1843200*rate_in_ms/64; // FCY * ms(parsed in sec) * prescale
                T1CONbits.TON = 1;
                break;
                
                
            case 2:
                T2CONbits.TON = 0;
                TMR2 = 0;
                T1CONbits.TCS = 0;
                T2CONbits.TCKPS = 2;
                PR2 = 1843200*rate_in_ms/64;
                T2CONbits.TON = 1;
                break;
    }         
}

void tmr_wait_period(int timer){
    
    switch(timer){
            case 1:
                
                //while loop that waiting for the flag go up
                while (IFS0bits.T1IF == 0)
                {
                 
                }
        
                IFS0bits.T1IF = 0 ;// reset TxIF to 0 again 
                break;
            case 2:
                 while (IFS0bits.T2IF == 0)
                {
                 
                }
                IFS0bits.T2IF = 0 ;// reset TxIF to 0 again 
                break;
    }         
}
