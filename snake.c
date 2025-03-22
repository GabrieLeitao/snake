/* File: snake.c */

#include "snake.h"

CharProbability simb[] = {
    {EMPTY_CHAR, 120},  // 60% empty space
    {BONUS_CHAR, 12},  // 20% bonus
    {MINUS_CHAR, 2},  // 10% minus
    {LOOP_CHAR, 1},  // 10% loop
};

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
    int i, j;

    #if TEST_MODE
    {
        int k, totalWeight = 0, randVal;
        int numChars = sizeof(simb) / sizeof(simb[0]);

        //Calculate total weight
        for (k = 0; k < numChars; ++k)
        totalWeight += simb[k].weight;
        
        for (i = 0; i < MAXH; ++i) {
            for (j = 0; j < MAXW; ++j) {
                randVal = myRand(totalWeight);
                
                int cumulativeWeight = 0;
                for (k = 0; k < numChars; ++k) {
                    cumulativeWeight += simb[k].weight;
                    if (randVal < cumulativeWeight) {
                        x[i][j].ch = simb[k].character;
                        x[i][j].creation_time = SDL_GetTicks();
                        break;
                    }
                }
            }
        }
    }
    #else
    {
        for (i = 0; i < MAXH; ++i) {
            for (j = 0; j < MAXW; ++j) {
                x[i][j].ch = EMPTY_CHAR;
                x[i][j].creation_time = SDL_GetTicks();
            }
        }
    
        // Add a few objects at the beginning
        for (i = 0; i < 2; ++i) {
            int randRow = myRand(MAXH);
            int randCol = myRand(MAXW);
    
            x[randRow][randCol].ch = BONUS_CHAR;
            x[randRow][randCol].creation_time = SDL_GetTicks();
        }
    }
    #endif

    x[SNAKESTART_LINE][SNAKESTART_COL].ch = EMPTY_CHAR; // Ensure the start position is empty
}

/*-------------------------------------------------*/
void showWorldSnake(WORLD x, SNAKE snake)
{
    int i, j;

    for(i = 0; i < MAXH; ++i)
        for (j = 0; j < MAXW; ++j)
            if (x[i][j].ch == SNAKE_CHAR)
                x[i][j].ch = EMPTY_CHAR;

    for (i = 0; i < snake.dim; ++i)
        x[snake.pos[i].l][snake.pos[i].c].ch = SNAKE_CHAR;

    printf("     ");
    for (i = 0; i < MAXH; ++i)
        printf("[%2d ]", i);
    printf("\n");

    for(i = 0; i < MAXH; ++i)
    {
        printf("[%2d ]", i);
        for (j = 0; j < MAXW; ++j)
            printf("%3c  ", x[i][j].ch);
        printf("\n");
    }
}

/*-------------------------------------------------*/
int gameOn(WORLD x)
{
    int i, j, count = 0;

    for(i = 0; i < MAXH; ++i)
        for (j = 0; j < MAXW; ++j)
            if (x[i][j].ch == BONUS_CHAR || x[i][j].ch == MINUS_CHAR || x[i][j].ch == LOOP_CHAR)
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

    if (world[newp.l][newp.c].ch == SNAKE_CHAR) {
        printf("You shouldn't eat yourself!\n");
        return 1;
    }
    if (newp.l < 0 || newp.l >= MAXH || newp.c < 0 || newp.c >= MAXW) {
        printf("Watch your head, buddy!\n");
        return 1;
    }

    // Clear the tail
    world[snake->pos[snake->dim - 1].l][snake->pos[snake->dim - 1].c].ch = EMPTY_CHAR;

    // Handle bonuses or minus (growth or shrinkage)
    if (world[newp.l][newp.c].ch == BONUS_CHAR)
    {
        world[newp.l][newp.c].ch = EMPTY_CHAR;  // Clear the bonus
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
        if (snake->activeGlows < MAX_GLOWS)
        {
            int glowIndex = snake->activeGlows++;
            snake->glowing[glowIndex] = 1;
            snake->glowCounter[glowIndex] = 0;
            snake->glowFrameCounter[glowIndex] = 0;
            snake->glowStartTime[glowIndex] = SDL_GetTicks();
        }
    }
    else if (world[newp.l][newp.c].ch == MINUS_CHAR)
    {
        world[newp.l][newp.c].ch = EMPTY_CHAR;  // Clear the minus
        // Decrease snake size
        if (snake->dim > 1) {
            snake->dim -= 1;
            world[snake->pos[snake->dim - 1].l][snake->pos[snake->dim - 1].c].ch = EMPTY_CHAR;
        } else {
            printf("You shrinked!\n");
            return 1;
        }
    } 
    else if (world[newp.l][newp.c].ch == LOOP_CHAR)
    {
        world[newp.l][newp.c].ch = EMPTY_CHAR;  // Clear the loop
        POSITION loop_pos = get_new_loop_position(world);
        if (loop_pos.c == -1) {
            printf("No loop found\n");
            return 1;
        }
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
    world[snake->pos[0].l][snake->pos[0].c].ch = SNAKE_CHAR;  // Update the world with the new head
    world[snake->pos[0].l][snake->pos[0].c].creation_time = SDL_GetTicks();  // Update the world with the new head

    return 0;
}


void addNewObject(WORLD x)
{
    int k, totalWeight = 0;
    int numChars = sizeof(simb) / sizeof(simb[0]);

    // Calculate total weight
    for (k = 0; k < numChars; ++k)
        totalWeight += simb[k].weight;

    int randRow = myRand(MAXH);
    int randCol = myRand(MAXW);
    int randVal = myRand(totalWeight);

    // Ensure the new object is placed in an empty cell
    while (x[randRow][randCol].ch != EMPTY_CHAR) {
        randRow = myRand(MAXH);
        randCol = myRand(MAXW);
    }

    // Add a new object based on the probability distribution
    int cumulativeWeight = 0;
    for (k = 0; k < numChars; ++k) {
        cumulativeWeight += simb[k].weight;
        if (randVal < cumulativeWeight) {
            if (simb[k].character == LOOP_CHAR) {
                if (randRow < 1 || randRow > MAXH || randCol < 1 || randCol > MAXW) {
                    continue;  // Skip the loop if it's too close to the border
                }
            }
            x[randRow][randCol].ch = simb[k].character;
            x[randRow][randCol].creation_time = SDL_GetTicks();
            break;
        }
    }
}

/*-------------------------------------------------*/

POSITION get_new_loop_position(WORLD x)
{
    int i, j;
    POSITION loop_pos = set_position(-1, -1);
    for(i = 0; i < MAXH; ++i)
        for (j = 0; j < MAXW; ++j)
            if (x[i][j].ch == LOOP_CHAR)
                loop_pos = set_position(i, j);
    return loop_pos;
}
/* End of snake_dinamica.c */