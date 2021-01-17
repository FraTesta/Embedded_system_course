#include "xc.h"
#include "timerFunc.h"
#include "mySPI.h"
#include "stdio.h"
#include "string.h"

/*
 * Advanced ex
 * Application must :
 * 1) When the botton S5 is pressed stop the siding motion 
 * 2) when it's pressed again restart the siding motion 
 * 3) use the potentiometer to change the speed of the sliding from 250ms to 1s
 * 4) After the last character has been displayed, make sure the first char
 *   appears on the right side (So 15 blank spaces must be dispayed before 
 *   the firs char)
 */

#define MAX_TASK 5
#define FIRST_ROW 0x80

typedef struct {
    void * (*task) (void*); // Per dare generalità, faccio un campo puntatore a funzione: 
    // la unzione deve essere un task e il parametro un puntatore a void.
    // In questo modo ogni task avrà anche la sua funzione corrispondente che può avere qualsiasi nome e parametro 
    void* params;
    int n;
    int N;
} heart_beat;

// add the field active 

typedef struct {
    char* longString;
    int startIndex;
    int written;
    int active; // paramiter to stop the sliding function like a boolean variable 
    int blanks; // conta quati spazi vuoti devo lasciare ( per il punto 4) ) 
} slidingInfo;

// field of the buttons 

typedef struct {
    int prevBut;
    int currentBut;
} buttonStatus;

// Sort of container which connects the two type of struct dedicated to the buttons and the sliding in order to make point 1)

typedef struct {
    buttonStatus bs[2]; // which istanciate 2 buttons 
    slidingInfo* si; // and a pointer to a slidingInfo which contain the "active" field 
    // which we need since it is able to stop the sliding task 
} buttonCheckParams;


heart_beat schedInfo[MAX_TASK];

void scheduler() { // puntatore alla struct sliding info 
    int i;
    int executed = 0;
    for (i = 0; i < MAX_TASK; i++) {
        schedInfo[i].n++;
        if (schedInfo[i].n >= schedInfo[i].N) {
            // ora non ho più bisogno dello switch perchè è tutto integrato nella struct
            schedInfo[i].task(schedInfo[i].params); // chiamo la funzione del task i-esimo passandogli il suo i-esimo 
            // pramas = &si 
            schedInfo[i].n = 0;
        }
    }
}

////////////////////////////////////////////////////// TASK //////////////////////////////////////////////////////////////////
// RISCRIVE LE FUNZIONI COME PUNTATORI 

void* task1(void* params) {// passo un param generico (che nel nostro caso sarà un puntatore a "si" come nel es normale )
    slidingInfo* si = (slidingInfo*) params; // prendo il parametro e faccio il casting a tipo slidingInfo 

    if (si->written < 16) {
        if (si->written < si->blanks) { // se il numero di spazzi bianchi da lasciare è più grande del numero di lettere scritte 
            // vedi prima task2 
            spi_put_char(' ');// lascio uno spazio per il punto 5) 
            
            //now I have to modify also the following  IF
        }else if (si->startIndex + si->written - si->blanks < strlen(si->longString)) { // ora NON devo considerare gli spazzi vuoti
                                                                                        // che ho aggiunto  

            spi_put_char(si->longString[si->startIndex + si->written - si->blanks]);

        } else {
            spi_put_char(' ');
        }
        si->written++;
    }
    return NULL;
}

void* task2(void* params) {
    slidingInfo* si = (slidingInfo*) params;
    if (si->active == 1) {
        if (si->blanks == 0) {// se non devo lasciare spazzi allora faccio scorrere il messaggio normalmente 
            si->startIndex = (si->startIndex + 1) % strlen(si->longString);
            if (si->startIndex == 0) {// so if the last char has been displayed and we have to restart the message 
                si->blanks = 16; // rimetto al massimo il numero di spazzi da lasciare per il punto 5) 
            }
        } else {
            si->blanks--;
        }
    }
    si->written = 0;
    spi_put_char(FIRST_ROW);

    return NULL;
}

