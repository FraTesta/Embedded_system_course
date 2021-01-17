/*
 * File:   main.c
 * Author: francesco testa
 *         luca covizi
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
#include "myPWM.h"
#include "global_&_define.h"  // file.h which contains all global variables 
#include "myBuffer.h"



// urat buffer
circularBuffer UARTbuf;
// Parser state variable
parser_state pstate;

///////////////////////////////////////////////////// FUNCTIONS //////////////////////////////////////////////////

// check whether the sent rpm vaue is outside of the rpm allowed, in such case the system saturate those values to the max/min allowed. 

int check_RPM_value(int rpm, motorsData *mot_data) {
    if (rpm > mot_data->maxRPM) {
        rpm = mot_data->maxRPM;
    } else if (rpm < mot_data->minRPM) {
        rpm = mot_data->minRPM;
    }
    return rpm;
}

int checkRange(int min, int max, motorsData* mot_data) {
    if (max > min && max < MIN_PROPELLER && min > MIN_PROPELLER && max >= 0 && min <= 0) {
        mot_data->minRPM = min;
        mot_data->maxRPM = max;
        return 0;
    }
    return -1; // send negative ack 
}

void msg_handler(char* msg_type, char* msg_payload, motorsData* mot_data) {
    // temporary local variables
    int tempRPM1 = 0;
    int tempRPM2 = 0;
    int tempMAXrpm = 0;
    int tempMINrpm = 0;

    switch (uC_state) {
        case TIMEOUT_MODE:
            // set motor to 0 this is done in the pwm task probably 
            uC_state = CONTROLLED_MODE;
            //msg_handler();
            break;
        case SAFE_MODE:
            // interrupt di S5 
            //( IMMIDIATLY motor speed 0, PROB. using interupt )
            if (strcmp(msg_type, "HLENA") == 0) {
                uC_state = CONTROLLED_MODE;
                // vel mot to 0
                // ack msg
            }
            break;
        case CONTROLLED_MODE:
            if (strcmp(msg_type, "HLREF") == 0) {
                // extract rpms from msg_payload
                sscanf(msg_payload, "%d,%d", &tempRPM1, &tempRPM2);

                tempRPM1 = check_RPM_value(tempRPM1, mot_data);
                tempRPM2 = check_RPM_value(tempRPM2, mot_data);

                mot_data->leftRPM = tempRPM1;
                mot_data->rightRPM = tempRPM2;
            }
            if (strcmp(msg_type, "HLSAT") == 0) {
                // extract min and max RPMs allowed 
                sscanf(msg_payload, "%d,%d", &tempMINrpm, &tempMAXrpm);
                if (!checkRange(tempMINrpm, tempMAXrpm, mot_data)) {
                    // positive ack
                    send_string_UART2("MCACK,REF,1"); 
                     // Restart timer since a new reference arrived
                }
                // negative ack
                send_string_UART2("MCACK,REF,0");
            }
            break;

    }
}




///////////////////////////////////////////////// TASKS ///////////////////////////////////////////////////

void* task_PWM_controller(void* params) {
}

/*
 * - check if max/min settati non supero il max/min consentito, in tal caso saturo tali variabili al loro massimo
 * - refresh
 */

void* task_temperature_acquisition(void* params) {
}
// acquisizione dati temperatura (quindi ADC ....) 

void* task_send_temperature(void* params) {
}

/* media dei valori di temp
 * send msg MCTEM
 */

void* task_feedback_ack(void* params) {
}
// send MCFBK msg

void* task_LED_blink(void* params) {
}
// blink led D3
// and D4 iff we are in safe mode

void* task_LCD(void* params) {
}
// in base allo stato di S6 stampare un msg o l'altro 

void* task_receiver(void* params) {
    motorsData* mot_data = (motorsData*) params;
    int UARTbyte = 0; // byte read from UART
    char tempChar; // contains a char read from the UART buffer 
    int bufError;
    int parseFlag;
    int ack;
    // check if there is some unread data in the UART buffer
    // notice that this buffer is automatically 
    while (sizeBuf(&UARTbuf) > 0) {
        bufError = readBuf(&UARTbuf, &UARTbyte); // Read msg from the UART buffer 
        tempChar = UARTbyte; // Convert int into corresponding char ascii code
        parseFlag = parse_byte(&pstate, tempChar); // Parse each byte 

        if (parseFlag == NEW_MESSAGE) {
            ack = msg_handler(pstate.msg_type, pstate.msg_payload, mot_data); // Get type of message
            send_string_UART2(ack)
        }
    }
    //check UART


}

int main(void) {
    //////////////////////////////   Initialization Protocols   /////////////////////////////////////////
    // ADC init
    adc_config_2_chan(AN2, AN3); // to configure two channel of the adc 
    // PWM init 
    PWM_config();
    // UART init
    UART_config(2);
    //////////////////////////////   Initialization Data   ///////////////////////////////////////////////
    // LED init 
    TRISBbits.TRISB0 = 0; // D3
    TRISBbits.TRISB1 = 0; // D4
    // motor data init
    motorsData motor_data;
    motor_data.leftRPM = 0;
    motor_data.rightRPM = 0;
    motor_data.maxRPM = MAX_PROPELLER;
    motor_data.minRPM = MIN_PROPELLER;

    initBuf(&UARTbuf, UART_BUFF_DIM);
    // init LCD mode
    lcd_mode = LCD1;
    // init microcontroller mode 
    uC_state = CONTROLLED_MODE;

    return 0;
}
