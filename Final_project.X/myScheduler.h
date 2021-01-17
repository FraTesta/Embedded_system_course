// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include "stdio.h"

typedef struct {
    void * (*task) (void*); 
    void* params;
    int n;
    int N;
} heart_beat;

void scheduler(int max_task, void* sched);


#endif /* __cplusplus */