// Led switching 
void* task3(void* params) {
    slidingInfo* si = (slidingInfo*) params;
    LATBbits.LATB0 = !LATBbits.LATB0;
    return NULL;
}

// task which check if some button are pressed , point 1) 

void* task_check_buttons(void* params) {
    buttonCheckParams* buttonParam = (buttonCheckParams*) params; // faccio il cast come prima
    buttonParam->bs[0].currentBut = PORTEbits.RE8;
    if (buttonParam->bs[0].currentBut == 0 && buttonParam->bs[0].prevBut == 1) {
        buttonParam->si->active = !buttonParam->si->active;
    }
    buttonParam->bs[0].prevBut = buttonParam->bs[0].currentBut;
    return NULL;
}

// task for point 3)
void* task_sliding_speed(void* params){
    heart_beat* sched = (heart_beat*)params;
    while(ADCON1bits.DONE == 0); // I wair for the conversion to end 
    int portValue = ADCBUF0; //read the result of conversion 
    sched[1].N = 50 + portValue/1024.0 * 150 ;// cambio la velocità del task 1 
    // perchè 50 + 150 = 200 e 200*5 ms = 1s
}

/////////////////////////////////////////////////////////// MAIN ////////////////////////////////////////////////////////////////////////////

void adc_config();

int main(void) {

    TRISBbits.TRISB0 = 0;
    TRISEbits.TRISE8 = 1; // set the button S5 in read mode  
    tmr_wait_ms(TIMER1, 1000);
    tmr_setup_period(TIMER1, 5);
    spi_config();
    adc_config();

    slidingInfo si;
    si.active = 1; // initialize the active field 
    si.written = 0;
    si.startIndex = 0;
    si.blanks = 0;
    si.longString = "This is a very long string";
    
    buttonCheckParams buttonParams; // struct condivisa bottone e sliding per camciare l'active quando il bottone è premuto 
    buttonParams.si = &si;



    // ora posso assegnare pure le funzioni al campo task degli si  
    schedInfo[0].task = &task1;
    schedInfo[1].task = &task2;
    schedInfo[2].task = &task3;
    schedInfo[3].task = &task_check_buttons; // we have to add also a new task in the scheduling 
    schedInfo[4].task = &task_sliding_speed;

    /* setto anche i param da passare alla varie funz 
    che sono quasi tutti si tranne l'ultimo */
    schedInfo[0].params = &si;
    schedInfo[1].params = &si;
    schedInfo[2].params = NULL; // since it has no params
    schedInfo[3].params = &buttonParams;
    schedInfo[4].params = &schedInfo; 

    // initialize al 
    schedInfo[0].n = 0;
    schedInfo[1].n = 0;
    schedInfo[2].n = 0;
    schedInfo[3].n = 0;
    schedInfo[4].n = 0;

    // set N according to the time of execution of each tasks
    schedInfo[0].N = 1; // 5 ms
    schedInfo[1].N = 50; // 250 ms 
    schedInfo[2].N = 100; // 500 ms 
    schedInfo[3].N = 4; // 20 ms since it must be quite frequent
    schedInfo[4].N = 4; // 20 ms as well 
    
    while (1) {

        scheduler(); // ora non devo più passare &si allo scheduler 
        tmr_wait_period(TIMER1);
    }

    return 0;
}

void adc_config(){// config the adc to use the potentiometer 
    ADCON3bits.ADCS = 8; //TAD value
    // automatin sampling start & set automatic conversion start
    ADCON1bits.ASAM = 0; 
    ADCON3bits.SAMC = 1; //  1*TAD 
    ADCON1bits.SSRC = 7; //automatic conersion start
    // channel 0 only 
    ADCON2bits.CHPS = 0; 
    //AN2 w.r.t GND 
    ADCHSbits.CH0SA = 2; // selct positive input AN2 potentiometer
    ADCHSbits.CH0NA = 0; 
    // just AN2 as analog 
    ADPCFG = 0xFFFF;
    ADPCFGbits.PCFG2 = 0;
    //turn on
    ADCON1bits.ADON = 1;
}