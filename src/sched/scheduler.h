#include "process.h"
#include "utils.h"

// Convention: implementors of scheduler_<algorithm>_new functions allocate 
// underlying data with `malloc`.  It is freed by scheduler_free using `free`
struct scheduler {
	void * _data;
	void (*_report_state)(void* data, prid_t, process_state);
	prid_t (*_select)(void* data);
	void (*_free)(void * data);
};

void scheduler_report_state(struct scheduler* self, prid_t prid, process_state state);

prid_t scheduler_select(struct scheduler* self);

void scheduler_free(struct scheduler* self);

