#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "simulation.h"
#include "utils.h"

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
					PANIC("TODO : implement preemptive scheduling");
				}
			}

			process_state new_state = process_tick(proc, proc->cpu_id);

		}
	}
}

bool simulation_process_run(struct simulation *self, struct program prg){
	if(self->min_free_prid == SIMULATION_PROCESS_MAX){
		return false;
	}

	self->processes[self->min_free_prid] = process_new(self->min_free_prid, prg);

	prid_t new_min_free_prid = self->min_free_prid+1;
	while(new_min_free_prid < SIMULATION_PROCESS_MAX && self->processes[new_min_free_prid].prid != 0){
		new_min_free_prid++;
	}
	self->min_free_prid = new_min_free_prid;
	return true;
}

bool simulation_process_remove(struct simulation *self, prid_t prid){
	if(prid >= SIMULATION_PROCESS_MAX || prid == 0 || self->processes[prid].prid == 0){
		return false;
	}
	struct process * proc = &self->processes[prid];

	if(proc->cpu_id >= 0){
		self->cpus[proc->cpu_id].prid = 0;
	}
	process_free(&self->processes[prid]);

	if(prid < self->min_free_prid){
		self->min_free_prid = prid;
	}
	

	return true;
}


