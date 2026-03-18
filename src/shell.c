#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "shell.h"

struct shell_input {
  const char *cmd;
  const char *arg[SHELL_ARG_MAX];
	bool too_many_args;
};

static unsigned long djb2_hash(const char *str) {
    unsigned long hash = 5381; 
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + (unsigned char)c; 
    }

    return hash;
}

struct shell shell_new(void* context){
	struct shell self = {
		.cmd_callbacks = {NULL},
		.cmd_hashes = {0},
		.context = context
	};
	return self;
}

void shell_register_callback(struct shell* self, const char * cmd, cmd_callback_t cb){
	unsigned long hash = djb2_hash(cmd);
	
	for(int i = 0; i < SHELL_REGISTERED_CMD_MAX; ++i){
		assert(hash != self->cmd_hashes[i]);
	}

	size_t index = 0;
	// find first nonzero hash bucket
	while (index < SHELL_REGISTERED_CMD_MAX && self->cmd_hashes[index] != 0L) {
		index++;
	}
	assert(index != SHELL_REGISTERED_CMD_MAX);

	self->cmd_hashes[index] = hash;
	self->cmd_callbacks[index] = cb;
}

static cmd_callback_t shell_get_callback(struct shell* self, const char * cmd){
	unsigned long hash = djb2_hash(cmd);

	size_t index = 0;
	while (index < SHELL_REGISTERED_CMD_MAX && self->cmd_hashes[index] != hash) {
		index++;
	}

	if(index == SHELL_REGISTERED_CMD_MAX){
		return NULL;
	}
	return self->cmd_callbacks[index];
}


static struct shell_input shell_process_raw_input(char *raw_cmd) {

  struct shell_input self = {.cmd = NULL, .arg = {NULL}, .too_many_args = false};

  char *c = raw_cmd;
  int arg_counter = 0;
  const char *current_cmd_or_arg;

  while (isspace(*c))
    ++c;
  if (*c == 0) {
    return self;
  }

  while (1) {
    current_cmd_or_arg = c;

    while (*c && !isspace(*c))
      c++;

		if (*c == 0) {
			break;
		}

		if(arg_counter == SHELL_ARG_MAX+1){
			self.too_many_args = true;
			break;
		}

    if (arg_counter == 0) {
      self.cmd = current_cmd_or_arg;
    } else {
      self.arg[arg_counter - 1] = current_cmd_or_arg;
    }

    arg_counter++;


    *c = 0;
    c++;

    while (isspace(*c))
      ++c;
    if (*c == 0) {
      break;
    }
  }

  return self;
}

void shell_start(struct shell * self) {
	char raw_input[256];
  while (1) {
    printf("> ");

		// if encountered EOF or an error occured,
		// stop the shell
		if ( fgets(raw_input, 256, stdin) == NULL) {
			break;
		}

		struct shell_input input = shell_process_raw_input(raw_input);

		// skip empty input
		if(input.cmd == NULL){
			continue;
		}

		if(input.too_many_args){
			printf("too many arguments, maximum is %d\n", SHELL_ARG_MAX);
			continue;
		}

		cmd_callback_t callback = shell_get_callback(self, input.cmd);
		if(callback == NULL){
			fprintf(stderr, "unkown command: %s\n", input.cmd);
		} else {
			callback(self->context, input.arg);
			printf("\n");
		}

	}
}
