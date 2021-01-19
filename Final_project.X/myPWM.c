/*
 * File:   myPWM.c
 * Author: francesco testa
 *
 * Created on 19 dicembre 2020, 10.09
 */


#include "myPWM.h"
#include "global_&_define.h"

// configuration for 2 PWM signals 
void PWM_config() {
    // PWM period 20 ms = 50 Hz
    PTCONbits.PTMOD = 0;
    // set positive PWM signals
    PWMCON1bits.PEN2H = 1; // set port PWM2H
    PWMCON1bits.PEN3H = 1; // set port PWM2H
    
    // PTPER must fit in 15 bits -> <32767
    // otherwise I have to change the prescaler 
    // PTPER = Fcy/(f_pwm * PTMR_Prescaler) -1
    // Given our frequency requirement, we can set the prescaler at 1  (1:1)
    // PTPER = 1843200/(1000*1) - 1 = 1842 
    // so a prescaler 1:1 is sufficent
    PTCONbits.PTCKPS = 0; // prescaler 1:1
    PTPER = FCY/(F_PWM*1) - 1;
    PTCONbits.PTEN = 1; //enable PWM
}

void sendRPM(int rpm1, int rpm2){
    double dutyCycle1 = (rpm1 - MIN_MOTOR_RPM)/ (MAX_MOTOR_RPM - MIN_MOTOR_RPM);
    double dutyCycle2 = (rpm2 - MIN_MOTOR_RPM)/ (MAX_MOTOR_RPM - MIN_MOTOR_RPM);
    PDC2 = 2 * PTPER*dutyCycle1;
    PDC3 = 2 * PTPER*dutyCycle2;
}
// check whether the sent rpm vaue is outside of the rpm allowed, in such case the system saturate those values to the max/min allowed. 
int check_RPM_value(int rpm, motorsData *mot_data) {
    if (rpm > mot_data->maxRPM) {
        rpm = mot_data->maxRPM;
    } else if (rpm < mot_data->minRPM) {
        rpm = mot_data->minRPM;
    }
    return rpm;
}
// set new 
int checkRange(int min, int max, motorsData* mot_data) {
    if (max > min && max < MIN_PROPELLER && min > MIN_PROPELLER && max >= 0 && min <= 0) {
        mot_data->minRPM = min;
        mot_data->maxRPM = max;
        return 0;
    }
    return -1; // send negative ack 
}


