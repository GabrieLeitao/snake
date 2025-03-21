#include "snake.h"

int main (void)
{
    WORLD world;
    SNAKE snake;
    POSITION position;

    initWorld(world);
    initSnake(&snake);

    position = snake.pos[0];

    do {
        showWorldSnake(world, snake);
        showSnakePositions(snake);

        if (!gameOn(world))
        {
            printf("\n !! You won, Congrats!! \n");
            break;
        }
        printf("w-Up; s-Down; a-Left; d-Right: ");
        position = readKeyboard(position);

    } while (WorldSnakeInteraction(world, &snake, position) != 1);

    cleanSnake(&snake);

    printf("\n !! Snake-End !!\n");

    return 0;
}