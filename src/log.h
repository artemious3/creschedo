#include "simulation.h"

typedef void (* log_callback_t)(struct simulation * sim);

void log_simulation_process_flow_line(struct simulation * sim);

void log_simulation_process_flow_header(struct simulation * sim);

void log_simulation_cpu_flow_header(struct simulation * sim);

void log_simulation_cpu_flow_line(struct simulation * sim);

void log_nop(struct simulation * sim);
