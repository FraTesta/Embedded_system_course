

#include "xc.h"
#include "timerFunc.h"
#include "mySPI.h"
#include "stdio.h"
#include "string.h"

/*
 * Application must :
 * - BLink the Led at 1 Hz frequency 
 * - Write on the firt row of the LCD the string "This is a very long string"
 *   Since it is too long we have to slide the string from the right to the left 
 *   continuously  
 */

#define MAX_TASK 3
#define FIRST_ROW 0x80

typedef struct{
    int n; // how many heart beats periods are executed for a specific task
    int N; // number of periods necessary to finish the task i-th
}heart_beat;
// struct which stores 
typedef struct{
    char* longString; // string that we wanna send 
    int startIndex; // where we have to start (perchè dobbiamo tener conto della posizione essendo che la stringa scorre)
    int written; // how many char we have already written
}slidingInfo;

// global variables
heart_beat schedInfo[MAX_TASK];


void scheduler(slidingInfo* siPtr){ // puntatore alla struct sliding info 
    int i;
    int executed = 0;
    for(i = 0; i<MAX_TASK; i++){
        schedInfo[i].n++;
        if (schedInfo[i].n >= schedInfo[i].N){
            switch(i){
                case 0:
                    task1(siPtr);
                    break;
                case 1:
                    task2(siPtr);
                    break;
                case 2:
                    task3(siPtr);
                    break;
                    
            }
            schedInfo[i].n = 0;
        }
    }
}

// write the current character to the LCD
void task1(slidingInfo* si){
    // remark in order to access to the field of a pointer I have to use "->"
    
    if(si->written < 16){// check if the LCD is finished, if it is we won't do anything Becouse it's managed by the task2 
        if( si->startIndex + si->written < strlen(si->longString)){// check if the string is finished 
            
            spi_put_char(si->longString[si->startIndex + si->written]);
            
        }else{ // if it's finished i put a space 
          spi_put_char(' ');  
        }
        si->written++;
    }
}

// LCD slide controller 
void task2(slidingInfo* si){
    
    si->startIndex = (si->startIndex + 1) % strlen(si->longString); // in questo modo quando raggiungeremo il limite (scorre da Dx 
                                                              // vserso Sx il resto della divisione sarà 0 e quindi si riparte
                                                              // dal fondo 
    si->written = 0; // reset the number of written characters 
    spi_put_char(FIRST_ROW);

}

// Led switching 
void task3(slidingInfo* si){
    LATBbits.LATB0 = !LATBbits.LATB0; // turn led D3 on/off
}


int main(void) {
    
    TRISBbits.TRISB0 = 0; // enable the LED
    tmr_wait_ms(TIMER1, 1000);
    tmr_setup_period(TIMER1,5); // 5 ms is the heart beats period (periodo di ogni cilco del while) 
    spi_config();
    slidingInfo si;
    si.longString= "This is a very long string";
    
    // initialize al 
    schedInfo[0].n = 0; 
    schedInfo[1].n = 0; 
    schedInfo[2].n = 0;
    
    // set N according to the time of execution of each tasks
    schedInfo[0].N = 1; // 5 ms
    schedInfo[1].N = 50; // 250 ms 
    schedInfo[2].N = 100; // 500 ms 
    while(1){
        
        scheduler(&si); // vedo passare la struct si per indirizzo per poterla usare nella funzion
        tmr_wait_period(TIMER1);
    }
    
    return 0;
}
