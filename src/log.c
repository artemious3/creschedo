#include <stdio.h>
#include "log.h"
#include "simulation.h"


void log_simulation_process_flow_line(struct simulation * sim){
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

void log_simulation_cpu_flow_line(struct simulation * sim){
	for(int i = 0; i < SIMULATION_CPU_NUMBER; ++i){
		struct cpu * cpu = &sim->cpus[i];

		if(cpu->prid > 0){
			printf("%-6d", cpu->prid);
	 	} else {
			printf("%-6c",'-');
		}
	}
	printf("\n");
}

void log_simulation_process_flow_header(struct simulation * sim){
	printf("<id> -active on cpu <id>;\nw - waiting;. - ready; F - finished\n");
	for(int i = 1; i <= sim->max_prid; ++i){
		printf("%-4d",i);
	}
	printf("\n");
}

void log_simulation_cpu_flow_header(struct simulation * sim){
	for(int i = 0; i < SIMULATION_CPU_NUMBER; ++i){
		printf("%-6d",i);
	}
	printf("\n");
	for(int i = 0; i < SIMULATION_CPU_NUMBER; ++i){
		printf("------");
	}
	printf("\n");
}

void log_nop(struct simulation * _){}
