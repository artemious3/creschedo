CC=clang
CFLAGS = -Wall -g -MMD -MP
TARGET = rbobin
OBJ = main.o shell.o program.o
DEPS = $(OBJS:.o=.d)


$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

main: $(OBJ)
	$(CC) $(OBJ) -o main

clean:
	rm -f $(TARGET) $(OBJ)


