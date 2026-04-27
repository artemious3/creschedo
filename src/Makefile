BUILD_TYPE ?= debug
NCPU ?= 4

B = build
CC=clang
SRC = main.c shell.c program.c simulation.c process.c log.c  \
			sched/scheduler.c sched/fifo.c sched/scheduler_fcfs.c
CFLAGS = -std=c23 -Wall -pedantic -DSIMULATION_CPU_NUMBER=$(NCPU)
CFLAGS.debug = -O0 -g -fsanitize=address
CFLAGS.release = -O3
TARGET = creschedo
INCLUDE= . ./sched/
INCLUDE := $(foreach d, $(INCLUDE), -I$(abspath $d))

CFLAGS += $(CFLAGS.$(BUILD_TYPE))
OBJ = $(patsubst %.c, $(B)/%.o, $(SRC))
DEP = $(patsubst %.c, $(B)/%.d, $(SRC))

$(B):
	mkdir -p $(B) $(B)/sched

$(B)/%.o: %.c | $(B)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ $(INCLUDE)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

-include $(DEP)

clean:
	rm -f $(TARGET)
	rm -r $(B)
