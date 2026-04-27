#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "simulation.h"
#include "process.h"
#include "utils.h"

#define PROCESS_EXISTS(proc) ((proc).prid != 0)

#define PROCESS_IS_CPU_ASSIGNED(proc) ((proc).cpu_id >= 0)

#define CPU_IS_BUSY(cpu) ((cpu).prid != 0)
#define CPU_IS_IDLE(cpu) (!CPU_IS_BUSY((cpu)))

#define NO_CPU (-1)

//TODO : replace all simulation logs with `simulation_event` calls,


// #define SIMULATION_LOG(tag,format,...) \
// 	fprintf(stderr, "[%6ld] [%10s] " format "\n", self->t, tag  __VA_OPT__(,) __VA_ARGS__);
//
#define SIMULATION_LOG(tag,format,...)

static bool simulation_process_remove(struct simulation *self, prid_t prid);

static void simulation_report_event(struct simulation * self, struct simulation_event ev){

	switch (ev.type) {
		case PROCESS_SPAWNED:
			SIMULATION_LOG("PROCESS","prid spawned");
		  if(self->processes[ev.prid].state == READY){
				struct scheduler_process_descriptor pd = {
				.prid = ev.prid,
				.priority = 0, // TEMPORARY
				.t_since = self->t
				};
				scheduler_report_ready(&self->sched, pd);
			}
			break;
		case PROCESS_REMOVED:
			SIMULATION_LOG("PROCESS", "prid %d removed", ev.prid);
			break;
		case PROCESS_CHANGED_STATE:
			SIMULATION_LOG("PROCESS", "prid %d changed state : %s -> %s",
					ev.prid,
					process_state_to_string(ev.process_old_state),
					process_state_to_string(ev.process_new_state));
			if(self->processes[ev.prid].state == READY){
  			struct scheduler_process_descriptor pd = {
  			.prid = ev.prid,
  			.priority = 0, // TEMPORARY
  			.t_since = self->t
  			};
  			scheduler_report_ready(&self->sched, pd);
  		}
			break;

		case CPU_TIMER_OVERFLOW:
			SIMULATION_LOG("CPU", "cpuid %d timer overflow (process %d)", ev.cpuid, ev.prid);
			break;
		case CPU_ASSIGNED:
			SIMULATION_LOG("CPU", "cpuid %d assigned to prid %d", ev.cpuid, ev.prid);
			break;
		case CPU_RELEASED:
			SIMULATION_LOG("CPU", "cpuid %d released", ev.cpuid);
			break;
	}
}

static prid_t simulation_DBG_sched_first(struct simulation * self){
	for(int i = 1; i <= self->max_prid; ++i){
		struct process * proc = &self->processes[i];
		if(PROCESS_EXISTS(*proc) && !PROCESS_IS_CPU_ASSIGNED(*proc) && proc->state == READY){
			return i;
		}
	}
	return 0;
}

struct simulation simulation_new(struct scheduler sched){
	struct simulation self = {
		.t = 0,
		.cpus = {{.prid=0, .t_since_last_sched=0}},
		.min_free_prid = 1,
		.max_prid = 0,
		.processes = {{.prid = 0}},
		.PREEMPT_TICKS = 100,
		.sched = sched
	};
	return self;
}

prid_t simulation_schedule_process(struct simulation *self){
	prid_t prid = scheduler_select(&self->sched);
	while(prid != 0 && !PROCESS_EXISTS(self->processes[prid])){
		// when process is killed, it's not removed from scheduler
		// instead we check if scheduler selected process that still exists
		prid = scheduler_select(&self->sched);
	}
	return prid;
}

static void simulation_cpu_assign(struct simulation * self, int cpu_id, prid_t prid){
	if(prid <= 0 || prid >= SIMULATION_PROCESS_MAX){
		PANIC("invalid process id");
	}
	if(!PROCESS_EXISTS(self->processes[prid])){
		PANIC("process does not exist");
	}
	if(cpu_id < 0 || cpu_id > SIMULATION_CPU_NUMBER){
		PANIC("invalid cpu id");
	}
	if(CPU_IS_BUSY(self->cpus[cpu_id])){
		PANIC("cpu must be idle in time of assignment");
	}
	if(PROCESS_IS_CPU_ASSIGNED(self->processes[prid])){
		PANIC("cpu is already assigned to given process");
	}

	self->cpus[cpu_id].prid = prid;
	self->processes[prid].cpu_id = cpu_id;

	struct simulation_event ev = {.type = CPU_ASSIGNED, .cpuid = cpu_id, .prid = prid};
	simulation_report_event(self, ev);
}

static void simulation_cpu_release(struct simulation * self, int cpu_id){

	if(cpu_id < 0 || cpu_id > SIMULATION_CPU_NUMBER){
		PANIC("invalid cpu id");
	}
	if(CPU_IS_IDLE(self->cpus[cpu_id])){
		PANIC("cpu must is already idle");
	}

	int older_prid = self->cpus[cpu_id].prid;
	self->processes[older_prid].cpu_id = NO_CPU;
	self->cpus[cpu_id].prid = 0;

	struct simulation_event ev = {.type = CPU_RELEASED, .cpuid = cpu_id, .prid = older_prid};
	simulation_report_event(self, ev);
}


