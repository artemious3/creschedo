
#define SHELL_ARG_MAX (3)
#define SHELL_REGISTERED_CMD_MAX (16)


typedef int (*cmd_callback_t)(const char* args[SHELL_ARG_MAX]);

struct shell {
	unsigned long cmd_hashes[SHELL_REGISTERED_CMD_MAX];
	cmd_callback_t cmd_callbacks[SHELL_REGISTERED_CMD_MAX];
};

struct shell shell_new();

void shell_register_callback(struct shell* self, const char * cmd, cmd_callback_t cb);

void shell_start(struct shell * self);
