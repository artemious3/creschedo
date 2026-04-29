#pragma once
#include "program.h"
#include "metrics.h"
#include <stdbool.h>

typedef unsigned int prid_t;

typedef enum {ACTIVE, READY, WAIT, FINISHED } process_state;

const char * process_state_to_string(process_state state);

struct process {
  prid_t prid;
  process_state state;
  struct program program;
  size_t pc;
  unsigned int priority;
  struct metrics_process metrics;

	// index of cpu + 1, 0 means no cpu
	int cpu_id;
};

 process_state process_tick(struct process * self);

 struct process process_new(long pid, struct program prog, unsigned int priority);

 void process_free(struct process * self);
