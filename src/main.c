#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shell.h"
#include "program.h"
#include "simulation.h"
#include "scheduler_fcfs.h"
#include "utils.h"
#include "log.h"

static const char * HELP_MSG =
"@<filename>      -- execute all commands from a file <filename>\n"
"spawn <prio>     -- spawn process with priority <prio>;\n"
"                    You will be asked to input a program;\n"
"                    program consists of lines `<N> <Action>`,\n"
"                    where <N> is a positive number,\n"
"                    <Action> is one of `r` - run, `w` - wait.\n"
"                    End program with an empty line.\n"
"kill <prid>      -- kill process with id <prid>\n"
"cpu              -- show table of CPUs\n"
"ps               -- show table of processess\n"
"tick             -- invoke simulation tick\n"
"run <log_option> -- invoke ticks until all processes are finished\n"
"                    log options are:\n"
"                    `show_cpu` -- show state of cpus after each tick\n"
"                    `show_proc` -- show state of processes after each tick\n"
"clear            -- reset simulation\n"
"metrics          -- show metrics\n"
"help             -- show this help message\n";


int parse_positive_int(const char * s, bool * err){
	if(err == NULL){
		PANIC("err must not be null");
	}

	*err = false;

	if(s == NULL){
		*err = true;
		return 0;
	}

	char* end;
	long result = strtol(s, &end , 10);
	if(result > (long)INT_MAX || result < (long)INT_MIN){
		*err = true;
		return 0;
	}
	if(end == s || *end != '\0' || result <= 0){
		*err = true;
		return 0;
	}
	return result;
}

program_op parse_op(const char * s, bool * err){
	if(s == NULL || s[0] == '\0' || s[1] != '\0'){
		*err = true;
		return 0;
	}

	switch(s[0]){
		case 'r':
		case 'R':
			return OP_RUN;
		case 'w':
		case 'W':
			return OP_WAIT;
		default:
			*err = true;
			return 0;
	}
}

static int run_process(void * ctx, FILE * istream, const char * args[SHELL_ARGS_MAX]){
	struct simulation * sim = (struct simulation *)(ctx);

	bool err;
	if(args[0] == NULL){
   	printf("error: usage `spawn <PRIORITY>`\n");
    return 1;
	}
	int prio = parse_positive_int(args[0], &err);
	if(err){
  	printf("error: invalid priority\n");
    return 1;
	}

	char line[16];
	struct program prg = program_new();

	if(istream == stdin){
		eprintf("program> ");
	}

	while ( fgets(line, 15, istream) != NULL && line[0] != '\n' ){
		struct shell_input input = shell_split_into_words(line);
		bool err;

		long count = parse_positive_int(input.words[0], &err);
		if(err){
			eprintf("error: bad op count\nprogram> ");
			continue;
		}

		program_op op = parse_op(input.words[1], &err);
		if(err){
			eprintf("error: bad op\nprogram> ");
			continue;
		}

		program_append_ops(&prg, count, op);

		if(istream == stdin){
			eprintf("program> ");
		}
	}

	if(prg.length == 0){
		eprintln("error: program is empty, process was not spawned");
		program_free(&prg);
	} else {
		simulation_process_spawn(sim, prg, prio);
	}


	return 0;
}


static int process_list(void * ctx, FILE * _, const char * args[SHELL_ARGS_MAX]){
	struct simulation * sim = (struct simulation *)(ctx);
	simulation_process_list(sim);
	return 0;
}

static int process_kill(void * ctx, FILE * _, const char * args[SHELL_ARGS_MAX]){
	struct simulation * sim = (struct simulation *)(ctx);
	bool err;
	int prid = parse_positive_int(args[0], &err);
	if(err){
		eprintf("error: invalid pid");
	}

	bool killed = simulation_process_kill(sim, prid);
	if(!killed){
		eprintf("error: process with given prid does not exist");
	}

	eprintf("process %d killed", prid);
	return 0;
}

static int cpu_list(void * ctx, FILE * _, const char * args[SHELL_ARGS_MAX]){
	struct simulation * sim = (struct simulation *)(ctx);
	simulation_cpu_list(sim);
	return 0;
}

