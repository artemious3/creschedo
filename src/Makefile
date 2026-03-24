BUILD ?= debug
NCPU ?= 4

CC=clang
CFLAGS = -std=c23 -Wall -pedantic -DSIMULATION_CPU_NUMBER=$(NCPU)
CFLAGS.debug = -O0 -g -fsanitize=address
CFLAGS.release = -O3 
CFLAGS += $(CFLAGS.$(BUILD))
TARGET = creschedo
SRC = main.c shell.c program.c simulation.c process.c log.c
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)


%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

-include $(DEP)

clean:
	rm -f $(TARGET) $(OBJ)


