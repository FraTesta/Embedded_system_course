// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MY_ADC_H
#define	MY_ADC_H

#define AN0 0
#define AN1 1
#define AN2 2
#define AN3 3

#include <xc.h> // include processor files - each processor file is guarded.  

// ADC configuration 
void adc_config(int port);
// ADC configuration for 2 channels 
void adc_config_2_chan(int port1, int port2);


#endif /* __cplusplus */