/*
 * File:   myADC.c
 * Author: francesco testa
 *
 * Created on 16 dicembre 2020, 19.35
 */


#include "myADC.h"

// ADC configuration 
void adc_config(int port){
    // 10 Hz acquisition temperature func 
    ADCON3bits.ADCS = 63; //TAD value
    ADCON1bits.ASAM = 1; //automatic sampling start
    //automatic conversion start
    ADCON3bits.SAMC = 15;  // Sample time 15 Tad: how long the sampling should last [0 Tad - 31 Tad]
    ADCON1bits.SSRC = 7; 
    ADCON2bits.CHPS = 0; // set channel 0
    //select input  
    switch(port){
        case AN1:
            ADCHSbits.CH0SA = 1;
            ADPCFGbits.PCFG1 = 0; 
            break;
        case AN2:
            ADCHSbits.CH0SA = 2; 
            ADPCFGbits.PCFG2 = 0; 
            break;
        case AN3:
            ADCHSbits.CH0SA = 3; 
             ADPCFGbits.PCFG3 = 0;  
            break;
    }

    ADCHSbits.CH0NA = 0; // set the negeative value on -Vref , in order to have channel 1 w.r.t GROUND    
    
    ADPCFG = 0xFFFF; // set all register to 1 so digital 
    ADCON1bits.ADON = 1; // enable the ADC
}

// ADC configuration for 2 channels 
void adc_config_2_chan(int port1, int port2){
    ADCON3bits.ADCS = 8; //TAD value
    ADCON1bits.ASAM = 1; //automatic sampling start
    // set automatic conversion 
    ADCON3bits.SAMC = 1; 
    ADCON1bits.SSRC = 7; 
    
    ADCON2bits.CHPS = 1; //to use cha0 and cha1 
    
    switch(port1){
        case AN1:
            ADCHSbits.CH0SA = 1;
            ADPCFGbits.PCFG1 = 0; 
            break;
        case AN2:
            ADCHSbits.CH0SA = 2; // selct positive input AN2 
            ADPCFGbits.PCFG2 = 0; //and set only AN2 to analog ( 0 )
            break;
        case AN3:
            ADCHSbits.CH0SA = 3; 
             ADPCFGbits.PCFG3 = 0;  //and set only AN2 to analog ( 0 )
            break;
    }
    //select input 
    ADCHSbits.CH0NA = 0; // set the negeative value on -Vref , in order to have AN2 w.r.t GROUND 
    
    switch(port2){
        case AN1:
           ADCHSbits.CH123SA = 1;
           ADPCFGbits.PCFG1 = 0; 
           break;
        case AN2:
            ADCHSbits.CH123SA = 0;
            ADPCFGbits.PCFG2 = 0;
            break;
        case AN3:
            ADCHSbits.CH123SA = 1;
            ADPCFGbits.PCFG3 = 0;
            break;
    }
    ADCHSbits.CH123NA = 0; // for put to GND ch1 which have just connected to AN3
    
    // we have to set AN3 and AN2 to analog 
    ADPCFG = 0xFFFF; 
    
    ADCON1bits.ADON = 1; // enable the ADC
}