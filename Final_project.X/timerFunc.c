/*
 * File:   timerFunc.c
 * Author: francesco testa
 *
 * Created on 15 ottobre 2020, 10.21
 */


#include "timerFunc.h"
#include "global_&_define.h"

void choose_prescaler(int ms, int *pr, int* tckps) {
    // Fosc = 7.3728 MHz
    // Fcy = 7.3728/4 = 1843200 Hz

    long ticks = 1843.2 * ms;
    if (ticks <= 65535) {
        // prescaler 1:1
        *pr = ticks;
        *tckps = 0;
        return;
    }

    ticks = ticks / 8;
    if (ticks <= 65535) {
        // prescaler 1:8
        *pr = ticks;
        *tckps = 1;
        return;
    }

    ticks = ticks / 8;
    if (ticks <= 65535) {
        // prescaler 1:64
        *pr = ticks;
        *tckps = 3;
        return;
    }

    ticks = ticks / 8;
    if (ticks <= 65535) {
        // prescaler 1:256
        *pr = ticks;
        *tckps = 3;
        return;
    }
}

void tmr_wait_ms(int timer, int ms) {
    double rate_in_ms = (double) ms / 1000;

    switch (timer) {
        case 1:
            T1CONbits.TON = 0;
            TMR1 = 0;
            T1CONbits.TCS = 0;
            T1CONbits.TCKPS = 3; // because if look at the manual, 64 bits corresponds to set the register to 10 
            //binary so basically in decimal it s gonna be 2 
            PR1 = 1843200 * rate_in_ms / 256;
            T1CONbits.TON = 1;

            //while loop waiting for the flag go up
            while (IFS0bits.T1IF == 0);


            IFS0bits.T1IF = 0; // reset TxIF to 0 again 
            T1CONbits.TON = 0; // otherwise another timer cannot start
            break;


        case 2:
            T2CONbits.TON = 0;
            TMR2 = 0;
            T2CONbits.TCS = 0;
            T2CONbits.TCKPS = 3;
            PR2 = 1843200 * rate_in_ms / 256;
            T2CONbits.TON = 1;

            while (IFS0bits.T2IF == 0) {

            }
            IFS0bits.T2IF = 0; // reset TxIF to 0 again 
            T2CONbits.TON = 0;
            break;
    }

}

void tmr_setup_period(int timer, int ms) {
    //float rate_in_ms = (float) ms / 1000; // set 
    // the maximum number of PRx register is 65535
    // if ms = 5000 (in question 3) ->  5000/1000 = 5 
    // -> 1843200 * 5 = 9216000 So i could use a prescale of 16 -> 9216000 / 16 = 576000 but it is over
    // so I m gonna chose 64 bits of precale 

    int pr;
    int tckps;

    switch (timer) {
        case TIMER1:
            T1CONbits.TON = 0; //disable the timer 1
            TMR1 = 0; // clean all register of the timer 1
            T1CONbits.TCS = 0; // set that i don't wanna use TCS functionality
            choose_prescaler(ms, &pr, &tckps);
            T1CONbits.TCKPS = tckps; // because if look at the manual, 64 bits corresponds to set the register to 10 
            //binary so basically in decimal it s gonna be 2 
            PR1 = pr; // FCY * ms(parsed in sec) * prescale
            T1CONbits.TON = 1;
            break;


        case TIMER2:
            T2CONbits.TON = 0;
            TMR2 = 0;
            T2CONbits.TCS = 0;
            choose_prescaler(ms, &pr, &tckps);
            T2CONbits.TCKPS = tckps;
            PR2 = pr;
            T2CONbits.TON = 1;
            break;

        case TIMER3:
            T3CONbits.TON = 0;
            TMR3 = 0;
            T3CONbits.TCS = 0;
            choose_prescaler(ms, &pr, &tckps);
            T3CONbits.TCKPS = tckps;
            PR2 = pr;
            T3CONbits.TON = 1;
            break;
    }
}

void tmr_wait_period(int timer) {

    switch (timer) {
        case 1:

            //while loop that waiting for the flag go up
            while (IFS0bits.T1IF == 0) {

            }

            IFS0bits.T1IF = 0; // reset TxIF to 0 again 
            break;
        case 2:
            while (IFS0bits.T2IF == 0) {

            }
            IFS0bits.T2IF = 0; // reset TxIF to 0 again 
            break;

        case 3:
            while (IFS0bits.T3IF == 0) {

            }
            IFS0bits.T3IF = 0; // reset TxIF to 0 again 
            break;

    }
}

// evrey time a new reference is recived 

void restart_TIMEOUT_timer() {
    T3CONbits.TON = 0; // Stops the timer
    IEC0bits.T3IE = 1; // Enable timer 3 interrupt
    TMR3 = 0;
    IFS0bits.T3IF = 0; // Set the timer flag to zero to be notified of a new event
    T3CONbits.TON = 1; // Starts the timer
}

// Timer 3 ISR - to bring the microcontroller in the TIMEOUT mode 

void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt() {
    IFS0bits.T3IF = 0; // Reset the flag of timer 3
    IEC0bits.T3IE = 0; // Disable interrupt of timer t2
    // Set timeout state
    uC_state = TIMEOUT_MODE;
    motor_data.leftRPM = 0;
    motor_data.rightRPM = 0;  // these RPMs will be set at the next PWM refresh
    //they do not necessarily have to be set to 0 immediately

    T3CONbits.TON = 0; // Stop the timer
    TMR2 = 0; // reset timer 
}