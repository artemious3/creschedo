#pragma once
#include "process.h"
#include "utils.h"

struct scheduler_process_descriptor {
    prid_t prid;
    unsigned int priority;
    unsigned int t_since;
};

// Convention: implementors of scheduler_<algorithm>_new functions allocate
// underlying data with `malloc`.  It is freed by scheduler_free using `free`
struct scheduler {
	void * _data;
	void (*_report_ready)(void* data, struct scheduler_process_descriptor);
	prid_t (*_select)(void* data);
	void (*_free)(void * data);
};

void scheduler_report_ready(struct scheduler* self, struct scheduler_process_descriptor);

prid_t scheduler_select(struct scheduler* self);

void scheduler_free(struct scheduler* self);
