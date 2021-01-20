

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

// task functions
void* task_receiver(void* params);
void* task_LCD(void* params);
void* task_PWM_controller(void* params) ;
void* task_temperature_acquisition(void* params);
void* task_send_temperature(void* params) ;
void* task_feedback_ack(void* params);
void* task_LED_blink(void* params) ;
// utilities function
void msg_handler(char* msg_type, char* msg_payload);

#endif	/* XC_HEADER_TEMPLATE_H */

