// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MY_TIMER_H
#define	MY_TIMER_H

#define TIMER1 1
#define TIMER2 2

#include <xc.h> // include processor files - each processor file is guarded.  

void tmr_wait_ms(int timer,int ms);
void tmr_setup_period(int timer , int ms);
void tmr_wait_period(int timer);

#endif /* __cplusplus */


