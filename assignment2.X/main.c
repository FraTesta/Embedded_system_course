/*
 * File:   main.c
 * Author: francesco testa
 *
 * Created on 16 dicembre 2020, 18.12
 */


#include <p30F4011.h>

#include "xc.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
// personal libraries
#include "mySPI.h"
#include "timerFunc.h"
#include "myADC.h"
#include "parser.h"
#include "myScheduler.h"
#include "myUART.h"

#define MAX_TASK 5
#define MAX_DC_Vel 5

heart_beat schedInfo[MAX_TASK];

// Data struct which contains the sensors data  

typedef struct {
    float potCurrent;
    float temperature;
} sensors_data;

// struct to parse the msg coming from UART
typedef struct {
    parser_state* parsState;
    int rpm;
} parsed_data;

// task which recives motor velocities from UART and parse them
void* task_vel_reciver(void* params) {
    parsed_data* parsData = (parsed_data*) params;
    // check the overflow of UART
    if (U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;
    }

    while (U2STAbits.URXDA == 1) {
        // parse the UART data 
        int status = parse_byte(parsData->parsState, U2RXREG);

        if (status == NEW_MESSAGE) {// check whether there is a new parsed msg ready
            // check the msg type and whether the message has content payload
            if (strcmp(parsData->parsState->msg_type, "MCREF") == 0 && strlen(parsData->parsState->msg_payload) > 0) {
                parsData->rpm = atoi(parsData->parsState->msg_payload); // string to int conversion 
            }

        }
    }
    return NULL;
}

// send temperature and current values through UART
void* task_send_data_sensor(void* params) {
    sensors_data* sensData = (sensors_data*) params;
    char msg[21];
    // prepare the msg to be sent via UART
    sprintf(msg, "$MCFBK,%1.1f,%1.1f*", sensData->potCurrent, sensData->temperature);
    // send the msg through UART
    send_string_UART2(msg);
    return NULL;
}

// send the recived velocities to the motors through UART
void* task_set_voltage_PWM(void* params) {
    parsed_data* parsData = (parsed_data*) params;
    // 1000 rpm = 100% duty cycle 
    double dutyCycle = (parsData->rpm / MAX_DC_Vel);
    PDC2 = 2 * PTPER*dutyCycle;
    return NULL;
}

// read data from sensors 
void* task_read_sensors(void* params) {
    sensors_data* sensData = (sensors_data*) params;
    while (ADCON1bits.DONE == 0);
    ADCON1bits.DONE = 0;
    // Potentiometer
    int potBits = ADCBUF0;
    float potVolts = 3 + 2 * (potBits / 1024.0);
    sensData->potCurrent = (potVolts - 3)* 10;

    if (sensData->potCurrent > 15) { // check if the current is greater than 15 A 
        LATBbits.LATB1 = 1; // turn the led on
    } else {
        LATBbits.LATB1 = 0;
    }
    // Temperature 
    int tempBits = ADCBUF1;
    float tempVolts = tempBits * 5.0 / 1024.0; // conversion from bits to voltage 
    sensData->temperature = (tempVolts - 0.75) * 100.0 + 25; // coversion from volt to temperature 
    return NULL;
}

// led blinking  
void* task_Led_blinking(void* params) {
    LATBbits.LATB0 = !LATBbits.LATB0; // turn the Led D3 on and off
    return NULL;
}

int main(void) {
    //////////////////////////////   Initialization Protocols   /////////////////////////////////////////
    // ADC init
    adc_config_2_chan(AN2, AN3); // to configure two channel of the adc 
    // PWM init 
    PWM_config();
    // UART init
    UART_config(2);
    // Parser init 

    //////////////////////////////   Initialization Data   ///////////////////////////////////////////////
    // LED init 
    TRISBbits.TRISB0 = 0; // D3
    TRISBbits.TRISB1 = 0; // D4

    // Parser init
    parser_state pstate;
    pstate.state = STATE_DOLLAR;
    pstate.index_type = 0;
    pstate.index_payload = 0;

    // Sensor data init 
    sensors_data sensData;
    sensData.potCurrent = 0;
    sensData.temperature = 0;

    // parsed data struct init 
    parsed_data parsData;
    parsData.rpm = 0;
    parsData.parsState = &pstate;

    // heartbeat time
    tmr_setup_period(TIMER1, 5); //200 Hz = 5 ms 

    schedInfo[0].task = &task_vel_reciver;
    schedInfo[1].task = &task_send_data_sensor;
    schedInfo[2].task = &task_set_voltage_PWM;
    schedInfo[3].task = &task_read_sensors;
    schedInfo[4].task = &task_Led_blinking;

    schedInfo[0].params = &parsData;
    schedInfo[1].params = &sensData;
    schedInfo[2].params = &parsData;
    schedInfo[3].params = &sensData;
    schedInfo[4].params = NULL;

    // initialize n
    schedInfo[0].n = 0;
    schedInfo[1].n = 0;
    schedInfo[2].n = 0;
    schedInfo[3].n = 0;
    schedInfo[4].n = 0;


    // set N according to the time of execution of each tasks
    schedInfo[0].N = 2;//since UART boundrate is 9600 bps -> 9 bpms . Our UART msg has 11 bits so 1.3 ms necessary to send it
    schedInfo[1].N = 200; // UART in transmission must be 1 Hz
    schedInfo[2].N = 8; // the frequency with which we send the motor vel to the motors should be higher than the velocity sent through UART
    schedInfo[3].N = 10; // since the transmit frequency is 1 Hz, then the sensor reading frequency should be higher.
    schedInfo[4].N = 100; // Led D3 must blink at 1 Hz


    while (1) {
        scheduler(MAX_TASK, &schedInfo); // ora non devo più passare &si allo scheduler 
        tmr_wait_period(TIMER1);
    }

    return 0;
}

