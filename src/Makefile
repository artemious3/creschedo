BUILD ?= debug
NCPU ?= 4

CC=clang
SRC = main.c shell.c program.c simulation.c process.c log.c  \
			sched/scheduler.c sched/fifo.c sched/scheduler_fcfs.c
CFLAGS = -std=c23 -Wall -pedantic -DSIMULATION_CPU_NUMBER=$(NCPU)
CFLAGS.debug = -O0 -g -fsanitize=address
CFLAGS.release = -O3 
TARGET = creschedo
INCLUDE= . ./sched/
INCLUDE := $(foreach d, $(INCLUDE), -I$(abspath $d))

CFLAGS += $(CFLAGS.$(BUILD))
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)


%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ $(INCLUDE)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

-include $(DEP)

clean:
	rm -f $(TARGET) $(OBJ)


