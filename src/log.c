#include <stdio.h>
#include <string.h>
#include "log.h"
#include "simulation.h"
#include "scheduler_fcfs.h"
#include "fifo.h"


void log_simulation_process_flow_line(const struct simulation * sim){
	for(int i = 1; i <= sim->max_prid; ++i){
		if(sim->processes[i].prid != 0){
			switch(sim->processes[i].state){
				case ACTIVE:
					printf("%-4d",sim->processes[i].cpu_id);
					break;
				case WAIT:
					printf("%-4c",'w');
					break;
				case READY:
					printf("%-4c",'.');
					break;
				case FINISHED:
					printf("%-4c",'F');
					break;
			}
	 	} else {
			printf("%-4c", ' ');
		}
	}
	printf("\n");
}

void log_simulation_cpu_flow_line(const struct simulation * sim){
	for(int i = 0; i < SIMULATION_CPU_NUMBER; ++i){
		const struct cpu * cpu = &sim->cpus[i];

		if(cpu->prid > 0){
			printf("%-6d", cpu->prid);
	 	} else {
			printf("%-6c",'-');
		}
	}
	printf("\n");
}

void log_simulation_process_flow_header(const struct simulation * sim){
	printf("<id> -active on cpu <id>;\nw - waiting;. - ready; F - finished\n");
	for(int i = 1; i <= sim->max_prid; ++i){
		printf("%-4d",i);
	}
	printf("\n");
}

void log_simulation_cpu_flow_header(const struct simulation * sim){
	for(int i = 0; i < SIMULATION_CPU_NUMBER; ++i){
		printf("%-6d",i);
	}
	printf("\n");
	for(int i = 0; i < SIMULATION_CPU_NUMBER; ++i){
		printf("------");
	}
	printf("\n");
}


void log_simulation_sched_queues(const struct simulation * sim){
  if(strcmp(sim->sched.name, "fcfs") != 0){
    printf("UNSUPPORTED SCHEDULER TYPE : %.15s\n", sim->sched.name);
    return;
  }

  printf("======== TICK %ld ========\n", sim->t);
  const struct scheduler_fcfs * sched_data = (struct scheduler_fcfs*)(sim->sched._data);

  for(int i = 0; i < SCHEDULER_FCFS_PRIORITIIES; ++i ){
    printf("PRIO %-2d @ ", i);
    const struct fifo * fifo = &sched_data->fifo[i];

    const struct scheduler_process_descriptor * pd = fifo->head;

    for(int i = 0; i < fifo->len; ++i){
      printf("%4d| ", pd->prid);
      pd++;
      if(pd == fifo->mem + fifo->capacity){
        pd = fifo->mem;
      }
    }
    printf("\n");
  }
  printf("\n");
}



void log_nop(const struct simulation * _){}
