/*
 * File:   task.c
 * Author: francesco testa
 *
 * Created on 20 gennaio 2021, 16.33
 */


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
#include "tasks.h"

// function which manages messages sent via UART based on the state of the microcontroller
// and the header of the message itself
void msg_handler(char* msg_type, char* msg_payload) {
    // temporary local variables
    int tempRPM1 = 0;
    int tempRPM2 = 0;
    int tempMAXrpm = 0;
    int tempMINrpm = 0;

    // it's a sort of FSM 
    switch (uC_state) {
        case TIMEOUT_MODE:
            // set rpm motors to 0 (when the pwm is refreshed)
            motor_data.leftRPM = 0;
            motor_data.rightRPM = 0;

            if (strcmp(msg_type, "HLREF") == 0) {
                uC_state = CONTROLLED_MODE;
                // recursive call 
                msg_handler(msg_type, msg_payload); // to don't lose a msg 
            }
            break;
        case SAFE_MODE: // whether the S5 button is pressed 
            // interrupt check S5 status (see buttons.c file) 
            send_string_UART2("$MCACK,ENA,0*"); // no enable msg sent 
            if (strcmp(msg_type, "HLENA") == 0) {
                // set motors to 0
                motor_data.leftRPM = 0;
                motor_data.rightRPM = 0;
                uC_state = CONTROLLED_MODE;
                send_string_UART2("$MCACK,ENA,1*");
                resetButtonS5();
            }
            break;
        case CONTROLLED_MODE:
            // New reference msg
            if (strcmp(msg_type, "HLREF") == 0) {
                // extract rpms from msg_payload
                sscanf(msg_payload, "%d,%d", &tempRPM1, &tempRPM2);

                // saturate the value just sent considering the current max/min sat vealues (see myPWM.c file )
                tempRPM1 = check_RPM_value(tempRPM1); 
                tempRPM2 = check_RPM_value(tempRPM2);

                motor_data.leftRPM = tempRPM1;
                motor_data.rightRPM = tempRPM2;

                restart_TIMEOUT_timer(); // Reset the TIMER3 which set the TIMEOUT mode after 5 sec, since a new ref is arrived 
            }
            // New Max , Min rpm msg
            if (strcmp(msg_type, "HLSAT") == 0) {
                // extract the new min and max RPMs allowed 
                sscanf(msg_payload, "%d,%d", &tempMINrpm, &tempMAXrpm);
                if (!checkRange(tempMINrpm, tempMAXrpm)) { // checks if the new saturation values are within the system hardwere specifications
                    // positive ack
                    send_string_UART2("MCACK,SAT,1");
                }
                // negative ack
                send_string_UART2("MCACK,SAT,0");
            }
            break;
    }

}


// Task which recives references from UART buffer and parse them
// actually it reads the msgs contained in a circular buffer, which is filled using UART interupt  
void* task_receiver(void* params) {
    parser_state* pstate = (parser_state*) params;
    int UARTbyte = 0; // number of byte read from UART
    char tempChar; // contains a char read from the UART buffer 
    int bufError; // flag reading buffer error 
    int parseFlag;  // flag to check whether there is a new msg

    // check if there is some unread data in the UART circular buffer
    // notice that this buffer is automatically written
    while (dataToRead(&UARTbuf) > 0) {
        bufError = readBuf(&UARTbuf, &UARTbyte); // Read msg from the UART buffer 
        tempChar = UARTbyte; // Convert int into corresponding char ascii code
        parseFlag = parse_byte(pstate, tempChar); // Parse each byte 

        if (parseFlag == NEW_MESSAGE) {
            msg_handler(pstate->msg_type, pstate->msg_payload); // it handles the msg and generates the appropriate akc msg (it's defined above)
        }
    }
    
    return NULL;
}

// Task which write on the LCD different msg based on the status of button S6
void* task_LCD(void* params) {
    temperatureBuffer* tempBuf = (temperatureBuffer*) params;
    char tempStr[5];
    char rpmStr[14];
    if (S6status == S6_NOT_PRESSED) {
        spi_clean_LCD();

        // first row
        setTags1(); // print the Labels of the msg on the LCD (ST:  ,T: )
        // print the status of the microcontroller 
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
    } else { // S6 pressed
        spi_clean_LCD();
        setTags2();
        // first row msg
        // print the minimum and maximum current saturation values set
        sprintf(rpmStr, "%d ,%d", motor_data.minRPM, motor_data.maxRPM);
        spi_put_string(rpmStr, FIRST_ROW + 0x04);
        //second row msg
        //the values of the duty cycle PWM registers
        double dutyCycle1 = PDC2 / (2 * PTPER);
        double dutyCycle2 = PDC3 / (2 * PTPER);
        sprintf(rpmStr, "%.0f ,%.0f", dutyCycle1, dutyCycle2);
        spi_put_string(rpmStr, SECOND_ROW + 0x03);
    }
    return NULL;
}

// Task which refreshes the PWM values (RPM motor values)
void* task_PWM_controller(void* params) {
    //refresh PWM
    sendRPM(motor_data.leftRPM, motor_data.rightRPM); // see myPWM
    return NULL;
}

// Task which reads temperature from sensors and loads them into a circular buffer
void* task_temperature_acquisition(void* params) {
    temperatureBuffer* tempBuf = (temperatureBuffer*) params;
    while (ADCON1bits.DONE == 0);
    ADCON1bits.DONE = 0;

    int tempBits = ADCBUF0; // read from channel 0
    float tempVolts = tempBits * 5.0 / 1024.0; // bits to Volt
    float temperature = (tempVolts - 0.75) * 100.0 + 25; // Volt to Deg
    writeTempBuf(tempBuf, temperature); //Write values on temperature buffer
    return 0;
}

// Task which sends average temperature through UART 
void* task_send_temperature(void* params) {
    temperatureBuffer* tempBuf = (temperatureBuffer*) params;
    int i;
    double averageTemp = 0.0;
    // Calculate the average among read values 
    for (i = 0; i <= TEMP_BUFF_DIM; i++) {
        averageTemp = averageTemp + tempBuf->indexTemp[i];
    }
    tempBuf->averageTemp = averageTemp / TEMP_BUFF_DIM;
    // Send temperature to pc with MCTEM message [1 Hz]
    char msgTemp[20];
    // prepare the msg to be sent via UART
    sprintf(msgTemp, "$MCTEM,%.2f*", tempBuf->averageTemp);
    // send the msg through UART
    send_string_UART2(msgTemp);
    return NULL;
}

// Task which send feedback ack through UART
void* task_feedback_ack(void* params) {
    char msg[50];
    // prepare msg
    sprintf(msg, "$MCFBK,%d,%d,%d*", motor_data.leftRPM, motor_data.rightRPM, uC_state);
    //send to 
    send_string_UART2(msg);
    return NULL;
}

// Task which blinks led D3 and D4 when the uC is in TIMEOUT mode
void* task_LED_blink(void* params) {
    // Blink led D3
    LATBbits.LATB0 = !LATBbits.LATB0; // turn the Led D3 on and off

    if (uC_state == TIMEOUT_MODE) // If board is in timeout state
    { // Blink led D4
        LATBbits.LATB1 = !LATBbits.LATB1;
    } else // If board is NOT in timeout state
        LATBbits.LATB1 = 0; // Switch off led D4 
    return NULL;
}
