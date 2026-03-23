#include <stdlib.h>

typedef enum {
	OP_RUN,
	OP_WAIT
} program_op;


struct program {
	program_op * ops;
	size_t length;
	size_t capacity;
};

#define PROGRAM_INITIAL_CAPACITY (8)
#define PROGRAM_MAX_CAPACITY (16384)

struct program program_new();

void program_append_ops(struct program * self, unsigned int count, program_op op);

void program_free(struct program * self);

void program_list(struct program * self);
