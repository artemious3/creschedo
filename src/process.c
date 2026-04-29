#include "process.h"
#include "utils.h"
#include <stdio.h>

const char * process_state_to_string(process_state state){
	switch (state) {
		case ACTIVE:
			return "ACTIVE";
		case READY:
			return "READY";
		case WAIT:
			return "WAIT";
		case FINISHED:
			return "FINISHED";
	}
}

struct process process_new(long pid, struct program prog, unsigned int priority){
	struct process self = {
		.prid = pid,
		.cpu_id = -1,
		.pc = 0,
		.state = READY,
		.program = prog,
		.priority = priority,
		.metrics = {0}
	};
	return self;
}

void process_free(struct process * self){
	program_free(&self->program);
	self->prid = 0;
}

 process_state process_tick(struct process *self) {
       if (self->state == FINISHED) return FINISHED;
       if (self->pc >= self->program.length) {
           self->state = FINISHED;
           return FINISHED;
       }

       switch (self->state) {
         case ACTIVE: self->metrics.active_ticks++;
         case WAIT: self->metrics.wait_ticks++;
         case READY: self->metrics.ready_ticks++;
         case FINISHED: break;
       }

       program_op op = self->program.ops[self->pc];

       if (self->cpu_id >= 0 && op == OP_RUN) {
           self->pc++; // Process uses CPU
       } else if (op == OP_WAIT) {
           self->pc++; // I/O progresses in background
       }

       // Refresh op after possible increment to check next state
       // if (self->pc >= self->program.length) {
       //     self->state = FINISHED;
       //     return;
       // }
       // op = self->program.ops[self->pc];


       // 2. State Transition Logic
       if (self->cpu_id >= 0) {

           if (self->state == READY) self->state = (op == OP_WAIT) ? WAIT : ACTIVE;
           else if (self->state == ACTIVE && op == OP_WAIT) {
             self->state = WAIT;
             self->metrics.blocked_times++;
           }
           else if (self->state == WAIT && op == OP_RUN) self->state = ACTIVE;

       } else {
           if (self->state == ACTIVE || self->state == READY) {
             if(self->state == ACTIVE){
               self->metrics.preempted_times++;
             }
             self->state = (op == OP_WAIT) ? WAIT : READY;
           }
           else if (self->state == WAIT && op == OP_RUN) self->state = READY;
       }

			 return self->state;
   }
