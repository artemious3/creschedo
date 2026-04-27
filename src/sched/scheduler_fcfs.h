#include "scheduler.h"
#include "fifo.h"

#define SCHEDULER_FCFS_FIFO_SIZE (2048)
#define SCHEDULER_FCFS_PRIORITIIES (16)

struct scheduler_fcfs{
	struct fifo fifo[SCHEDULER_FCFS_PRIORITIIES];
};

struct scheduler scheduler_fcfs_new();
