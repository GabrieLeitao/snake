#include "snake.h"
#include "utils.h"


// Rendering functions using SDL
void initializeSDL();
void drawGame(WORLD world, SNAKE *snake);
void drawRoundedRect(SDL_Renderer *renderer, SDL_Rect rect, int radius, SDL_Color color);
void drawObject(SDL_Renderer *renderer, SDL_Rect cellRect, SDL_Color Color, int objSize);
SDL_Color get_pulse_color(SDL_Color baseColor, float glowSpeed, Uint32 creationTime, float delta);
SDL_Color get_glow_color(SDL_Color baseColor, float glowSpeed, Uint32 creationTime, float delta);
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
    Uint32 lastLogicUpdateTime = SDL_GetTicks();
    Uint32 lastRenderTime = SDL_GetTicks();
    const Uint32 renderInterval = 1000 / RENDER_FPS;
    Uint32 objectAddInterval = 2000; // 2 seconds
    Uint32 lastObjectAddTime = SDL_GetTicks();
    // Main game loop
    do {
        startTime = SDL_GetTicks();

        // Atualizar direcao
        direction = get_direction(direction);
        
        // Update game logic based on snake speed
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastLogicUpdateTime >= delay) {
            quit = WorldSnakeInteraction(world, &snake, direction);
            lastLogicUpdateTime = currentTime;

            // Gradually decrease delay to increase speed
            iteration++;
            delay = DELAY_TIME * exp(-lambda * iteration) + 20;
        }

        // Add new objects at a gradually increasing rate
        if (currentTime - lastObjectAddTime >= objectAddInterval) {
            addNewObject(world);
            lastObjectAddTime = currentTime;

            // Gradually decrease the interval to add new objects
            objectAddInterval = objectAddInterval * 0.9; // Decrease interval by 5%
            if (objectAddInterval < 500) { // Set a minimum interval of 1 second
                objectAddInterval = 500;
            }
        }

        // Render the game at a fixed interval (60 FPS)
        if (currentTime - lastRenderTime >= renderInterval) {
            drawGame(world, &snake);
            lastRenderTime = currentTime;
        }

        if (!gameOn(world))
        {
            printf("\n !! You won, Congrats!! \n");
            break;
        }
        
        // Calculate time taken for frame
        endTime = SDL_GetTicks();
        deltaTime = endTime - startTime;

        // Delay to control frame rate
        if (deltaTime < renderInterval) {
            SDL_Delay(renderInterval - deltaTime);
        }
            
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
    int windowWidth = WINDOW_WIDTH;
    int windowHeight = WINDOW_HEIGHT;

    SDL_DisplayMode DM;
    if (SDL_GetCurrentDisplayMode(0, &DM) == 0) { // 0 = Primary display
        windowHeight = DM.h * 0.8;
        windowWidth = DM.w * 0.8;
    } else {
        fprintf(stderr, "SDL_GetCurrentDisplayMode failed: %s\n", SDL_GetError());
        fprintf(stderr, "Using fallback resolution: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    // Create a window and renderer
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "Unable to create window: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    #if FULLSCREEN
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    #endif

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

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Draw the snake with a green gradient and glow effect
    for (i = 0; i < snake->dim; i++) {
        int c = snake->pos[i].c;
        int l = snake->pos[i].l;

        // Ensure the indices are within bounds
        if (c >= 0 && c < MAXW && l >= 0 && l < MAXH) {
            SDL_Rect cellRect = {(c + 1) * CELL_SIZE, (l + 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            int r = 110;
            int g = 170 - (i * (3 + i % 2));
            r = clamp(r, 50, 110);
            g = clamp(g, 110, 170);
            SDL_Color snakeColor = {r, g, 69, 255}; // Soft green gradient

            // Apply the glowing effects if the snake is glowing
            for (int k = 0; k < snake->activeGlows; k++) {
                if (snake->glowing[k] && i == snake->glowCounter[k]) {
                    snakeColor = get_glow_color(snakeColor, 100.0, snake->glowStartTime[k], 40);
                }
            }
            drawRoundedRect(renderer, cellRect, CELL_SIZE / 4, snakeColor);
        }
    }

     // Update the glow counters to propagate the glowing effects
     Uint32 currentTime = SDL_GetTicks();
     for (int k = 0; k < snake->activeGlows; k++) {
        if (snake->glowing[k]) {
            snake->glowFrameCounter[k]++;
            if (currentTime - snake->glowFrameCounter[k] >= 3.14159 * 100 * 2) {
                snake->glowCounter[k]++;
                snake->glowStartTime[k] = SDL_GetTicks();
                if (snake->glowCounter[k] >= snake->dim) {
                    snake->glowing[k] = 0; // Stop the glowing effect when it reaches the tail
                }
            }
        }
    }

    // Remove inactive glows
    int activeGlows = 0;
    for (int k = 0; k < snake->activeGlows; k++) {
        if (snake->glowing[k]) {
            snake->glowing[activeGlows] = snake->glowing[k];
            snake->glowCounter[activeGlows] = snake->glowCounter[k];
            snake->glowFrameCounter[activeGlows] = snake->glowFrameCounter[k];
            snake->glowStartTime[activeGlows] = snake->glowStartTime[k];
            activeGlows++;
        }
    }
    snake->activeGlows = activeGlows;

    // Draw objects with softer colors and glow effect for bonus and loop
    for (i = 0; i < MAXH; ++i) {
        for (j = 0; j < MAXW; ++j) {
            SDL_Rect cellRect = {(j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_Color baseColor = {0, 0, 0, 0};
            int objSize = 0;
            float glowSpeed = 1000.0; // Default glow speed

            if (world[i][j].ch == BONUS_CHAR) {
                baseColor = (SDL_Color){74, 144, 226, 255}; // Soft blue
                objSize = BONUS_SIZE;
                glowSpeed = 500.0; // glow
            } else if (world[i][j].ch == MINUS_CHAR) {
                baseColor = (SDL_Color){233, 78, 78, 255}; // Soft red
                objSize = MINUS_SIZE;
            } else if (world[i][j].ch == LOOP_CHAR) {
                baseColor = (SDL_Color){168, 30, 170, 255}; // Soft brown
                objSize = LOOP_SIZE;
                glowSpeed = 2000.0; // Slower glow
            }

            if (baseColor.r != 0) { // Skip empty spaces
                SDL_Color glowColor = get_pulse_color(baseColor, glowSpeed, world[i][j].creation_time, 40);
                drawObject(renderer, cellRect, glowColor, objSize);
            }
        }
    }

    // Draw borders
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 175);
    for (i = 0; i <= MAXH + 1; ++i)
    {
        SDL_Rect cellRect = {(MAXW + 1) * CELL_SIZE, i * CELL_SIZE, CELL_SIZE / 2, CELL_SIZE};
        SDL_RenderFillRect(renderer, &cellRect);
        SDL_Rect cellRect2 = {CELL_SIZE / 2, i * CELL_SIZE, CELL_SIZE / 2, CELL_SIZE};
        SDL_RenderFillRect(renderer, &cellRect2);
    }
    for(j = 0; j <= MAXW + 1; ++j)
    {
        SDL_Rect cellRect = {j * CELL_SIZE, (MAXH + 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE / 2};
        SDL_RenderFillRect(renderer, &cellRect);
        SDL_Rect cellRect2 = {j * CELL_SIZE, CELL_SIZE / 2, CELL_SIZE, CELL_SIZE / 2};
        SDL_RenderFillRect(renderer, &cellRect2);
    }
        
    // Update the screen
    SDL_RenderPresent(renderer);
}

SDL_Color get_pulse_color(SDL_Color baseColor, float glowSpeed, Uint32 creationTime, float delta)
{
    Uint32 currentTime = SDL_GetTicks();
    float time = (currentTime - creationTime) / glowSpeed;
    float glowFactor = pow(0.5 + 0.5 * sin(time), 6);
    int r = clamp(baseColor.r + (int)(delta * glowFactor), 0, 255);
    int g = clamp(baseColor.g + (int)(delta * glowFactor), 0, 255);
    int b = clamp(baseColor.b + (int)(delta * glowFactor), 0, 255);
    SDL_Color glowColor = {r, g, b, 255};
    return glowColor;
}

SDL_Color get_glow_color(SDL_Color baseColor, float glowSpeed, Uint32 creationTime, float delta)
{
    Uint32 currentTime = SDL_GetTicks();
    float time = (currentTime - creationTime) / glowSpeed;
    float glowFactor = (1 + sin(time)) / 2;
    int r = clamp(baseColor.r + (int)(delta * glowFactor), 0, 255);
    int g = clamp(baseColor.g + (int)(delta * glowFactor), 0, 255);
    int b = clamp(baseColor.b + (int)(delta * glowFactor), 0, 255);
    SDL_Color glowColor = {r, g, b, 255};
    return glowColor;
}

void drawObject(SDL_Renderer *renderer, SDL_Rect cellRect, SDL_Color Color, int objSize)
{
    // Adjust the size of the cellRect based on objSize
    cellRect.w = objSize;
    cellRect.h = objSize;
    cellRect.x += (CELL_SIZE - objSize) / 2;
    cellRect.y += (CELL_SIZE - objSize) / 2;

    drawRoundedRect(renderer, cellRect, objSize / 4, Color);
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
