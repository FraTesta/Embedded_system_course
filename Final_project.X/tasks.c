/*
 * File:   tasks.c
 * Author: francesco testa
 *
 * Created on 20 gennaio 2021, 12.01
 */
#include "tasks.h"
#include "global_&_define.h"
#include "myPWM.h"
#include "mySPI_LCD.h"

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

void* task_PWM_controller(void* params) {
    //refresh PWM
    sendRPM(motor_data.leftRPM, motor_data.rightRPM);
}
/*
 * - check if max/min settati non supero il max/min consentito, in tal caso saturo tali variabili al loro massimo
 * - refresh
 */

void* task_temperature_acquisition(void* params) {
    temperatureBuffer* tempBuf = (temperatureBuffer*) params;
    while (ADCON1bits.DONE == 0);
    ADCON1bits.DONE = 0;

    int tempBits = ADCBUF1;
    float tempVolts = tempBits * 5.0 / 1024.0; // bits to Volt
    float temperature = (tempVolts - 0.75) * 100.0 + 25; // Volt to Deg
    writeTempBuf(&tempBuf, temperature); //Write values on temperature buffer
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

/* media dei valori di temp
 * send msg MCTEM
 */

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
        sprintf(rpmStr, "%.2f ,%.2f", PDC2 / (2 * PTPER), PDC3 / (2 * PTPER));
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
        parseFlag = parse_byte(&pstate, tempChar); // Parse each byte 

        if (parseFlag == NEW_MESSAGE) {
            msg_handler(pstate->msg_type, pstate->msg_payload); // Get type of message
            //send_string_UART2(ack);
        }
    }
    //check UART


}