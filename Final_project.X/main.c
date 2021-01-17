/*
 * File:   main.c
 * Author: francesco testa
 *
 * Created on 16 gennaio 2021, 15.14
 */


#include <p30F4011.h>

#include "xc.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
// personal libraries
#include "timerFunc.h"
#include "myADC.h"
#include "parser.h"
#include "myScheduler.h"
#include "myUART.h"
#include "global_&_define.h"  // file.h which contains all global variables 



///////////////////////////////////////////////////////////////////////////////////////////////////////
void* task_PWM_controller(void* params){} 
/*
 * - check if max/min settati non supero il max/min consentito, in tal caso saturo tali variabili al loro massimo
 * - refresh
 */

void* task_temperature_acquisition(void* params){}
// acquisizione dati temperatura (quindi ADC ....) 


void* task_send_temperature(void* params){}
/* media dei valori di temp
 * send msg MCTEM
 */

void* task_feedback_ack(void* params){}
// send MCFBK msg

void* task_LED_blink(void* params){}
// blink led D3
// and D4 iff we are in safe mode

void* task_LCD(void* params){}
// in base allo stato di S6 stampare un msg o l'altro 

void* task_receiver(void* params){
    //check UART
    //parse data
    switch(uC_state){
        case TIMEOUT_MODE:
            // set motor to 0
            uC_state = CONTROLLED_MODE;
            msg_handler();
            break;
        case SAFE_MODE:
            // interrupt di S5 deve stoppare i motori 
            // if(msg_type == HLENA ){
            uC_state = CONTROLLED_MODE;
            // motor speed 0
            // ack msg
            break;
        case CONTROLLED_MODE:
            msg_handler();
            break;
            
    }
}
void msg_handler(){}


int main(void) {
    
    //data init 
    // motor init
    motorsData motor_data;
    motor_data.leftRPM = 0;
    motor_data.rightRPM = 0;
    motor_data.maxRPM = 8000;
    motor_data.minRPM = -8000;
    // init LCD mode
    lcd_mode = LCD1;
    // init microcontroller mode 
    uC_state = CONTROLLED_MODE;
          
    return 0;
}
