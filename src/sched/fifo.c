
#include <stdio.h>
#include "process.h"
#include "utils.h"
#include "fifo.h"

struct fifo fifo_new(size_t cap){

	if(cap == 0 || cap > 32000){
		PANIC("fifo must not have capacity %ld", cap);
	}

	prid_t * mem = malloc(cap * sizeof(prid_t));
	struct fifo fifo  = {
		.tail = mem,
		.head = mem,
		.mem = mem,
		.capacity = cap,
		.len = 0,
	};

	return fifo;
}

void fifo_push(struct fifo * self, prid_t val){

	if(self->len == self->capacity){
		PANIC("fifo is full, can't push");
	}

	*(self->tail) = val;
	self->len++;
	self->tail++;
	if(self->tail == self->mem + self->capacity){
		self->tail = self->mem;
	}
}

prid_t fifo_pop(struct fifo * self){
	if(self->len == 0){
		PANIC("fifo is empty, can't push");
	}

	prid_t val = (*self->head);
	self->len--;
	self->head++;
	if(self->head == self->mem + self->capacity){
		self->head = self->mem;
	}
	return val;
}

prid_t fifo_peek(struct fifo * self){
	if(self->len == 0){
		PANIC("can't peek from empty fifo");
	}
	return *(self->head);
}

bool fifo_full(const struct fifo * self ){
	return self->len == self->capacity;
}

bool fifo_empty(const struct fifo * self ){
	return self->len == 0;
}

void fifo_free(struct fifo *  self){
	free(self->mem);
}
