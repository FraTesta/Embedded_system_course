/*
 * File:   myPWM.c
 * Author: francesco testa
 *
 * Created on 19 dicembre 2020, 10.09
 */


#include "myPWM.h"
#include "global_&_define.h"

void PWM_config() {
    // PWM period 20 ms = 50 Hz
    PTCONbits.PTMOD = 1;
    PTCONbits.PTCKPS = 1; // prescaler 1:4
    PWMCON1bits.PEN2H = 1; // set port PWM2H
    PTPER = 9215;
    PTCONbits.PTEN = 1;
}



