#include "simulation.h"

typedef void (* log_callback_t)(const struct simulation * sim);

void log_simulation_process_flow_line(const struct simulation * sim);

void log_simulation_process_flow_header(const struct simulation * sim);

void log_simulation_cpu_flow_header(const struct simulation * sim);

void log_simulation_cpu_flow_line(const struct simulation * sim);

void log_simulation_sched_queues(const struct simulation * sim);

void log_nop(const struct simulation * sim);
