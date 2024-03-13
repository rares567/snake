#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define GRID_SIZE 15
#define SNAKE_SPEED 4

//colors are based on rgba
#define GRID_LIGHT_GREEN (Color) {170, 215, 81, 255}
#define GRID_GREEN (Color) {162, 209, 73, 255}

typedef enum gameScreen {
    LOGO, TITLE, GAMEPLAY, WIN, LOSE
} gameScreen;

typedef struct snakeHead snakeHead, *head;
typedef struct snakeBody snakeBody, *body;

//the snake is a singly linked list with sentinel
struct snakeHead {
    Vector2 position;
    Vector2 speed;
    int radius;
    /*
    The snake will have to stick to the grid so when the
    direction changes it will not necessarily turn on the spot
    (it will memorise up to 2 actions and turn only when it can)
    (6 for '\0' to use string operations)
    */
    char direction[3];
    body next;
};

struct snakeBody {
    Vector2 position;
    body next;
};

typedef struct food {
    Vector2 position;
    int radius;
} Food;

void drawGrid(int gridSize, int gridPx);
head initPlayer(int screenWidth, int screenHeight, int gridSize);
int movePlayer(head player);
void drawSnake (head player);
void directionChange (head player, int gridSize, int gridPx);
bool isNearGridCenter (head player, int gridPx);
void createFood(Food *food, int gridSize, int gridPx);
void drawFood (Food *food);
Food *initFood(int gridSize, int gridPx);
bool hasHitFood(head player, Food *food);
void addBodySegm(head player);
void freeAll(head player, Food *food);

int main() {
    //get screen dimensions
    int screenWidth = GRID_SIZE * 50;
    int screenHeight = GRID_SIZE * 50;
    int gridPx = 50;

    //initialisation of player and food
    head player = initPlayer(screenWidth, screenHeight, GRID_SIZE);
    Food *food = initFood(GRID_SIZE, gridPx);

    //seed the rng
    SetRandomSeed(time(NULL));

    //define window
    InitWindow(screenWidth, screenHeight, "Snake");

    //define screen type
    gameScreen screen = GAMEPLAY;
    //int frameCounter = 0;
    //int gameResult = -1; //-1 not defined, 0 lose, 1 win
    //bool gamePaused = 0;

    //set fps of game
    SetTargetFPS(60);

    //main game
    while (!WindowShouldClose()) {
        //game processes
        switch (screen) {
            case LOGO:
                //ADD LOGO
            break;
            case TITLE:
                
            break;
            case GAMEPLAY:
                directionChange(player, GRID_SIZE, gridPx);
                if (hasHitFood(player, food)) {
                    createFood(food, GRID_SIZE, gridPx);
                    addBodySegm(player);
                }
                if (!movePlayer(player)) {
                    screen = LOSE;
                    break;
                }
            break;
            case WIN:

            break;
            case LOSE:

            break;
            default: break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        //game visuals
        switch (screen) {
            case LOGO:
                //ADD LOGO
            break;
            case TITLE:
                
            break;
            case GAMEPLAY:
                drawGrid(GRID_SIZE, gridPx);
                drawSnake(player);
                drawFood(food);
            break;
            case WIN:

            break;
            case LOSE:
                DrawText("YOU LOST", (screenWidth >> 1) - 100, (screenHeight >> 1) - 40, 40, BLACK);
            break;
            default: break;
        }

        EndDrawing();
    }
    freeAll(player, food);
    CloseWindow();
    return 0;
}

void drawGrid(int gridSize, int gridPx) {
    ClearBackground(GRID_LIGHT_GREEN);
    int i, pos;
    for (i = 1; i <= gridSize; i++) {
        pos = 0;
        //when on the odd numbered grid lines, offset first square
        if (i % 2) pos += gridPx; 
        //while a square can fit
        while (pos <= (gridSize - 1) * gridPx) {
            DrawRectangle(pos, (i - 1) * gridPx, gridPx, gridPx, GRID_GREEN);
            pos += gridPx << 1; //2*gridPx
        }
    }         
}

head initPlayer(int screenWidth, int screenHeight, int gridSize) {
    head player = (head)malloc(sizeof(snakeHead));
    player->position.x = screenWidth >> 1; //screenWidth / 2
    player->position.y = screenHeight >> 1;
    //by default, snake will go upwards
    player->speed.x = 0;
    player->speed.y = -SNAKE_SPEED;
    player->radius = (int)(screenWidth / gridSize * 0.4);
    player->next = (body)malloc(sizeof(snakeBody));
    player->next->next = NULL;
    player->direction[0] = 'N';
    player->direction[1] = '\0';
    return player;
}

int movePlayer(head player) {
    player->position.x += player->speed.x;
    player->position.y += player->speed.y;
    if (player->position.x <= 0 || player->position.x >= GetRenderWidth() ||
        player->position.y <= 0 || player->position.y >= GetRenderHeight())
        return 0;
    body p = player->next;
    
    return 1;
}

void drawSnake (head player) {
    //skip sentinel
    body p = player->next->next;
    DrawCircle(player->position.x, player->position.y, player->radius, BLACK);
    while (p != NULL) {
        DrawCircle(p->position.x, p->position.y, player->radius, BLACK);
        p = p->next;
    }
}

void directionChange (head player, int gridSize, int gridPx) {
    int k = strlen(player->direction);
    //memorise new direction only if it doesnt exceed 2 chars
    if (k < 2) {
        /*
        memorise in the string the directions inputed only if
        it does not coincide with the direction already going
        (this way it wont fill with same direction when spamming
        which would create input lag by first "changing direction"
        to the same one, esentially blocking new valid inputs)
        */
        if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && player->direction[0] != 'W') {
            player->direction[k++] = 'W';
            player->direction[k++] = '\0';
        }
        else if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && player->direction[0] != 'N') {
            player->direction[k++] = 'N';
            player->direction[k++] = '\0';
        }
        else if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && player->direction[0] != 'E') {
            player->direction[k++] = 'E';
            player->direction[k++] = '\0';
        }
        else if ((IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) && player->direction[0] != 'S') {
            player->direction[k++] = 'S';
            player->direction[k++] = '\0';
        }
    }
    //check whether the snake is close enough to the center to turn
    if (isNearGridCenter(player, gridPx)) {
        player->position.x = player->position.x - (int)player->position.x % gridPx + (gridPx >> 1);
        player->position.y = player->position.y - (int)player->position.y % gridPx + (gridPx >> 1);
        switch (player->direction[0]) {
            case 'N':
                player->speed.y = -SNAKE_SPEED;
                player->speed.x = 0;
            break;
            case 'S':
                player->speed.y = SNAKE_SPEED;
                player->speed.x = 0;
            break;
            case 'E':
                player->speed.y = 0;
                player->speed.x = SNAKE_SPEED;
            break;
            case 'W':
                player->speed.y = 0;
                player->speed.x = -SNAKE_SPEED;
            break;
            default: break;
        }
        /*
        shift the chars in the string left, removing the first one
        (which has been executed above)
        */
        for (int i = 0; i < k; i++)
            player->direction[i] = player->direction[i + 1];
    }
}

