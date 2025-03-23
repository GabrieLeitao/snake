# Compiler
CC = gcc
# Compile flags
CFLAGS = -Wall -pedantic -std=c99 -g
LDFLAGS = -lSDL2 -lm  # Libraries

# Manually specify source files
SRC_TERMINAL = main.c snake.c
SRC_GRAPHIC = graphic.c snake.c utils.c

# Generate object file list
OBJ_TERMINAL = $(SRC_TERMINAL:.c=.o)
OBJ_GRAPHIC = $(SRC_GRAPHIC:.c=.o)

# Default target (builds the terminal version)
default: graphic

# Terminal version
snake: $(OBJ_TERMINAL)
	$(CC) $(CFLAGS) -o $@ $^

# Graphic version
graphic: $(OBJ_GRAPHIC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each .c file into a .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f *.o snake graphic