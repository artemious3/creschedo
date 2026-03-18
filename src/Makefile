CC=clang
CFLAGS = -Wall -g
TARGET = rbobin
OBJ = main.o shell.o


$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


main: shell.o src/main.c
	$(CC) shell.o src/main.c -o main

clean:
	rm -f $(TARGET) $(OBJ)