bool isNearGridCenter (head player, int gridPx) {
    //calculate distance from grid-cell center
    int distCenterX = ((int)player->position.x % gridPx + (gridPx >> 1)) % gridPx;
    int distCenterY = ((int)player->position.y % gridPx + (gridPx >> 1)) % gridPx;
    //check if player is as close to center as can get
    if (-(abs((int)player->speed.x) >> 1) <= distCenterX &&
        (abs((int)player->speed.x) >> 1) >= distCenterX &&
        -(abs((int)player->speed.y) >> 1) <= distCenterY &&
        (abs((int)player->speed.y) >> 1) >= distCenterY)
        return true;
    return false;
}

void createFood(Food *food, int gridSize, int gridPx) {
    food->position.x = GetRandomValue(0, gridSize - 1) * gridPx + (gridPx >> 1);
    food->position.y = GetRandomValue(0, gridSize - 1) * gridPx + (gridPx >> 1);
}

void drawFood(Food *food) {
    DrawCircle(food->position.x, food->position.y, food->radius, RED);
}

Food *initFood(int gridSize, int gridPx) {
    Food *food = (Food *)malloc(sizeof(food));
    food->position.x = gridSize * gridPx >> 1;
    food->position.y = (int)(gridSize >> 2) * gridPx + (gridPx >> 1);
    food->radius = gridPx * 0.3;
    return food;
}

bool hasHitFood(head player, Food *food) {
    if (abs(player->position.x - food->position.x) <= player->radius + food->radius &&
        abs(player->position.y - food->position.y) <= player->radius + food->radius)
        return true;
    return false;
}

//add body segment at the beginning
void addBodySegm(head player) {
    body newSegm = (body)malloc(sizeof(snakeBody));
    newSegm->position.x = player->position.x;
    newSegm->position.y = player->position.y;
    //skip sentinel
    newSegm->next = player->next->next;
    player->next->next = newSegm;
}

void freeAll(head player, Food *food) {
    body p = player->next;
    body tmp;
    while (p != NULL) {
        tmp = p->next;
        free(p);
        p = tmp;
    }
    free(player);
    free(food);
}