
#include "process.h"
#include <stdbool.h>

#define SIMULATION_CPU_NUMBER 8
#define SIMULATION_PROCESS_MAX 512

struct cpu {
	pid_t prid;
	long t_since_last_sched;
};

struct simulation {
  long t;
  struct process processes[SIMULATION_PROCESS_MAX];

  struct cpu cpus[SIMULATION_CPU_NUMBER];

  pid_t min_free_prid;
	pid_t max_prid;

	long PREEMPT_TICKS;
};

struct simulation simulation_new();

void simulation_tick(struct simulation *self);

bool simulation_process_run(struct simulation *self, struct program prg);

bool simulation_process_remove(struct simulation *self, prid_t prid);


