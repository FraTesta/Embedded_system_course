/*
 * File:   main.c
 * Author: francesco testa
 *
 * Created on 21 novembre 2020, 18.20
 */

// PWM
#include "xc.h"
#include "timerFunc.h"
#include "mySPI.h"
#include "stdio.h"

void setADC(){
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
}

int main(void) {
    
 tmr_wait_ms(TIMER1, 1000);
    tmr_setup_period(TIMER2, 200);
    spi_config();
    setADC();
    // configuration PWM
    
    /* PTPER = (FCY / Fpwm)-1 = (1843200/50)-1 = 36863
     which is a to big number for a 15 bits register, so I have to set a prescaler
     */
    PTCONbits.PTCKPS = 1; // prescaler pf 1:4
    PTPER = 9215; // which is (1843200/(50*4)) - 1
    PTCONbits.PTMOD = 0; // free running mode
    PWMCON1bits.PEN2H = 1; // set as positive PWM pin 2
    PTCONbits.PTEN = 1; //anable the PWM module

    
    
    int potBits = 0; // varible where I put the bits readed after the conversion
    char row[17];
    
    while(1)
    {
      ADCON1bits.SAMP = 1; // start sempling 
      tmr_wait_ms(TIMER1,1);// I wait to sample for 1 ms 
      ADCON1bits.SAMP = 0; // stop the sampling         
      while(ADCON1bits.DONE ==  0); // I wait for the conversion to end
      ADCON1bits.DONE = 0; 
      
      potBits = ADCBUF0 ; // read the result 
      
      /* if the potentiometer is 0 -> PWM  1 ms
       *  if the potentiometer is 1023 -> PWM  2 ms
       * being the period of the PWM 20 ms Then:
       *  1 ms correspond to a duty cycle of 5%
       * and 2 ms correspond to a duty cycle of 10% 
       * 
      */
      
      double dutyCycle = 0.05 + 0.05*(potBits/1024); // so starting from 0 (0.05) and then adding the value mesured 
      PDC2 = 2* PTPER * dutyCycle; // set when the timer must report to put low the level :
      //so twice the period, multiply by the percentage of the dutyCycle
      
      
      // send to the LCD
      sprintf(row,"DC = %3.0f %%",dutyCycle*100);
      spi_put_char(0x80); // first row
      spi_put_string(row);


      
      tmr_wait_period(TIMER2);
    } 
    
    return 0;
}