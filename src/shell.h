#pragma once
#include <stdbool.h>

#define SHELL_WORDS_MAX (4)
#define SHELL_ARGS_MAX (SHELL_WORDS_MAX-1)
#define SHELL_REGISTERED_CMD_MAX (16)


typedef int (*cmd_callback_t)(void* ctx, const char* args[SHELL_ARGS_MAX]);


struct shell_input {
  const char *words[SHELL_WORDS_MAX];
	bool too_many_words;
};

struct shell {
	unsigned long cmd_hashes[SHELL_REGISTERED_CMD_MAX];
	cmd_callback_t cmd_callbacks[SHELL_REGISTERED_CMD_MAX];
	void * context;
};

struct shell shell_new(void* context);

void shell_register_callback(struct shell* self, const char * cmd, cmd_callback_t cb);

void shell_start(struct shell * self);

struct shell_input shell_split_into_words(char *raw_cmd);
