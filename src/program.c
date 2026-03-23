#include "program.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

struct program program_new(){
	struct program self = {
		.capacity = PROGRAM_INITIAL_CAPACITY,
		.length = 0,
		.ops = malloc(sizeof(program_op)*PROGRAM_INITIAL_CAPACITY),
	};
	return self;
}


void program_append_ops(struct program * self, unsigned int count, program_op op){
	while(self->length + count > self->capacity){
		
		if(self->capacity >= PROGRAM_MAX_CAPACITY){
			PANIC("program max capacity exceeded");
		}

		program_op * ops_realloc = realloc(self->ops, sizeof(program_op)*(self->capacity * 2));

		if(ops_realloc == NULL){
			PANIC("realloc fail");
		}

		self->capacity *= 2;
		self->ops = ops_realloc;
	}

	for(int i = 0; i < count; ++i){
		self->ops[self->length + i] = op;
	}
	self->length += count;
}

void program_free(struct program * self){
	free(self->ops);
}

void program_list(struct program * self){
	for(int i = 0; i < self->length; ++i){
		char op_representation;
		switch(self->ops[i]){
			case OP_RUN:
				op_representation = 'R';
				break;
			case  OP_WAIT:
				op_representation = 'W';
				break;
		}
		printf("%c\n", op_representation);
	}
}


