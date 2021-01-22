/*
 * File:   myBuffer.c
 * Author: francesco testa
 *
 * Created on 17 gennaio 2021, 11.28
 */


#include "xc.h"
#include "myBuffer.h"

void initBuf(circularBuffer *buf, temperatureBuffer *tempBuf,  int dimCircBuff) {
    // temperature
    tempBuf-> writeTempIndex = 0;
    //circular buffer
    buf->dim = dimCircBuff;
    buf->readIndex = 0;
    buf->writeIndex = 0;
}

void writeTempBuf(temperatureBuffer *tempBuf, float floatData) {
    tempBuf->indexTemp[tempBuf->writeTempIndex] = floatData;
    tempBuf->writeTempIndex++;
    if (tempBuf->writeTempIndex == TEMP_BUFF_DIM) {
        tempBuf->writeTempIndex = 0; // restart to write from the first cell
    }
}

void writeBuf(circularBuffer *buf, int data) {
    buf->index[buf->writeIndex] = data;
    buf->writeIndex++;
    if (buf->writeIndex == buf->dim) {
        buf->writeIndex = 0; // restart to write from the first cell
    }
}

int readBuf(circularBuffer *buf, int *data) {
    IEC1bits.U2RXIE = 0; // Disable interrupt of UART
    if (buf->readIndex == buf->writeIndex) {// We've finished to read the buffer
        IEC1bits.U2RXIE = 1; // Enable interrupt of UART
    }
    // reading 
    *data = buf->index[buf->readIndex];
    buf->readIndex++;
    if (buf->readIndex == buf->dim) { //check if I have to restart from the first cell of the buffer 
        buf->readIndex = 0;  // reset the read index
    }
    IEC1bits.U2RXIE = 1; // Enable interrupt of UART
    return 1;
}

// tell us whether there are new data in the buffer 
int dataToRead(circularBuffer *buf) {
    
    if (buf->writeIndex >= buf->readIndex) {
        return (buf->writeIndex - buf->readIndex);
    } else {// case in which the buffer is overwriting 
        return (buf->dim - (buf->readIndex - buf->writeIndex));
    }
    return -1; // Something went wrong
}

