#include "snake.h"
#include "utils.h"


// Rendering functions using SDL
void initializeSDL();
void drawGame(WORLD world, SNAKE *snake);
void drawRoundedRect(SDL_Renderer *renderer, SDL_Rect rect, int radius, SDL_Color color);
POSITION get_direction(POSITION direction);
void cleanupSDL();


SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

Uint32 elapsedTime = 0;

int main() {
    srand(time(NULL));

    // Initialize SDL and game objects
    initializeSDL();
    WORLD world;
    SNAKE snake;
    POSITION direction;
    int quit = 0;
    float delay = DELAY_TIME;
    float lambda = 0.005; // Decay rate
    int iteration = 0;

    initWorld(world);
    initSnake(&snake);

    direction = set_position(0, 1); // line, column

    Uint32 startTime, endTime, deltaTime;
    // Main game loop
    do {
        startTime = SDL_GetTicks();

        // Atualizar direcao
        direction = get_direction(direction);
        
        quit = WorldSnakeInteraction(world, &snake, direction);
        // Draw the game
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

        // Gradually decrease delay to increase speed
        iteration++;
        delay = DELAY_TIME * exp(-lambda * iteration) + 10;
            
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
void drawGame(WORLD world, SNAKE *snake)
{
    int i, j;
    
    // Background color (soft dark blue)
    SDL_SetRenderDrawColor(renderer, 44, 62, 80, 255);
    SDL_RenderClear(renderer);

    elapsedTime = SDL_GetTicks();

    // Draw the snake with a green gradient
    for (i = 0; i < snake->dim; i++) {
        SDL_Rect cellRect = {(snake->pos[i].c + 1) * CELL_SIZE, (snake->pos[i].l + 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE};
        int r = 110 - (i * 5);
        int g = 190 - (i * 5);
        r = clamp(r, 0, 255);
        g = clamp(g, 0, 255);
        SDL_Color snakeColor = {r, g, 69, 255}; // Soft green gradient
        drawRoundedRect(renderer, cellRect, CELL_SIZE / 4, snakeColor);
    }

    // Draw objects with softer colors
    for (i = 0; i < MAXL; ++i) {
        for (j = 0; j < MAXC; ++j) {
            SDL_Rect cellRect = {(j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_Color objColor = {0, 0, 0, 0};

            if (world[i][j] == BONUS_CHAR) {
                objColor = (SDL_Color){74, 144, 226, 255}; // Soft blue
            } else if (world[i][j] == MINUS_CHAR) {
                objColor = (SDL_Color){233, 78, 78, 255}; // Soft red
            } else if (world[i][j] == LOOP_CHAR) {
                objColor = (SDL_Color){168, 30, 170, 255}; // Soft brown
            }

            if (objColor.r != 0) { // Skip empty spaces
                drawRoundedRect(renderer, cellRect, CELL_SIZE / 6, objColor);
            }
        }
    }

    // Draw borders
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 175);
    for (i = 0; i <= MAXL + 1; ++i)
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

void drawRoundedRect(SDL_Renderer *renderer, SDL_Rect rect, int radius, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Fill center rectangle
    SDL_Rect centerRect = {rect.x + radius, rect.y + radius, rect.w - 2 * radius, rect.h - 2 * radius};
    SDL_RenderFillRect(renderer, &centerRect);

    // Fill side rectangles (excluding rounded areas)
    SDL_Rect leftRect = {rect.x, rect.y + radius, radius, rect.h - 2 * radius};
    SDL_RenderFillRect(renderer, &leftRect);
    SDL_Rect rightRect = {rect.x + rect.w - radius, rect.y + radius, radius, rect.h - 2 * radius};
    SDL_RenderFillRect(renderer, &rightRect);

    SDL_Rect topRect = {rect.x + radius, rect.y, rect.w - 2 * radius, radius};
    SDL_RenderFillRect(renderer, &topRect);
    SDL_Rect bottomRect = {rect.x + radius, rect.y + rect.h - radius, rect.w - 2 * radius, radius};
    SDL_RenderFillRect(renderer, &bottomRect);

    // Draw rounded corners using pixel-by-pixel rendering
    for (int w = 0; w < radius; w++) {
        for (int h = 0; h < radius; h++) {
            if ((w * w + h * h) <= (radius * radius)) { // Circle equation
                SDL_RenderDrawPoint(renderer, rect.x + radius - w, rect.y + radius - h); // Top-left
                SDL_RenderDrawPoint(renderer, rect.x + rect.w - radius + w - 1, rect.y + radius - h); // Top-right
                SDL_RenderDrawPoint(renderer, rect.x + radius - w, rect.y + rect.h - radius + h - 1); // Bottom-left
                SDL_RenderDrawPoint(renderer, rect.x + rect.w - radius + w - 1, rect.y + rect.h - radius + h - 1); // Bottom-right
            }
        }
    }
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
