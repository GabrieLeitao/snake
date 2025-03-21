# Compiler
CC = gcc
# Compile flags
CFLAGS = -Wall -pedantic -std=c99 -O3

default: snake

# Terminal version: "make snake" ou apenas: "make"
snake: main.o snake.o
	$(CC) $(CFLAGS) -o snake main.o snake.o

# Graphic version: "make graphic"
graphic: graphic.o snake.o
	$(CC) $(CFLAGS) -o graphic graphic.o snake.o -lSDL2

# Object files
graphic.o: graphic.c
	$(CC) $(CFLAGS) -c graphic.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

snake.o: snake.c
	$(CC) $(CFLAGS) -c snake.c

clean:
	rm -f *.o snake graphic