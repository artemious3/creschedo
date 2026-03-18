#include "shell.h"
#include <stdio.h>

int dbg(const char* args[SHELL_ARG_MAX]){
	printf("This is a shell debug command\n");

	for(int i = 0; i < SHELL_ARG_MAX; ++i){
		if(args[i] == NULL){
			printf("arg %d is NULL\n", i);
		} else {
			printf("arg %d : %s\n", i, args[i]);
		}
	}

	return 0;
}


int main(){
	printf("RobinBobin Scheduler Simulator.\n");
	printf("Run `help` to get list of allowed commands\n");
	struct shell sh = shell_new();
	shell_register_callback(&sh, "dbg", dbg);
	shell_start(&sh);
}
