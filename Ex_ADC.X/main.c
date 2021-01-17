/*
 * File:   main.c
 * Author: francesco testa
 *
 * Created on 20 novembre 2020, 9.09
 */

// ADC
#include "xc.h"
#include "mySPI.h"
#include "timerFunc.h"
#include "stdio.h"

// manual conversion and sampling read voltage and bits from AN2
int ex1(){
    
    tmr_wait_ms(TIMER1, 1000);
    tmr_setup_period(TIMER2, 200);
    spi_config();
    
    ADCON3bits.ADCS = 8; //TAD value
    ADCON1bits.ASAM = 0; //manual sampling start
    ADCON1bits.SSRC = 0; //manual conersion start
    
    ADCON2bits.CHPS = 0; // number of channel, we need just one so we set 00 (CH0)
    
    //select input 
    ADCHSbits.CH0SA = 2; // selct positive input AN2 as required by the text
    ADCHSbits.CH0NA = 0; // set the negeative value on -Vref , in order to have AN2 w.r.t GROUND 
    
    // we have to set only AN2 to analog 
    ADPCFG = 0xFFFF; // set all register to 1 so digital 
    ADPCFGbits.PCFG2 = 0; //and set only AN2 to analog ( 0 )

    ADCON1bits.ADON = 1; // enable the ADC
    
    int potBits = 0; // varible where I put the bits readed after the conversion
    float potVolts = 0; // variable for read the Voltage readed after the conversion. Of course after the conversion
                        // thr value is digital so I have to do a easy conversion to compute the corresponding voltage 
    char row[17];
    
    while(1)
    {
      ADCON1bits.SAMP = 1; // start sempling 
      tmr_wait_ms(TIMER1,1);// I wait to sample for 1 ms 
      ADCON1bits.SAMP = 0; // stop the sampling 
      
            
      while(ADCON1bits.DONE ==  0); // I wait for the conversion to end
      ADCON1bits.DONE = 0; 
      
      potBits = ADCBUF0 ; // read the result 
      potVolts = potBits * 5.0/1024.0; // conversion from bits to voltage 
      // it's just a proportion 1024:5 = potBits:potVolts
      
      // send to the LCD
      sprintf(row,"P = %4d [bits]",potBits);
      spi_put_char(0x80); // first row
      spi_put_string(row);

      
      sprintf(row,"P = %1.1f [V]",potVolts);
      spi_put_char(0xC0);
      spi_put_string(row);

      
      tmr_wait_period(TIMER2);
    } 
    
    return 0;
}
// like es 1 but automatic conversion and sensor on AD3 
int ex2(){ 
    tmr_wait_ms(TIMER1, 1000);
    tmr_setup_period(TIMER2, 200);
    spi_config();
    
    ADCON3bits.ADCS = 8; //TAD value
    ADCON1bits.ASAM = 0; //manual sampling start
    // set automatic conversion 
    ADCON3bits.SAMC = 1; // so we are gonna convert after 1*TAD 
    ADCON1bits.SSRC = 1; //automatic conersion start
    // quindi stiamo dicendo che il sampling time durerà 1*TAD dopo il quale partirà in automatico il convertitore 
    
    ADCON2bits.CHPS = 0; 
    
    //select input 
    ADCHSbits.CH0SA = 3; // selct positive input AN3 as required by the text
    ADCHSbits.CH0NA = 0; // set the negeative value on -Vref , in order to have AN2 w.r.t GROUND 
    
    // we have to set only AN3 to analog 
    ADPCFG = 0xFFFF; 
    ADPCFGbits.PCFG3 = 0; //and set only AN3 to analog ( 0 )

    ADCON1bits.ADON = 1; // enable the ADC
    
    int tempBits = 0; // varible where I put the bits readed after the conversion
    float tempVolts = 0; // variable for read the Voltage readed after the conversion. Of course after the conversion
                        // thr value is digital so I have to do a easy conversion to compute the corresponding voltage 
    float tempCel=0;
   
    char row[17];
    
    while(1)
    {
      ADCON1bits.SAMP = 1; // start sempling 
      // now dont need anymore to wait and set manually the finish of the sampling 
           
      while(ADCON1bits.DONE ==  0); // I wait for the conversion to end
      ADCON1bits.DONE = 0; 
      
      tempBits = ADCBUF0 ; // read the result 
      tempVolts = tempBits * 5.0/1024.0; // conversion from bits to voltage 
      tempCel = (tempVolts -0.75) * 100.0 + 25;
      // we know that 0.75 volts correspond to 25°C
      // and we know that the scale is 10mV/C, so 1 V correspond to 100 °C
      // so we subtract 25°C (in voltage ) to the tempVolts each volts corresponds to 100 °C and we have readd the 25 
      // that we have remuved at the beginning
      
      // send to the LCD
      sprintf(row,"T = %4d [bits]",tempBits);
      spi_put_char(0x80); // first row
      spi_put_string(row);

      
      sprintf(row,"T = %2.1f [C]",tempCel);
      spi_put_char(0xC0);
      spi_put_string(row);

      
      tmr_wait_period(TIMER2);
    } 
    
    return 0;
}

