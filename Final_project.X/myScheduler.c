

#include "myScheduler.h"




void scheduler(int max_task, void* sched){
    heart_beat* schedInfo = (heart_beat*) sched;
    int i;
    for (i = 0; i < max_task; i++) {
        schedInfo[i].n++;
        if (schedInfo[i].n >= schedInfo[i].N) {

            schedInfo[i].task(schedInfo[i].params); 

            schedInfo[i].n = 0;
        }
    }
}



