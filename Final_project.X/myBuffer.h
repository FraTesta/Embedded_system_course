
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_MY_BUFFER_H
#define	XC_HEADER_MY_BUFFER_H


#include <xc.h> // include processor files - each processor file is guarded.  

#define TEMP_BUFF_DIM 10

typedef struct{
    float indexTemp[TEMP_BUFF_DIM]; // celle dedicate 
    int writeTempIndex; // idice celle per scrittura dati
}temperatureBuffer; //buffer for temperature

typedef struct{
    int *index;
    int readIndex;
    int writeIndex;
    int dim;
}circularBuffer; // circular buffer for the UART msgs

void initBuf(circularBuffer *buf, temperatureBuffer *tempBuf,  int dimCircBuff);
void writeTempBuf(temperatureBuffer *tempBuf, float floatData);
void writeBuf(circularBuffer *buf, int data);
int readBuf(circularBuffer *buf, int *data);
// tell us whether there are new data in the buffer 
int dataToRead(circularBuffer *buf);

#endif	/* XC_HEADER_MY_BUFFER_H */