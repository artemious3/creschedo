#include <stdlib.h>
#include "process.h"

struct fifo {
	prid_t * tail;
	prid_t * head;
	size_t capacity;
	size_t len;
};

struct fifo fifo_new(size_t cap);

void fifo_push(struct fifo * self, prid_t val);

prid_t fifo_pop(struct fifo * self);

prid_t fifo_peek(struct fifo * self);

bool fifo_full(const struct fifo * self );

bool fifo_empty(const struct fifo * self );

void fifo_free(struct fifo *  self);
