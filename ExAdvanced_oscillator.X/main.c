/*
 * File:   main.c
 * Author: francesco testa
 *
 * Created on 12 ottobre 2020, 10.52
 */


#include "xc.h"
#include "timerDelay.h"

int main(void) {
    
    IFS0bits.T1IF = 0; 
    IFS0bits.T2IF = 0;
    // 
    TRISBbits.TRISB0 = 0; //led 
    TRISEbits.TRISE8 = 1; // button 
    
    int pulseNumber = 0; 
    int periodsCounter = 0; 
    
    int currentButtonStatus = 0;
    int prevButtonStatus = PORTEbits.RE8; // in order to check when it is pressed and realise or not
    int buttonPressCounter = 0; // counts how long the button was hold down 
    
    tar_setup_period(TIMER1 , 1000);
    
    while(1){
        
        periodsCounter++; // increase it's vale at each iteration in order to check when the period is finished 
        currentButtonStatus = PORTEbits.RE8;
        if (currentButtonStatus == 0 && prevButtonStatus == 1){// if the buttin is pressed 1 time then I increase the number of pulses
            pulseNumber++;
            if (pulseNumber >3)
                pulseNumber = 1; 
        }else if(currentButtonStatus == 0 && prevButtonStatus == 0){ //button is still pressed
            buttonPressCounter++; 
        }else if(currentButtonStatus == 1 && prevButtonStatus == 0){ //button has been relased
            if(buttonPressCounter > 30) // because each period is 1000 ms so 3 sec equals to 30 iteration
                pulseNumber = 0;
            buttonPressCounter = 0;
        }
        
        prevButtonStatus = currentButtonStatus; 

    //now I have to set the pulses during the period 
    // if we look at the graph we notice that each pulse must be evrey odd (dispari) period 
    if(periodsCounter %2 == 1) // cheack whether the period is odd 
    {
        //check what period it is 
        if (periodsCounter < pulseNumber*2 ){ // *2 since we have to consider that every pulses is followed by a low state 
        PORTEbits.RE0 = 1; // turn on the led
        }else{
            PORTEbits.RE0 = 0;
        }
    }else{PORTEbits.RE0 = 0;}       
        
    //now check in the periodsCounter is arrived at 1000ms 
        if(periodsCounter == 10)
            periodsCounter = 0; //then I reset it 

    tar_wait_period(TIMER1);
    }
    
    return 0;
}
