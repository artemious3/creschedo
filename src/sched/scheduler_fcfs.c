#include <stdlib.h>
#include "process.h"
#include <stdbool.h>
#include "utils.h"
#include "fifo.h"
#include "scheduler_fcfs.h"


struct scheduler_fcfs{
	struct fifo fifo;
};


static void scheduler_fcfs_report_state(void * vself, prid_t prid, process_state state){
	struct scheduler_fcfs * self = (struct scheduler_fcfs * )(vself);
	switch (state) {
		case ACTIVE:
		case FINISHED:
		case WAIT:
			break;

		case READY:
			fifo_push(&self->fifo, prid);
			break;
	}
}

static prid_t scheduler_fcfs_select(void* vself){
	struct scheduler_fcfs * self = (struct scheduler_fcfs * )(vself);
	if(fifo_empty(&self->fifo)){
		return 0;
	}
	return fifo_pop(&self->fifo);
}

static void scheduler_fcfs_free(void * vself){
	struct scheduler_fcfs * self = (struct scheduler_fcfs * )(vself);
	fifo_free(&self->fifo);
}

struct scheduler scheduler_fcfs_new(){

	struct scheduler_fcfs * data = malloc(sizeof(struct scheduler_fcfs));
	data->fifo = fifo_new(SCHEDULER_FCFS_FIFO_SIZE );

	struct scheduler sched = {
		._data = (void*)data,
		._select = &scheduler_fcfs_select,
		._report_state = &scheduler_fcfs_report_state,
		._free = &scheduler_fcfs_free,
	};

	return sched;

}
