#define PANIC(msg, ...) \
	fprintf(stderr, "PANIC:" #msg "\n" __VA_OPT__(,) __VA_ARGS__); \
	abort();

#define eprintln(msg, ...) \
	fprintf(stderr, #msg "\n" __VA_OPT__(,) __VA_ARGS__); \

#define eprintf(msg, ...) \
	fprintf(stderr, #msg __VA_OPT__(,) __VA_ARGS__); \
