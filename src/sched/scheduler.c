#include "scheduler.h"

void scheduler_report_state(struct scheduler* self, prid_t prid, process_state state){
	self->_report_state(self->_data, prid, state);
}

// note that select assumes that given process was made active
prid_t scheduler_select(struct scheduler* self){
	return self->_select(self->_data);
}

void scheduler_free(struct scheduler * self){
	self->_free(self->_data);
	free(self->_data);
}

