#include <stdlib.h>
#include "process.h"
#include "scheduler.h"


struct fifo {
	struct scheduler_process_descriptor * tail;
	struct scheduler_process_descriptor * head;
	struct scheduler_process_descriptor * mem;
	size_t capacity;
	size_t len;
};

struct fifo fifo_new(size_t cap);

void fifo_push(struct fifo * self, struct scheduler_process_descriptor);

struct scheduler_process_descriptor fifo_pop(struct fifo * self);

const struct scheduler_process_descriptor * fifo_peek(struct fifo * self);

bool fifo_full(const struct fifo * self );

bool fifo_empty(const struct fifo * self );

void fifo_free(struct fifo *  self);
