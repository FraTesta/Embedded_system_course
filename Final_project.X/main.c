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
#include "mySPI_LCD.h"
#include "global_&_define.h"  // file.h which contains all global variables 
#include "myBuffer.h"
#include "buttons.h"

// The following are variables necessarily global since they are also used in some interrupt handler functions,
//and in other project files:
// microcontroller state 
int uC_state;
// flag for button S6
int S6status;
// urat buffer
circularBuffer UARTbuf;
// scheduler
heart_beat schedInfo[MAX_TASK];
// motors data struct 
motorsData motor_data;
// Parser state variable
parser_state pstate;
// safe mode flag (per mandare l'ack quando un msg è arrivato )
int prevSafe = 0;

///////////////////////////////////////////////////// FUNCTIONS //////////////////////////////////////////////////

void msg_handler(char* msg_type, char* msg_payload) {
    // temporary local variables
    int tempRPM1 = 0;
    int tempRPM2 = 0;
    int tempMAXrpm = 0;
    int tempMINrpm = 0;

    switch (uC_state) {
        case TIMEOUT_MODE:
            // set motor to 0 this is done in the pwm task probably 
            motor_data.leftRPM = 0;
            motor_data.rightRPM = 0;
            // mandare msg PWM ? o aspettare il prossimo refrash
            if (strcmp(msg_type, "HLREF") == 0) {
                uC_state = CONTROLLED_MODE;
                // recursive call ?
                msg_handler(msg_type, msg_payload);
            }
            break;
        case SAFE_MODE:
            // interrupt di S5 
            send_string_UART2("MCACK,ENA,0"); // no enable msg sent 
            if (strcmp(msg_type, "HLENA") == 0) {
                // set motors to 0
                motor_data.leftRPM = 0;
                motor_data.rightRPM = 0;
                uC_state = CONTROLLED_MODE;
                send_string_UART2("MCACK,ENA,1");
                resetButtonS5();
            }
            break;
        case CONTROLLED_MODE:
            // New reference msg
            if (strcmp(msg_type, "HLREF") == 0) {
                // extract rpms from msg_payload
                sscanf(msg_payload, "%d,%d", &tempRPM1, &tempRPM2);

                tempRPM1 = check_RPM_value(tempRPM1);
                tempRPM2 = check_RPM_value(tempRPM2);

                motor_data.leftRPM = tempRPM1;
                motor_data.rightRPM = tempRPM2;


                restart_TIMEOUT_timer();
            }
            // New Max , Min rpm 
            if (strcmp(msg_type, "HLSAT") == 0) {
                // extract min and max RPMs allowed 
                sscanf(msg_payload, "%d,%d", &tempMINrpm, &tempMAXrpm);
                if (!checkRange(tempMINrpm, tempMAXrpm)) {
                    // positive ack
                    send_string_UART2("MCACK,SAT,1");
                }
                // negative ack
                send_string_UART2("MCACK,SAT,0");

            }
            break;
    }

}



///////////////////////////////////////////////// TASKS ///////////////////////////////////////////////////

void* task_PWM_controller(void* params) {
    //refresh PWM
    sendRPM(motor_data.leftRPM, motor_data.rightRPM);
}


void* task_temperature_acquisition(void* params) {
    temperatureBuffer* tempBuf = (temperatureBuffer*) params;
    while (ADCON1bits.DONE == 0);
    ADCON1bits.DONE = 0;

    int tempBits = ADCBUF1;
    float tempVolts = tempBits * 5.0 / 1024.0; // bits to Volt
    float temperature = (tempVolts - 0.75) * 100.0 + 25; // Volt to Deg
    writeTempBuf(tempBuf, temperature); //Write values on temperature buffer
    return 0;
}
// acquisizione dati temperatura (quindi ADC ....) 

void* task_send_temperature(void* params) {
    temperatureBuffer* tempBuf = (temperatureBuffer*) params;
    int i;
    double averageTemp = 0.0;
    // Calculate the average among read values 
    for (i = 0; i <= TEMP_BUFF_DIM; i++) {
        averageTemp = averageTemp + tempBuf->indexTemp[i];
    }
    tempBuf->averageTemp = tempBuf->averageTemp / TEMP_BUFF_DIM;
    // Send temperature to pc with MCTEM message [1 Hz]
    char msgTemp[20];
    // prepare the msg to be sent via UART
    sprintf(msgTemp, "$MCTEM,%.2f*", tempBuf->averageTemp);
    // send the msg through UART
    send_string_UART2(msgTemp);
    return NULL;
}



void* task_feedback_ack(void* params) {
    char msg[50];
    // prepare msg
    sprintf(msg, "MCFBK,%d,%d,%d", motor_data.leftRPM, motor_data.rightRPM, uC_state);
    //send to 
    send_string_UART2(msg);
}
// send MCFBK msg

void* task_LED_blink(void* params) {
    // Blink led D3
    LATBbits.LATB0 = !LATBbits.LATB0; // turn the Led D3 on and off

    if (uC_state == TIMEOUT_MODE) // If board is in timeout state
    { // Blink led D4
        LATBbits.LATB1 = !LATBbits.LATB1;
    } else // If board is NOT in timeout state
        LATBbits.LATB1 = 0; // Switch off led D4 
    //return 0;
}
// blink led D3
// and D4 iff we are in safe mode

