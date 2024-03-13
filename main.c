#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

struct snakeHead {
    Vector2 position;
    Vector2 speed;
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

void drawGrid(int gridSize, int gridPx);
head initPlayer(int screenWidth, int screenHeight);
int movePlayer(head player);
void drawSnake (head player, int gridSize, int screenWidth);
void directionChange (head player, int gridSize, int gridPx);
int isNearGridCenter (head player, int gridPx);

int main() {
    //get screen dimensions
    int screenWidth = GRID_SIZE * 50;
    int screenHeight = GRID_SIZE * 50;
    int gridPx = 50;

    //initialisation of player
    head player = initPlayer(screenWidth, screenHeight);

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
                drawSnake(player, GRID_SIZE, screenWidth);
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

head initPlayer(int screenWidth, int screenHeight) {
    head player = (head)malloc(sizeof(snakeHead));
    player->position.x = screenWidth >> 1; //screenWidth / 2
    player->position.y = screenHeight >> 1;
    //by default, snake will go upwards
    player->speed.x = 0;
    player->speed.y = -SNAKE_SPEED;
    player->next = NULL;
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
    return 1;
}

void drawSnake (head player, int gridSize, int screenWidth) {
    int bodySize = screenWidth / gridSize * 0.3;
    DrawCircle(player->position.x, player->position.y, bodySize, BLACK);
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
        printf("%s.", player->direction);
        for (int i = 0; i < k; i++)
            player->direction[i] = player->direction[i + 1];
        printf("%s\n", player->direction);
    }
}

int isNearGridCenter (head player, int gridPx) {
    int distCenterX = ((int)player->position.x % gridPx + (gridPx >> 1)) % gridPx;
    int distCenterY = ((int)player->position.y % gridPx + (gridPx >> 1)) % gridPx;
    if (-(abs((int)player->speed.x) >> 1) <= distCenterX &&
        (abs((int)player->speed.x) >> 1) >= distCenterX &&
        -(abs((int)player->speed.y) >> 1) <= distCenterY &&
        (abs((int)player->speed.y) >> 1) >= distCenterY)
        return 1;
    return 0;
}