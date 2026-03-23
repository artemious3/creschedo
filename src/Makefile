CC=gcc
CFLAGS = -std=c23 -Wall -g -pedantic
TARGET = rbobin
SRC = main.c shell.c program.c
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)
TARGET = rbobin

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

-include $(DEP)

clean:
	rm -f $(TARGET) $(OBJ)


