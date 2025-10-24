#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GRID_SIZE 20
#define MAX_SNAKE_LENGTH 400

typedef struct {
    int x;
    int y;
} Position;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    Position positions[MAX_SNAKE_LENGTH];
    int length;
    Direction direction;
} Snake;

typedef struct {
    Position pos;
} Food;

// Function declarations
void initializeSnake(Snake *snake);
void spawnFood(Food *food, const Snake *snake);
bool updateSnake(Snake *snake, Food *food);
void handleInput(SDL_Event *event, Snake *snake, bool *running);
void renderGame(SDL_Renderer *renderer, const Snake *snake, const Food *food);
bool checkCollision(const Snake *snake);

int main(void) {
    srand(time(NULL));


    SDL_Window *window = SDL_CreateWindow("Snake Game", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "Window creation error: %s\n", SDL_GetError());
        return 1;
    } 

    //printf("SDL2 initialized successfully.\n");

    SDL_Renderer *renderer = SDL_CreateRenderer(window, 0);
    if (!renderer) {
        fprintf(stderr, "Renderer creation error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Snake snake;
    Food food;
    bool running = true;
    Uint32 lastUpdateTime = 0;
    const int frameDelay = 100; // Controls game speed (milliseconds)

    initializeSnake(&snake);
    spawnFood(&food, &snake);

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handleInput(&event, &snake, &running);
        }

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastUpdateTime >= frameDelay) {
            if (!updateSnake(&snake, &food)) {
                running = false;
                break;
            }
            lastUpdateTime = currentTime;
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        renderGame(renderer, &snake, &food);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void initializeSnake(Snake *snake) {
    snake->length = 3;
    snake->direction = RIGHT;
    // Start in the middle of the screen
    int startX = (WINDOW_WIDTH / GRID_SIZE) / 2;
    int startY = (WINDOW_HEIGHT / GRID_SIZE) / 2;
    
    for (int i = 0; i < snake->length; i++) {
        snake->positions[i].x = startX - i;
        snake->positions[i].y = startY;
    }
}

void spawnFood(Food *food, const Snake *snake) {
    int maxX = (WINDOW_WIDTH / GRID_SIZE) - 1;
    int maxY = (WINDOW_HEIGHT / GRID_SIZE) - 1;
    
    do {
        food->pos.x = rand() % maxX;
        food->pos.y = rand() % maxY;
        
        // Check if food spawned on snake
        bool onSnake = false;
        for (int i = 0; i < snake->length; i++) {
            if (food->pos.x == snake->positions[i].x && 
                food->pos.y == snake->positions[i].y) {
                onSnake = true;
                break;
            }
        }
        if (!onSnake) break;
    } while (1);
}

bool updateSnake(Snake *snake, Food *food) {
    // Store previous head position
    Position newHead = snake->positions[0];
    
    // Update head position based on direction
    switch (snake->direction) {
        case UP:    newHead.y--; break;
        case DOWN:  newHead.y++; break;
        case LEFT:  newHead.x--; break;
        case RIGHT: newHead.x++; break;
    }
    
    // Check wall collision
    if (newHead.x < 0 || newHead.x >= WINDOW_WIDTH / GRID_SIZE ||
        newHead.y < 0 || newHead.y >= WINDOW_HEIGHT / GRID_SIZE) {
        return false;
    }
    
    // Move body
    for (int i = snake->length - 1; i > 0; i--) {
        snake->positions[i] = snake->positions[i - 1];
    }
    snake->positions[0] = newHead;
    
    // Check food collision
    if (newHead.x == food->pos.x && newHead.y == food->pos.y) {
        snake->length++;
        spawnFood(food, snake);
    }
    
    return !checkCollision(snake);
}

void handleInput(SDL_Event *event, Snake *snake, bool *running) {
    if (event->type == SDL_EVENT_QUIT) {
        *running = false;
    }
    else if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.scancode) {
            case SDL_SCANCODE_UP:
                if (snake->direction != DOWN) snake->direction = UP;
                break;
            case SDL_SCANCODE_DOWN:
                if (snake->direction != UP) snake->direction = DOWN;
                break;
            case SDL_SCANCODE_LEFT:
                if (snake->direction != RIGHT) snake->direction = LEFT;
                break;
            case SDL_SCANCODE_RIGHT:
                if (snake->direction != LEFT) snake->direction = RIGHT;
                break;
            case SDL_SCANCODE_ESCAPE:
                *running = false;
                break;
            default:
                break;
        }
    }
}

void renderGame(SDL_Renderer *renderer, const Snake *snake, const Food *food) {
    // Render snake
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int i = 0; i < snake->length; i++) {
        SDL_FRect rect = {
            snake->positions[i].x * GRID_SIZE,
            snake->positions[i].y * GRID_SIZE,
            GRID_SIZE - 1,
            GRID_SIZE - 1
        };
        SDL_RenderFillRect(renderer, &rect);
    }
    
    // Render food
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_FRect foodRect = {
        food->pos.x * GRID_SIZE,
        food->pos.y * GRID_SIZE,
        GRID_SIZE - 1,
        GRID_SIZE - 1
    };
    SDL_RenderFillRect(renderer, &foodRect);
}

bool checkCollision(const Snake *snake) {
    // Check if snake head collides with its body
    for (int i = 1; i < snake->length; i++) {
        if (snake->positions[0].x == snake->positions[i].x &&
            snake->positions[0].y == snake->positions[i].y) {
            return true;
        }
    }
    return false;
}
