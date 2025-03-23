#ifndef SNAKE_H_
#define SNAKE_H_

/* File: snake.h */

#include <stdio.h>
#include <stdlib.h>  /* rand() */
#include <time.h>  /* rand() */
#include <string.h>
#include <SDL2/SDL.h>

// World size
#define MAXH 40
#define MAXW 60

#define TEST_MODE 1

// Define constants for window dimensions and cell size, for graphic mode with lSDL2
#define CELL_SIZE 20
#define DELAY_TIME 250
#define RENDER_FPS 60
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define FULLSCREEN 0

// Snake
#define MAXSNAKE 5
#define INCSNAKE 5
#define SNAKESTART_LINE MAXH / 2
#define SNAKESTART_COL MAXW / 2
#define SNAKESTART_DIR_LINE 0
#define SNAKESTART_DIR_COL 1
#define EMPTY_CHAR ' '
#define SNAKE_CHAR '*'
#define BONUS_CHAR '$'
#define MINUS_CHAR '-'
#define PORTAL_CHAR '&'
#define SNAKE_GLOW_SPEED 100.0

#define SNAKE_SIZE (CELL_SIZE / 1)
#define BONUS_SIZE (CELL_SIZE / 1.4)
#define MINUS_SIZE (CELL_SIZE / 1.6)
#define PORTAL_SIZE (CELL_SIZE / 1.1)

#define MAX_GLOWS 10 // Maximum number of simultaneous glowing effects

typedef struct position
{
    int l, c;  /* line, column */
} POSITION;

typedef struct snake
{
    int dim;
    int size_vector;
    POSITION *pos;   /* De suporte ao vector dinâmico */
    int glowing[MAX_GLOWS];
    int glowCounter[MAX_GLOWS];
    int glowFrameCounter[MAX_GLOWS];
    Uint32 glowStartTime[MAX_GLOWS];
    int activeGlows;
} SNAKE;

typedef struct
{
    char character;
    int weight; // Higher values mean higher probability
} CharProbability;

typedef struct cell
{
    char ch;
    Uint32 creation_time;
} CELL;

typedef CELL WORLD[MAXH][MAXW];

// Define the probability distribution of characters
extern CharProbability simb[];

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

POSITION get_new_portal_position(WORLD x);
void addNewObject(WORLD x);

#endif /* SNAKE_H_ */