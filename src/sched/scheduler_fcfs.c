#include <stdlib.h>
#include "process.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "fifo.h"
#include "scheduler_fcfs.h"


static void scheduler_fcfs_report_ready(void * vself, struct scheduler_process_descriptor pd){
	struct scheduler_fcfs * self = (struct scheduler_fcfs * )(vself);
	if(pd.priority >= SCHEDULER_FCFS_PRIORITIIES ){
        PANIC("Invalid priority : %ud", pd.priority);
	}
	fifo_push(&self->fifo[pd.priority], pd);
}

static prid_t scheduler_fcfs_select(void* vself){
	struct scheduler_fcfs * self = (struct scheduler_fcfs * )(vself);
	// TODO: implement `t_since` checker to propagate long-waiting processes
	//       to higher priority queues
	for(int prio = 0; prio < SCHEDULER_FCFS_PRIORITIIES; ++prio){
	    if(!fifo_empty(&self->fifo[prio])){
			return fifo_pop(&self->fifo[prio]).prid;
		}
	}

	return 0;
}

static void scheduler_fcfs_free(void * vself){
	struct scheduler_fcfs * self = (struct scheduler_fcfs * )(vself);
	for(int i = 0; i < SCHEDULER_FCFS_PRIORITIIES; ++i){
		fifo_free(&self->fifo[i]);
	}
}

struct scheduler scheduler_fcfs_new(){

	struct scheduler_fcfs * data = malloc(sizeof(struct scheduler_fcfs));
	for(int i = 0; i < SCHEDULER_FCFS_PRIORITIIES; ++i){
    	data->fifo[i] = fifo_new(SCHEDULER_FCFS_FIFO_SIZE );
	}

	struct scheduler sched = {
	  .name = {0},
		._data = (void*)data,
		._select = &scheduler_fcfs_select,
		._report_ready = &scheduler_fcfs_report_ready,
		._free = &scheduler_fcfs_free,
	};
	strcpy(sched.name, "fcfs");
	return sched;

}
