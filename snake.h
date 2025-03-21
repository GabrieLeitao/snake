#ifndef SNAKE_H_
#define SNAKE_H_

/* File: snake.h */

#include <stdio.h>
#include <stdlib.h>  /* rand() */
#include <string.h>
#include <SDL2/SDL.h>

typedef struct position
{
    int l, c;  /* line, column */
} POSITION;

// Snake
#define MAXSNAKE 5
#define INCSNAKE 5
#define SNAKESTART_LINE 0
#define SNAKESTART_COL 0
#define SNAKE_CHAR '*'
#define BONUS_CHAR '$'
#define MINUS_CHAR '-'
#define LOOP_CHAR '&'

// World size
#define MAXL 40
#define MAXC 40

// Define constants for window dimensions and cell size, for graphic mode with lSDL2
#define CELL_SIZE 20
#define DELAY_TIME 400
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 900

typedef struct snake
{
    int dim;
    int size_vector;
    POSITION *pos;   /* De suporte ao vector dinâmico */
} SNAKE;

typedef struct {
    char character;
    int weight; // Higher values mean higher probability
} CharProbability;

typedef char WORLD[MAXL][MAXC];

/* Function prototypes */
POSITION set_position(int l, int c);
POSITION readKeyboard(POSITION x);

void initSnake(SNAKE *snake);
void cleanSnake(SNAKE *snake);
void showSnakePositions(SNAKE snake);

/* Gera um número pseudo aleatório entre 0 e max_value */
int myRand(int max_value);

void initWorld(WORLD x);
void showWorldSnake(WORLD x, SNAKE snake);

int gameOn(WORLD x);

int WorldSnakeInteraction(WORLD world, SNAKE *snake, POSITION direction);

POSITION get_new_loop_position(WORLD x);

#endif /* SNAKE_H_ */