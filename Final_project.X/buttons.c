/*
 * File:   buttons.c
 * Author: francesco testa
 *
 * Created on 18 gennaio 2021, 11.48
 */


#include "xc.h"
#include "global_&_define.h"
#include "buttons.h"
#include "myPWM.h"
#include "timerFunc.h"

void setButton() {
    IEC0bits.T2IE = 1; // Enable interrupt of debouncing timer t2

    IFS0bits.INT0IF = 0; // Reset interrupt flag for S5 button
    IFS1bits.INT1IF = 0; // Reset interrupt flag for S6 button
    IEC0bits.INT0IE = 1; // Enable interrupt for S5 button
    IEC1bits.INT1IE = 1; // Enable interrupt for S6 button
}

void resetButtonS5() {
    IEC0bits.T2IE = 1; // Enable interrupt of debouncing timer t2
    IFS0bits.INT0IF = 0; // Reset interrupt flag for S5 button
    IEC0bits.INT0IE = 1; // Enable interrupt for S5 button
}

// S5 button ISR
// non importa del bouncing tanto attiva solo un flag (non cambia lo stato precedente come in S6)

void __attribute__((__interrupt__, __auto_psv__)) _INT0Interrupt() {
    IFS0bits.INT0IF = 0; // Reset interrupt flag
    //IEC0bits.INT0IE = 0; // Disable interrupt of button s5

    // Set safe state -> halt
    uC_state = SAFE_MODE;

    // Stop motors
    int rpm1 = 0;
    int rpm2 = 0;
    sendRPM(rpm1, rpm2);
    motor_data.leftRPM = rpm1;
    motor_data.rightRPM = rpm2;

}

// S6 buttons ISR
// necessita controllo del bouncing 

void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt() {
    IFS1bits.INT1IF = 0; // Reset interrupt flag


    // Toggle state of the flag
    S6status = (S6status + 1) % 2; // If flag = 0 -> flag = 1. If flag = 1 -> flag = 0

    // to avoid bouncing
    tmr_setup_period(TIMER2, 50); // Start debouncing timer - previously was 15 but sometimes it still bounced
    IEC1bits.INT1IE = 0; // Disable interrupt of button s6
    IEC0bits.T2IE = 1; // anable the timer2 interrupt 
}


//interrupt for debouncing timer 

void __attribute__((__interrupt__, __auto_psv__)) _INT2Interrupt() {
    T2CONbits.TON = 0; // stop the timer T2
    IFS0bits.T2IF = 0; // put down the flag interrupt timer2

    IEC1bits.INT1IE = 0; // put down the flag of INT0 (perchè anche se disabilito l'interupt, il flag può cambiare il suo stato)
    IEC1bits.INT1IE = 1; // and finnaly I anable it 
}




