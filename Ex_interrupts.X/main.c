/*
 * File:   main.c
 * Author: francesco testa
 *
 * Created on 15 ottobre 2020, 10.08
 */


#include "xc.h"
#include "timerFunc.h"

int ex1_mio(void)
{
    TRISBbits.TRISB0 = 0; //led D3
    TRISBbits.TRISB1 = 0; //led D4
    IEC0bits.INT0IE = 1; // enable the interrupt INT0 becaouse I didn't understand the text LOL
    
    while(1){ 
        
        LATBbits.LATB0 = !LATBbits.LATB0; // d3 on/off
        IFS0bits.INT0IF = 1; // Here I geneate the interrupt manually, in order to turn on the led D4 
                            // but in this way the iintrerrup is pretty useless
        tmr_wait_ms(TIMER1,250);
        IFS0bits.INT0IF = 1; // here again I turn off the led D4
        tmr_wait_ms(TIMER1,250);        
    }
    
    return 0;
}

int ex1_prof(void){
    TRISBbits.TRISB0 = 0; //led D3
    TRISBbits.TRISB1 = 0; //led D4
    
    tmr_setup_period(TIMER1,500); // timer for led D3
    tmr_setup_period(TIMER1,250); // timer for led D4
    
    IEC0bits.T2IE = 1; // in this case I use the interrupt number 2 which is basically 
                       // the interrupt of timer2 which is the timer that I m using for D4
                       // in this way I don't have to generate the interrupt manually, because TIMER2 do this 
    
    while(1){
    LATBbits.LATB0 = !LATBbits.LATB0; // d3 on/off
    tmr_wait_period(TIMER1);
    }

    return 0;
}

int ex2(void){
    TRISBbits.TRISB0 = 0; //led D3
    TRISBbits.TRISB1 = 0; //led D4
    
    TRISEbits.TRISE8 = 0; // buttorn S5
    
    tmr_setup_period(TIMER1,500);
    
    IEC0bits.INT0IE = 1; // Now the interrupt on the same port of the button is INT0 
    
    while(1){
    LATBbits.LATB0 = !LATBbits.LATB0; // d3 on/off
    tmr_wait_period(TIMER1);
    }
    
    
    
}

int main(void) {
    //ex1_mio();
    //ex1_prof();
    ex2();
    return 0;
}

void __attribute__((__interrupt__,__auto_psv__)) _INT0Interrupt() //interrupt setting for my ex 1 and ex2
{
    IFS0bits.INT0IF = 0; // put down the flag of the interrupt 
    LATBbits.LATB1 = !LATBbits.LATB1;
    // for the 2nd ex it is sufficent becouse when the button is pressed it generates an interrupt on INT0 which turn on the LED D4
    // But in practice there is the problem of bouncing of the button
    /*
     solution:
     *  tmr_setup_period(TIMER2,50);  // I want to stop the INT0 interrupt for 50 ms
     *  IECObits.INT0IE = 0;  // disable interrutp INT0
     *  IECObits.T2IE = 0;  // anable the timer2 interrupt 
     *  
     * THEN I have to create a routine for the interrupt of the timer 2
     * 
     * void __attribute__((__interrupt__,__auto_psv__)) _INT2Interrupt() //interrupt setting for ex 1 by prof
        
     *  {
     *   T2CONbits.TON = 0; // stop the timer 
     *   IFS0bits.T2IF = 0; // put down the flag interrupt timer2
     *   
     *   IECObits.INT0IE = 0; // put down the flag of INT0
     *   IECObits.INT0IE = 1; // and finnaly I anable it 
     * }
     */
    
}

void __attribute__((__interrupt__,__auto_psv__)) _INT2Interrupt() //interrupt setting for ex 1 by prof
{
    IFS0bits.T2IF = 0; // put down the flag interrupt
    LATBbits.LATB1 = !LATBbits.LATB1;
}