void simulation_tick(struct simulation* self) {

	for(int i = 1; i <= self->max_prid; ++i){
		struct process * proc = &self->processes[i];
		if(PROCESS_EXISTS(*proc)){
			assert(i == proc->prid);

			// remove process 1 tick later after
			// it has finished
			if(proc->state == FINISHED){
				simulation_process_remove(self, i);
				continue;
			}

			process_state old_state = proc->state;
			process_state new_state = process_tick(proc);
			if(old_state != new_state){
						struct simulation_event ev = {
							.type = PROCESS_CHANGED_STATE,
							.prid = i,
							.process_old_state = old_state,
							.process_new_state = new_state
						};
						simulation_report_event(self, ev);

			}
			if((new_state == FINISHED || new_state == WAIT) && PROCESS_IS_CPU_ASSIGNED(*proc)){
				simulation_cpu_release(self, proc->cpu_id);
			}
		}
	}

	for(int i = 0; i < SIMULATION_CPU_NUMBER; ++i){

		if(CPU_IS_IDLE(self->cpus[i])){
			prid_t new_prid = simulation_schedule_process(self);
			if(new_prid > 0){
				// TODO : make separate function, like `simulation_schedule`
				simulation_cpu_assign(self, i, new_prid);
				process_state state = process_tick(&self->processes[new_prid]);
				if(state != ACTIVE){
					eprintln("warning: scheduler selected process %d, but its next state was %s", new_prid, process_state_to_string(state))
				}
			}


		} else if (self->cpus[i].t_since_last_sched >= self->PREEMPT_TICKS){
			struct simulation_event ev = {.type = CPU_TIMER_OVERFLOW, .cpuid = i, .prid = self->cpus[i].prid};
			simulation_report_event(self, ev);

			simulation_cpu_release(self, i);
			prid_t new_prid = simulation_schedule_process(self);
			if(new_prid > 0){
				simulation_cpu_assign(self, i, new_prid);
			}
		}
	}

	self->t++;
}

bool simulation_process_spawn(struct simulation *self, struct program prg){
	if(self->min_free_prid == SIMULATION_PROCESS_MAX){
		return false;
	}

	prid_t new_prid = self->min_free_prid;

	self->processes[new_prid] = process_new(self->min_free_prid, prg);

	if(self->max_prid < new_prid){
		self->max_prid = new_prid;
	}

	prid_t new_min_free_prid = self->min_free_prid+1;
	while(new_min_free_prid < SIMULATION_PROCESS_MAX &&
			PROCESS_EXISTS(self->processes[new_min_free_prid])){
		new_min_free_prid++;
	}
	self->min_free_prid = new_min_free_prid;
	struct simulation_event ev = {.type = PROCESS_SPAWNED, .prid = new_prid};
	simulation_report_event(self, ev);

	return true;
}


bool simulation_process_kill(struct simulation *self, prid_t prid){
	if(prid >= SIMULATION_PROCESS_MAX || prid == 0 || self->processes[prid].prid == 0){
		return false;
	}
	struct simulation_event ev = {
		.type = PROCESS_CHANGED_STATE,
		.prid = prid,
		.process_old_state = self->processes[prid].state,
		.process_new_state = FINISHED
	};
	self->processes[prid].state = FINISHED;
	simulation_report_event(self, ev);
	return true;
}

static bool simulation_process_remove(struct simulation *self, prid_t prid){
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

	prid_t new_max_prid = self->max_prid;
	while(new_max_prid > 0 && !PROCESS_EXISTS(self->processes[new_max_prid])){
		new_max_prid--;
	}
	self->max_prid = new_max_prid;
	struct simulation_event ev = {.type = PROCESS_REMOVED, .prid = prid};
	simulation_report_event(self, ev);

	return true;
}

void simulation_process_list(struct simulation *self){
	printf("%-6s%-10s%-10s%-8s%-20s\n", "ID", "State", "CPU", "PC", "Program Length");

	for(int i = 1; i <= self->max_prid; ++i){
		struct process * proc = &self->processes[i];
		if(PROCESS_EXISTS(*proc)){
			printf("%-6d%-10s%-10d%-8zu%-20zu\n",
					i,
					process_state_to_string(proc->state),
					proc->cpu_id,
					proc->pc,
					proc->program.length);
		}
	}

}

void simulation_cpu_list(struct simulation *self){
	printf("%-6s%-10s%-30s\n", "CPU", "PRID", "Time since last schedule");
	for(int i = 0; i < SIMULATION_CPU_NUMBER; ++i){
		printf("%-6d%-10d%-30ld\n", i, self->cpus[i].prid, self->cpus[i].t_since_last_sched);
	}
}

bool simulation_is_empty(struct simulation *self){
	return self->max_prid == 0;
}


void simulation_free(struct simulation * self){
	for(int i = 1; i <= self->max_prid; ++i){
		struct process * proc = &self->processes[i];
		if(PROCESS_EXISTS(*proc)){
			process_free(proc);
		}
	}
	scheduler_free(&self->sched);
}
