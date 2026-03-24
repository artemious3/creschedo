#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "shell.h"
#include "utils.h"


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


struct shell_input shell_split_into_words(char *raw_cmd) {


  struct shell_input self = {.words = {NULL}, .too_many_words = false};

  char *c = raw_cmd;
  int word_counter = 0;
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

		if(word_counter == SHELL_WORDS_MAX+1){
			self.too_many_words = true;
			break;
		}

		self.words[word_counter] = current_cmd_or_arg;

    word_counter++;


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

void shell_start(struct shell * self, FILE * stream) {

	if(stream == NULL){
		stream = stdin;
	}

	char raw_input[256];
  while (1) {
		if(stream == stdin)
			eprintf("> ");

		// if encountered EOF or an error occured,
		// stop the shell
		if ( fgets(raw_input, 256, stream) == NULL) {
			break;
		}

		struct shell_input input = shell_split_into_words(raw_input);

		// skip empty input
		if(input.words[0] == NULL){
			continue;
		}

		if(input.words[0][0] == '@'){
			FILE * subinput = fopen(input.words[0]+1, "r");
			if(subinput == NULL){
				eprintln("error: failed to open file %s", input.words[0]+1);
			} else {
				shell_start(self, subinput);
				fclose(subinput);
			}
			continue;
		}


		if(input.too_many_words){
			eprintln("too many arguments, maximum is %d", SHELL_ARGS_MAX);
			continue;
		}

		cmd_callback_t callback = shell_get_callback(self, input.words[0]);
		if(callback == NULL){
			eprintf("unkown command: %s\n", input.words[0]);
		} else {
			callback(self->context, stream, &input.words[1]);
			eprintln("");
		}

	}
}
