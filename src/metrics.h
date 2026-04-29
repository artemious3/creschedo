struct metrics_process {
  unsigned int wait_ticks;
  unsigned int active_ticks;
  unsigned int ready_ticks;

  unsigned int preempted_times;
  unsigned int blocked_times;
};
