#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "shell.h"
#include "program.h"
#include "simulation.h"
#include "utils.h"

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

static int run_process(void * ctx, const char * args[SHELL_ARGS_MAX]){
	struct simulation * sim = (struct simulation *)(ctx);
	char line[16];
	struct program prg = program_new();
	
	eprintf("program> ");
	while ( fgets(line, 15, stdin) != NULL && line[0] != '\n' ){
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
		eprintf("program> ");
	}

	simulation_process_run(sim, prg);

	return 0;
}


static int process_list(void * ctx, const char * args[SHELL_ARGS_MAX]){
	struct simulation * sim = (struct simulation *)(ctx);
	simulation_process_list(sim);
	return 0;
}

static int process_remove(void * ctx, const char * args[SHELL_ARGS_MAX]){
	struct simulation * sim = (struct simulation *)(ctx);
	bool err;
	int prid = parse_positive_int(args[0], &err);
	if(err){
		eprintf("error: invalid pid");
	}

	bool removed = simulation_process_remove(sim, prid);
	if(!removed){
		eprintf("error: process with given prid does not exist");
	}

	eprintf("process %d killed", prid);
	return 0;
}

static int cpu_list(void * ctx, const char * args[SHELL_ARGS_MAX]){
	struct simulation * sim = (struct simulation *)(ctx);
	simulation_cpu_list(sim);
	return 0;
}

static int sim_tick(void * ctx, const char * args[SHELL_ARGS_MAX]){
	struct simulation * sim = (struct simulation *)(ctx);
	simulation_tick(sim);
	return 0;
}


int main(){
	eprintln("CreSchedo - OS Scheduler Simulator.");
	eprintln("Run `help` to get list of allowed commands");
	struct simulation sim = simulation_new();
	struct shell sh = shell_new((void*)&sim);
	shell_register_callback(&sh, "spawn", run_process);
	shell_register_callback(&sh, "kill", process_remove);
	shell_register_callback(&sh, "cpu", cpu_list);
	shell_register_callback(&sh, "ps", process_list);
	shell_register_callback(&sh, "tick", sim_tick);
	shell_start(&sh);
	simulation_free(&sim);
}
