#include "snake.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

// Rendering functions using SDL
void initializeSDL();
void drawGame(WORLD world, SNAKE *snake);
POSITION get_direction(POSITION direction);
void cleanupSDL();

int main() {
    // Initialize SDL and game objects
    initializeSDL();
    WORLD world;
    SNAKE snake;
    POSITION position;
    POSITION direction;
    int quit = 0;
    float delay = DELAY_TIME;

    initWorld(world);
    initSnake(&snake);

    direction = set_position(0, 1); // line, column
    position = snake.pos[0];

    Uint32 startTime, endTime, deltaTime;
    // Main game loop
    do {
        startTime = SDL_GetTicks();

        // Atualizar direcao
        direction = get_direction(direction);
        // Mover snake
        position.c += direction.c;
        position.l += direction.l;
        // Draw the game
        quit = WorldSnakeInteraction(world, &snake, direction);
        drawGame(world, &snake);

        if (!gameOn(world))
        {
            printf("\n !! You won, Congrats!! \n");
            break;
        }

        // Calculate time taken for frame
        endTime = SDL_GetTicks(); // Get end time of frame
        deltaTime = endTime - startTime; // Calculate time taken for frame

        // Delay to control frame rate
        if (deltaTime < delay) {
            SDL_Delay(delay - deltaTime);
        }
        if (delay < DELAY_TIME / 6)
            delay -= delay / DELAY_TIME / 2;
        else
            delay -= DELAY_TIME / delay;
        if (delay <= 0)
            delay = 2;
            
    } while (quit != 1);

    // Clean up resources
    cleanupSDL();
    cleanSnake(&snake);

    printf("\n !! Snake-End !!\n");

    return 0;
}

// Function to initialize SDL
void initializeSDL() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // Create a window and renderer
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "Unable to create window: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Unable to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
}

// Function to draw the game objects (snake, world, etc.)
void drawGame(WORLD world, SNAKE *snake) {
    int i, j;
    
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB, alpha (transparency)
    SDL_RenderClear(renderer);

    for(i = 0; i < MAXL; ++i)
        for (j = 0; j < MAXC; ++j)
            if (world[i][j] == SNAKE_CHAR)
                world[i][j] = ' ';

    for (i = 0; i < snake->dim; ++i)
        world[snake->pos[i].l][snake->pos[i].c] = SNAKE_CHAR;

    // Draw the snake
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for the snake
    for (i = 0; i < snake->dim; i++) {
        SDL_Rect cellRect = {(snake->pos[i].c + 1) * CELL_SIZE, (snake->pos[i].l + 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE};
        SDL_RenderFillRect(renderer, &cellRect);
    }
    // Draw objects
    for(i = 0; i < MAXL; ++i)
        for (j = 0; j < MAXC; ++j)
        {
            if (world[i][j] == BONUS_CHAR)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue color for the Increase - BONUS_CHAR
                SDL_Rect cellRect = {(j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(renderer, &cellRect);
            }
            else if (world[i][j] == MINUS_CHAR)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for the decrease - MINUS_CHAR
                SDL_Rect cellRect = {(j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(renderer, &cellRect);
            }
            else if (world[i][j] == LOOP_CHAR)
            {
                SDL_SetRenderDrawColor(renderer, 120, 80, 30, 255); // Color for the LOOPHOLE
                SDL_Rect cellRect = {(j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(renderer, &cellRect);
            }
        }

    // Draw borders
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 175);
    for(i = 0; i <= MAXL + 1; ++i)
    {
        SDL_Rect cellRect = {(MAXC + 1) * CELL_SIZE, i * CELL_SIZE, CELL_SIZE / 2, CELL_SIZE};
        SDL_RenderFillRect(renderer, &cellRect);
        SDL_Rect cellRect2 = {CELL_SIZE / 2, i * CELL_SIZE, CELL_SIZE / 2, CELL_SIZE};
        SDL_RenderFillRect(renderer, &cellRect2);
    }
    for(j = 0; j <= MAXC + 1; ++j)
    {
        SDL_Rect cellRect = {j * CELL_SIZE, (MAXL + 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE / 2};
        SDL_RenderFillRect(renderer, &cellRect);
        SDL_Rect cellRect2 = {j * CELL_SIZE, CELL_SIZE / 2, CELL_SIZE, CELL_SIZE / 2};
        SDL_RenderFillRect(renderer, &cellRect2);
    }
        
    // Update the screen
    SDL_RenderPresent(renderer);
}

// Function to handle user input
POSITION get_direction(POSITION direction) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        return set_position(-1, 0);
                    case SDLK_DOWN:
                        return set_position(1, 0);
                    case SDLK_LEFT:
                        return set_position(0, -1);
                    case SDLK_RIGHT:
                        return set_position(0, 1);
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    return direction;
}

// Function to clean up SDL resources
void cleanupSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