// here we use sampler and converter both automatic and we read from 2 periferal so we need two channel 
int ex3(){
    
    tmr_wait_ms(TIMER1, 1000);
    tmr_setup_period(TIMER2, 200);
    spi_config();
    
    ADCON3bits.ADCS = 8; //TAD value
    ADCON1bits.ASAM = 1; //automatic sampling start
    // set automatic conversion 
    ADCON3bits.SAMC = 1; 
    ADCON1bits.SSRC = 1; 
    
    ADCON2bits.CHPS = 1; // in this way we are using cha0 and cha1 
    
    //select input 
    ADCHSbits.CH0SA = 2; // selct channle 0 positive for AN2
    ADCHSbits.CH0NA = 0; // set the negeative value on -Vref , in order to have AN2 w.r.t GROUND 
    
    ADCHSbits.CH123SA = 1; // in this way we assign ch1 automatically to AN3 (look at the data sheet) and also other 
    ADCHSbits.CH123NA = 0; // for put to GND ch1 which have just connected to AN3
    
    // we have to set AN3 and AN2 to analog 
    ADPCFG = 0xFFFF; 
    ADPCFGbits.PCFG3 = 0; 
    ADPCFGbits.PCFG2 = 0; //now we have to set also AN2
    
    ADCON1bits.ADON = 1; // enable the ADC
    
    int potBits = 0;
    float potVolts = 0;
    int tempBits = 0; // varible where I put the bits readed after the conversion
    float tempVolts = 0; // variable for read the Voltage readed after the conversion. Of course after the conversion
                        // thr value is digital so I have to do a easy conversion to compute the corresponding voltage 
    float tempCel=0;
   
    char row[17];
    
    while(1)
    {
      // sart sampling is automatically 
      // we can keep the DONE chacking even if is not necessary 
      while(ADCON1bits.DONE ==  0); 
      ADCON1bits.DONE = 0; 
      
      potBits = ADCBUF0 ; 
      potVolts = potBits * 5.0/1024.0; 
      tempBits = ADCBUF1 ; // now is from channel 1 
      tempVolts = tempBits * 5.0/1024.0; // conversion from bits to voltage 
      tempCel = (tempVolts -0.75) * 100.0 + 25;
      // we know that 0.75 volts correspond to 25°C
      // and we know that the scale is 10mV/C so 1 V correspond to 100 °C
      // so we subtract 25°C (in voltage ) to the tempVolts each volts corresponds to 100 °C and we have readd the 25 
      // that we have remuved at the beginning
      
      // send to the LCD
      sprintf(row,"P=%4d T=%4d[bits]",potBits,tempBits);
      spi_put_char(0x80); // first row
      spi_put_string(row);

      
      sprintf(row,"P=%1.1f T=%2.1f [C]",potVolts,tempCel);
      spi_put_char(0xC0);
      spi_put_string(row);

      
      tmr_wait_period(TIMER2);
    } 
    return 0;
}

int main(void) {
    
    ex2();
    
    return 0;
}
