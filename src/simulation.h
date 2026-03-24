#include "process.h"
#include <stdbool.h>

#ifndef SIMULATION_CPU_NUMBER
#define SIMULATION_CPU_NUMBER 4
#endif
#define SIMULATION_PROCESS_MAX 512

struct cpu {
	prid_t prid;
	long t_since_last_sched;
};

struct simulation_event{
	enum simulation_event_type {
		PROCESS_SPAWNED,
		PROCESS_REMOVED,
		PROCESS_CHANGED_STATE,

		CPU_ASSIGNED,
		CPU_RELEASED,
		CPU_TIMER_OVERFLOW,
	} type;

	prid_t prid;
	process_state process_old_state;
	process_state process_new_state;
	int cpuid;
};

struct simulation {
  long t;
  struct process processes[SIMULATION_PROCESS_MAX];

  struct cpu cpus[SIMULATION_CPU_NUMBER];

  prid_t min_free_prid;
	prid_t max_prid;

	long PREEMPT_TICKS;
};

struct simulation simulation_new();

void simulation_tick(struct simulation *self);

bool simulation_process_spawn(struct simulation *self, struct program prg);

bool simulation_process_remove(struct simulation *self, prid_t prid);

void simulation_process_list(struct simulation *self);

void simulation_cpu_list(struct simulation *self);

bool simulation_is_empty(struct simulation *self);

void simulation_free(struct simulation * self);

