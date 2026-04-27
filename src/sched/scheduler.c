#include "scheduler.h"

void scheduler_report_ready(struct scheduler* self, struct scheduler_process_descriptor pd){
	self->_report_ready(self->_data, pd);
}

// note that select assumes that given process was made active
prid_t scheduler_select(struct scheduler* self){
	return self->_select(self->_data);
}

void scheduler_free(struct scheduler * self){
	self->_free(self->_data);
	free(self->_data);
}
