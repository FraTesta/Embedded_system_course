#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#define TIMER1 1
#define TIMER2 2

#include <xc.h> // include processor files - each processor file is guarded.  

void tmr_wait_ms(int timer,int ms);
void tmr_setup_period(int timer , int ms);
void tmr_wait_period(int timer); // it's used to syncronize: for istance in while loop I usually do some other instructions 
                                //which spend some time. And if I want a timer that takes into account the time spends from
                                // those instruction I have to use this instead of tmr_wait_ms which just stop the program for 
                                // certain ms.

#endif /* __cplusplus */

