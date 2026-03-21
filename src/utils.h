#define PANIC(msg, ...) \
	fprintf(stderr, "PANIC:" #msg "\n" __VA_OPT__(,) __VA_ARGS__); \
	abort();

