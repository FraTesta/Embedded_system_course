/*
 * File:   myBuffer.c
 * Author: francesco testa
 *
 * Created on 17 gennaio 2021, 11.28
 */


#include "xc.h"
#include "myBuffer.h"

void initBuf(circularBuffer *buf, int dim) {
    buf->dim = dim;
    buf->readIndex = 0;
    buf->writeIndex = 0;
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
    if (buf->readIndex == buf->dim) { //check if I have to restart from the first cell
        buf->readIndex = 0;
    }
    IEC1bits.U2RXIE = 1; // Enable interrupt of UART
    return 1;
}

// tell us whether there are new data in the buffer 
int dataToRead(circularBuffer *buf) {
    // the size is the number of cells in the CIRCULAR buffer which have still to be read.
    if (buf->writeIndex >= buf->readIndex) {
        return (buf->writeIndex - buf->readIndex);
    } else {
        return (buf->dim - (buf->readIndex - buf->writeIndex));
    }
    return -1; // Something went wrong
}

