#ifndef XC_HEADER_GLOBAL_H
#define	XC_HEADER_GLOBAL_H

#include <xc.h>
#include "myBuffer.h"
#include "parser.h"
#include "myScheduler.h"
// da mettere in un global.h
#define MAX_PROPELLER 8000
#define MIN_PROPELLER -8000
#define MAX_MOTOR_RPM 10000
#define MIN_MOTOR_RPM -10000

// states 
#define CONTROLLED_MODE 0
#define TIMEOUT_MODE 1
#define SAFE_MODE 2

#define LCD1 1
#define LCD2_S6 2
//buffer 
#define UART_BUFF_DIM 100
#define TEMP_BUFF_DIM 10

#define FCY 1843200
#define F_PWM 1000

// messi tutti gli extern delle global variable per condividerli con altri file.c
typedef struct{
    int maxRPM;
    int minRPM;
    int leftRPM;
    int rightRPM;
}motorsData;

extern int uC_state;
extern int S6status;
extern circularBuffer UARTbuf;
extern temperatureBuffer tempBuf;
extern motorsData motor_data;



#endif	/* XC_HEADER_GLOBAL_H */

