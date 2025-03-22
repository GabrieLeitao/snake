/* File: snake.c */

#include "snake.h"

/*-------------------------------------------------*/
POSITION set_position(int l, int c)
{
    POSITION aux;
    aux.l = l;
    aux.c = c;
    return aux;
}
/*-------------------------------------------------*/
POSITION readKeyboard(POSITION head)
{
    char linha[100];
    fflush(stdout);
    (void) fgets(linha, 100, stdin);
    switch(linha[0])
    {
        case 'w':
            head.l -= 1;
            break;
        case 's':
            head.l += 1;
            break;
        case 'a':
            head.c -= 1;
            break;
        case 'd':
            head.c += 1;
            break;
    }

    printf("l=%d c=%d\n", head.l, head.c);
    fflush(stdout);

    return head;
}

/*-------------------------------------------------*/
void initSnake(SNAKE *snake)
{
    int i;
    (*snake).pos = (POSITION *)calloc(MAXSNAKE, sizeof(POSITION));
    if (snake->pos == NULL)
    {
        fprintf(stderr, "Memory reallocation failed\n");
        exit(1);
    }
    snake->size_vector = MAXSNAKE;

    for (i = 1; i < MAXSNAKE; ++i)
    {
        snake->pos[i] = set_position(-1,-1);
    }
    snake->pos[0] = set_position(SNAKESTART_LINE,SNAKESTART_COL);
    snake->dim = 1;
}

/*-------------------------------------------------*/
void cleanSnake(SNAKE *snake)
{
    if (snake->size_vector > 0)
    {
        free(snake->pos);
        snake->pos = NULL;
        snake->size_vector = 0;
        snake->dim = 0;
    }
}

/*-------------------------------------------------*/
void showSnakePositions(SNAKE snake)
{
    int i;
    printf("Snake (Points: %d) || ", snake.dim);
    for(i=0; i < snake.size_vector; ++i)
        printf("(%d,%d)", snake.pos[i].l, snake.pos[i].c);
    printf("\n");
}

/*-------------------------------------------------*/
int myRand(int max_value)
{
    /*man 3 rand -->
    The  rand()  function returns a pseudo-random integer in the range 0 to
        RAND_MAX inclusive (i.e., the mathematical range [0, RAND_MAX]).  */
    double max = (double)RAND_MAX;
    return ((int) max_value * (rand()/max) );
}

/*-------------------------------------------------*/
void initWorld(WORLD x)
{
    int i, j, k, randVal, totalWeight = 0;

    // Define the probability distribution of characters
    CharProbability simb[] = {
        {' ', 120},  // 60% empty space
        {BONUS_CHAR, 12},  // 20% bonus
        {MINUS_CHAR, 2},  // 10% minus
        {LOOP_CHAR, 1},  // 10% loop
    };

    int numChars = sizeof(simb) / sizeof(simb[0]);

    // Calculate total weight
    for (k = 0; k < numChars; ++k)
        totalWeight += simb[k].weight;

    for (i = 0; i < MAXL; ++i) {
        for (j = 0; j < MAXC; ++j) {
            randVal = myRand(totalWeight);

            int cumulativeWeight = 0;
            for (k = 0; k < numChars; ++k) {
                cumulativeWeight += simb[k].weight;
                if (randVal < cumulativeWeight) {
                    x[i][j] = simb[k].character;
                    break;
                }
            }
        }
    }

    x[SNAKESTART_LINE][SNAKESTART_COL] = ' '; // Ensure the start position is empty
}

/*-------------------------------------------------*/
void showWorldSnake(WORLD x, SNAKE snake)
{
    int i, j;

    for(i = 0; i < MAXL; ++i)
        for (j = 0; j < MAXC; ++j)
            if (x[i][j] == SNAKE_CHAR)
                x[i][j] = ' ';

    for (i = 0; i < snake.dim; ++i)
        x[snake.pos[i].l][snake.pos[i].c] = SNAKE_CHAR;

    printf("     ");
    for (i = 0; i < MAXL; ++i)
        printf("[%2d ]", i);
    printf("\n");

    for(i = 0; i < MAXL; ++i)
    {
        printf("[%2d ]", i);
        for (j = 0; j < MAXC; ++j)
            printf("%3c  ", x[i][j]);
        printf("\n");
    }
}

/*-------------------------------------------------*/
int gameOn(WORLD x)
{
    int i, j, count = 0;

    for(i = 0; i < MAXL; ++i)
        for (j = 0; j < MAXC; ++j)
            if (x[i][j] == BONUS_CHAR || x[i][j] == MINUS_CHAR || x[i][j] == LOOP_CHAR)
                count++;

    return count > 0;
}

/*-------------------------------------------------*/
int WorldSnakeInteraction(WORLD world, SNAKE *snake, POSITION direction)
{
    int i;

    POSITION newp;
    newp.c = snake->pos[0].c + direction.c;
    newp.l = snake->pos[0].l + direction.l;

    if (world[newp.l][newp.c] == SNAKE_CHAR) {
        printf("You shouldn't eat yourself!\n");
        return 1;
    }
    if (newp.l < 0 || newp.l >= MAXL || newp.c < 0 || newp.c >= MAXC) {
        printf("Watch your head, buddy!\n");
        return 1;
    }

    // Clear the tail
    world[snake->pos[snake->dim - 1].l][snake->pos[snake->dim - 1].c] = ' ';

    // Handle bonuses or minus (growth or shrinkage)
    if (world[newp.l][newp.c] == BONUS_CHAR) {
        world[newp.l][newp.c] = ' ';  // Clear the bonus
        // Increase snake size
        if (snake->dim == snake->size_vector) {
            POSITION *ap = (POSITION *)realloc(snake->pos, (sizeof(POSITION)) * (snake->size_vector + INCSNAKE));
            if (ap != NULL) {
                snake->pos = ap; 
                snake->size_vector += INCSNAKE;
            } else {
                fprintf(stderr, "Memory reallocation failed\n");
                exit(1);
            }
        }
        snake->dim += 1;
    }
    else if (world[newp.l][newp.c] == MINUS_CHAR) {
        world[newp.l][newp.c] = ' ';  // Clear the minus
        // Decrease snake size
        if (snake->dim > 1) {
            snake->dim -= 1;
            world[snake->pos[snake->dim - 1].l][snake->pos[snake->dim - 1].c] = ' ';
        } else {
            printf("You shrinked!\n");
            return 1;
        }
    } else if (world[newp.l][newp.c] == LOOP_CHAR) {
        world[newp.l][newp.c] = ' ';  // Clear the loop
        POSITION loop_pos = get_new_loop_position(world);
        if (snake->dim > 1) {
            newp = loop_pos;  // Set the new position if using the loop
        } else {
            printf("You need to have at least 2 points to use the loop\n");
        }
    }

    // Move the body
    for (i = snake->dim - 1; i > 0; i--)
    {
        snake->pos[i] = snake->pos[i - 1];
    }
    snake->pos[0] = newp;
    world[snake->pos[0].l][snake->pos[0].c] = SNAKE_CHAR;  // Update the world with the new head

    return 0;
}

/*-------------------------------------------------*/

POSITION get_new_loop_position(WORLD x)
{
    int i, j;
    POSITION loop_pos;
    for(i = 0; i < MAXL; ++i)
        for (j = 0; j < MAXC; ++j)
            if (x[i][j] == LOOP_CHAR)
                loop_pos = set_position(i, j);
    return loop_pos;
}
/* End of snake_dinamica.c */