static int sim_tick(void * ctx, FILE * _, const char * args[SHELL_ARGS_MAX]){
	struct simulation * sim = (struct simulation *)(ctx);
	simulation_tick(sim);
	return 0;
}

static int sim_run(void * ctx, FILE * _, const char * args[SHELL_ARGS_MAX]){
	struct simulation * sim = (struct simulation *)(ctx);
	long ticks_before = sim->t;
	sim->LOG_ENABLED = true;

	log_callback_t log = log_nop;
	if(args[0] != NULL){
		if( strcmp(args[0], "show_cpu") == 0){
			log_simulation_cpu_flow_header(sim);
			log = log_simulation_cpu_flow_line;
			sim->LOG_ENABLED = false;
		} else if( strcmp(args[0], "show_proc") == 0){
			log_simulation_process_flow_header(sim);
			log = log_simulation_process_flow_line;
			sim->LOG_ENABLED = false;
		} else if (strcmp(args[0], "show_queues") == 0) {
  		log = log_simulation_sched_queues;
		} else {
			eprintln("warning: unkown logging option %s", args[0]);
		}
	}

	while(!simulation_is_finished(sim)){
		simulation_tick(sim);
		log(sim);
	}
	eprintln("simulation finished in %ld ticks", sim->t - ticks_before);
	return 0;
}

static int show_metrics(void * ctx, FILE * _, const char * args[SHELL_ARGS_MAX]){
  struct simulation * sim = (struct simulation *)(ctx);

  bool show_process_metrics = false;
  bool show_cpu_metrics = false;

  if(args[0] == NULL || args[0][0] == '\0'){
    printf("Showing all metrics\n");
    show_process_metrics = true;
    show_cpu_metrics = true;
  } else if (strcmp(args[0], "proc") == 0){
    show_process_metrics = true;
  } else if (strcmp(args[0], "cpu") == 0){
    show_cpu_metrics = true;
  } else printf("Unknown metrics type: %.5s", args[0]);

  if(show_process_metrics){
    printf("%-6s%-4s%-4s%-4s%-20s%-20s\n", "ID", "A", "R", "W", "Times Blocked", "Times Preempted");
    for (int i = 1; i <= sim->max_prid; ++i ) {
      struct process * p = &sim->processes[i];
      if(p->prid != 0){
        printf("%-6d%-4d%-4d%-4d%-20d%-20d\n", p->prid,
          p->metrics.active_ticks, p->metrics.ready_ticks, p->metrics.wait_ticks,
          p->metrics.blocked_times, p->metrics.preempted_times);
      }
    }
  }

  if(show_cpu_metrics){
    printf("*unimplemented*");
  }

  return 0;
}


int sim_clear(void * ctx, FILE * _, const char * __[SHELL_ARGS_MAX]){
  struct simulation * sim = (struct simulation *)(ctx);
  simulation_free(sim);
  *sim = simulation_new(scheduler_fcfs_new());
  return 0;
}

int show_help(void * ctx, FILE * _, const char * __[SHELL_ARGS_MAX]){
  printf("%s", HELP_MSG);
  return 0;
}


int main(){
	eprintln("CreSchedo - OS Scheduler Simulator.");
	eprintln("Run `help` to get list of allowed commands");
	struct simulation sim = simulation_new(scheduler_fcfs_new());
	struct shell sh = shell_new((void*)&sim);
	shell_register_callback(&sh, "spawn", run_process);
	shell_register_callback(&sh, "kill", process_kill);
	shell_register_callback(&sh, "cpu", cpu_list);
	shell_register_callback(&sh, "ps", process_list);
	shell_register_callback(&sh, "tick", sim_tick);
	shell_register_callback(&sh, "run", sim_run);
	shell_register_callback(&sh, "clear", sim_clear);
	shell_register_callback(&sh, "metrics", show_metrics);
	shell_register_callback(&sh, "help", show_help);
	shell_start(&sh, stdin);
	simulation_free(&sim);
}
