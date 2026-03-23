#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "shell.h"
#include "program.h"
#include "simulation.h"
#include "utils.h"

long parse_count(const char * s, bool * err){
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

		long count = parse_count(input.words[0], &err);
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
	return 0;
}


int main(){
	eprintf("CreSchedo - OS Scheduler Simulator.\n");
	eprintf("Run `help` to get list of allowed commands\n");
	struct simulation sim = simulation_new();
	struct shell sh = shell_new((void*)&sim);
	shell_register_callback(&sh, "spawn", run_process);
	shell_register_callback(&sh, "ps", process_list);
	shell_start(&sh);
}
