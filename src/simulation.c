#include "simulation.h"
#include <assert.h>

struct simulation simulation_new(){
	struct simulation self = {
		.t = 0,
		.cpus = {{.prid=0, .t_since_last_sched=0}},
		.min_free_prid = 1,
		.max_prid = 0,
		.processes = {{.prid = 0}},
		.PREEMPT_TICKS = 100
	};
	return self;
}

void simulation_tick(struct simulation* self) {
	self->t++;
	for(int i = 1; i < self->max_prid; ++i){
		struct process * proc = &self->processes[i];

		if(proc->prid != 0){
			assert(i == proc->prid);

			if(proc->cpu_id >= 0){
				if(self->PREEMPT_TICKS != 0 && 
					 self->cpus[proc->cpu_id].t_since_last_sched  >= self->PREEMPT_TICKS){
					// call scheduler and assign cpu
				}
			}

			process_tick(proc, proc->cpu_id);
		}
	}
}


void simulation_process_run(struct simulation *self, struct program prg){
}

void simulation_process_remove(struct simulation *self, prid_t prid){
}


