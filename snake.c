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
    fgets(linha, 100, stdin);
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
        printf("Erro -> calloc - falta de memória ->>>> exit(0)\n");
        exit(0);
    }

    snake->size_vector =  MAXSNAKE;

    for (i = 1; i < MAXSNAKE; ++i)
        snake->pos[i] = set_position(-1,-1);
    
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
    int i, j, value, length;

    char simb[] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',BONUS_CHAR,BONUS_CHAR,BONUS_CHAR,MINUS_CHAR,'!','?'};
    length = strlen(simb);

    for(i = 0; i < MAXL; ++i)
        for (j = 0; j < MAXC; ++j)
        {
            value = myRand(length - 2);
            if (value  < 0)
                value = 0;
            if (value > length - 1)
                value = length - 1;

            x[i][j] = simb[value];
        }

    x[SNAKESTART_LINE][SNAKESTART_COL] = simb[0];
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
            if (x[i][j] != BONUS_CHAR && x[i][j] != MINUS_CHAR)
                count = count + 1;

    return (MAXL*MAXC - count);
}
/*-------------------------------------------------*/
int WorldSnakeInteraction(WORLD world, SNAKE *snake, POSITION newp)
{
    int i;

    /* Test position*/
    /* Snake eats goodies at the position if any */
    /* and increases its size just be one */

    if (world[newp.l][newp.c] == SNAKE_CHAR)
    {
        printf("You shouldn't eat yourself!\n");
        return 1;
    }
    else if (newp.l < 0 || newp.l >= MAXL || newp.c < 0 || newp.c >= MAXC)
    {
        printf("Watch your head, buddy!\n");
        return 1;
    }
    

    if (world[newp.l][newp.c] == BONUS_CHAR)
    {
        world[newp.l][newp.c] = ' ';

        /* Increase the vector size */
        if (snake->dim == snake->size_vector)
        {
            POSITION *ap;
            ap = (POSITION *) realloc(snake->pos, (sizeof(POSITION))*(snake->size_vector + INCSNAKE));
            if (ap != NULL)
            {
                snake->pos = ap;  /* update the pointer with the new address */
                snake->size_vector = snake->size_vector + INCSNAKE;
            }
        }
        if (snake->dim < snake->size_vector )
            snake->dim += 1;
    }
    else if (world[newp.l][newp.c] == MINUS_CHAR)
    {
        world[newp.l][newp.c] = ' ';

        /* Decrease the vector size */
        if (snake->size_vector - snake-> dim >= 5)
        {
            POSITION *ap;
            ap = (POSITION *) realloc(snake->pos, (sizeof(POSITION))*(snake->size_vector - INCSNAKE));
            if (ap != NULL)
            {
                snake->pos = ap;  /* update the pointer with the new address */
                snake->size_vector = snake->size_vector - INCSNAKE;
            }
        }
        if (snake->dim > 1)
            snake->dim -= 1;
    }
    /* Update positions */
    for (i = snake->dim - 1; i > 0; i--)
        snake->pos[i] =  snake->pos[i - 1];
    snake->pos[0] = newp;

    return 0;
}
/* End of snake_dinamica.c */