void* task_LCD(void* params) {
    temperatureBuffer* tempBuf = (temperatureBuffer*) params;
    char tempStr[5];
    char rpmStr[14];
    if (S6status == S6_NOT_PRESSED) {
        spi_clean_LCD();

        // first row
        setTags1();
        switch (uC_state) {

            case TIMEOUT_MODE:
                spi_put_string("T", FIRST_ROW + 0x04);
                break;
            case SAFE_MODE:
                spi_put_string("H", FIRST_ROW + 0x04);
                break;
            case CONTROLLED_MODE:
                spi_put_string("C", FIRST_ROW + 0x04);
                break;
        }
        sprintf(tempStr, "%.2f", tempBuf->averageTemp);
        spi_put_string(tempStr, TEMP_LCD_POSIT + 0x03);
        // second row    
        sprintf(rpmStr, "%d; %d", motor_data.leftRPM, motor_data.rightRPM);
        spi_put_string(rpmStr, SECOND_ROW + 0x03);
    } else {
        spi_clean_LCD();
        setTags2();
        // first row msg
        // the minimum and maximum current saturation values set
        sprintf(rpmStr, "%d ,%d", motor_data.minRPM, motor_data.maxRPM);
        spi_put_string(rpmStr, FIRST_ROW + 0x04);
        //second row msg
        //the values of the duty cycle PWM registers
        double dutyCycle1 = PDC2 / (2 * PTPER);
        double dutyCycle2 = PDC3 / (2 * PTPER);
        sprintf(rpmStr, "%.0f ,%.0f", dutyCycle1, dutyCycle2);
        spi_put_string(rpmStr, SECOND_ROW + 0x03);

    }
}
// in base allo stato di S6 stampare un msg o l'altro 

void* task_receiver(void* params) {
    parser_state* pstate = (parser_state*) params;
    int UARTbyte = 0; // byte read from UART
    char tempChar; // contains a char read from the UART buffer 
    int bufError;
    int parseFlag;
    //refresh PWM
    //sendRPM(motor_data.leftRPM, motor_data.rightRPM);

    // check if there is some unread data in the UART buffer
    // notice that this buffer is automatically 
    while (dataToRead(&UARTbuf) > 0) {
        bufError = readBuf(&UARTbuf, &UARTbyte); // Read msg from the UART buffer 
        tempChar = UARTbyte; // Convert int into corresponding char ascii code
        parseFlag = parse_byte(pstate, tempChar); // Parse each byte 

        if (parseFlag == NEW_MESSAGE) {
            msg_handler(pstate->msg_type, pstate->msg_payload); // Get type of message
            //send_string_UART2(ack);
        }
    }
    //check UART


}

int main(void) {
    //////////////////////////////   Peripherals   /////////////////////////////////////////
    // ADC init
    adc_config(AN3); // to configure two channel of the adc 
    // PWM init 
    PWM_config();
    // UART init
    UART_config(UART_2); // PROBABILMENTE DA CAMBIARE 


    //////////////////////////////   Initialization Data   ///////////////////////////////////////////////
    // LED init 
    TRISBbits.TRISB0 = 0; // D3
    TRISBbits.TRISB1 = 0; // D4
    
    // motor data init
    motor_data.leftRPM = 0;
    motor_data.rightRPM = 0;
    motor_data.maxRPM = MAX_PROPELLER;
    motor_data.minRPM = MIN_PROPELLER;

    // temperature buffer 
    temperatureBuffer tempBuf;

    // initialization of the two buffers
    initBuf(&UARTbuf, &tempBuf, UART_BUFF_DIM);
    // init LCD mode
    S6status = S6_NOT_PRESSED;
    // init microcontroller mode 
    uC_state = CONTROLLED_MODE;

    // Init timer for TIMEOUT mode
    tmr_setup_period(TIMER3, 5000);
    IFS0bits.T3IF = 1; // enable timer 3 interrupt 
    
    schedInfo[0].task = &task_PWM_controller;
    schedInfo[1].task = &task_temperature_acquisition;
    schedInfo[2].task = &task_send_temperature;
    schedInfo[3].task = &task_feedback_ack;
    schedInfo[4].task = &task_LED_blink;
    schedInfo[5].task = &task_LCD;
    schedInfo[6].task = &task_receiver;
    
    schedInfo[0].params = NULL;
    schedInfo[1].params = &tempBuf;
    schedInfo[2].params = &tempBuf;
    schedInfo[3].params = NULL;
    schedInfo[4].params = NULL;
    schedInfo[5].params = &tempBuf;
    schedInfo[6].params = &pstate;
    
    // initialize n
    schedInfo[0].n = 0;
    schedInfo[1].n = 0;
    schedInfo[2].n = 0;
    schedInfo[3].n = 0;
    schedInfo[4].n = 0;
    schedInfo[5].n = 0;
    schedInfo[6].n = 0;
    
    // set N according to the time of execution of each tasks
    schedInfo[0].N = 0; // PWM controller 
    schedInfo[1].N = 0; // Temperature Acquisition 10 Hz
    schedInfo[2].N = 0; // Send temperature 1 Hz
    schedInfo[3].N = 0; // Feedback 5 Hz
    schedInfo[4].N = 0; // LED blink 1 Hz
    schedInfo[5].N = 0; // LCD 10 Hz
    schedInfo[6].N = 0; // Task Reciver 10 Hz
 

    
    // heartbeat time
    tmr_setup_period(TIMER1, 5); //200 Hz = 5 ms 
    while (1) {
        scheduler(MAX_TASK, &schedInfo); // ora non devo più passare &si allo scheduler 
        tmr_wait_period(TIMER1);
    }
    return 0;
}
