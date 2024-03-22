#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define GRID_SIZE 15
#define SNAKE_SPEED 2

//colors are based on rgba
#define GRID_LIGHT_GREEN (Color) {170, 215, 81, 255}
#define GRID_GREEN (Color) {162, 209, 73, 255}

typedef enum gameScreen {
    LOGO, TITLE, GAMEPLAY, WIN, LOSE
} gameScreen;

typedef struct snakeSegm snakeSegm, *segm;
typedef struct food Food;

//the snake is a singly linked list
struct snakeSegm {
    Vector2 position;
    Vector2 speed;
    int radius;
    /*
    The snake will have to stick to the grid so when the
    direction changes it will not necessarily turn on the spot
    (it will memorise up to 2 actions and turn only when it can)
    (3 for '\0' to use string operations)
    */
    char direction[3];
    segm next;
};

struct food {
    Vector2 position;
    int radius;
};

void drawGrid(int gridSize, int gridPx);
segm initPlayer(int gridSize, int gridPx);
int movePlayer(segm player, int frameCounter, int framePause);
void drawSnake (segm player);
void directionChange (segm player, int gridSize, int gridPx);
bool isNearGridCenter (segm player, int gridPx);
void createFood(Food *food, segm player, int gridSize, int gridPx);
void drawFood (Food *food);
Food *initFood(int gridSize, int gridPx);
bool hasHitFood(segm player, Food *food);
void addSegm(segm player);
void freeAll(segm player, Food *food);
void turnSegm (segm segm, int gridPx);
bool hasHitBody (segm player);

int main() {
    //set screen dimensions
    int gridPx = 50;
    int screenWidth = GRID_SIZE * gridPx;
    int screenHeight = GRID_SIZE * gridPx;

    //initialisation of player and food
    segm player = initPlayer(GRID_SIZE, gridPx);
    Food *food = initFood(GRID_SIZE, gridPx);

    //seed the rng
    SetRandomSeed(time(NULL));

    //define window
    InitWindow(screenWidth, screenHeight, "Snake");

    //define screen type
    gameScreen screen = GAMEPLAY;
    int frameCounter = 0;
    //int gameResult = -1; //-1 not defined, 0 lose, 1 win
    //bool gamePaused = 0;

    //set fps of game
    SetTargetFPS(120);

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
                    createFood(food, player, GRID_SIZE, gridPx);
                    addSegm(player);
                    frameCounter = 0;
                }
                if (!movePlayer(player, frameCounter, 20)) {
                    screen = LOSE;
                    break;
                }
                frameCounter++;
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

//initialise snake head in the middle of the grid
segm initPlayer(int gridSize, int gridPx) {
    segm player = (segm)malloc(sizeof(snakeSegm));
    player->position.x = (gridSize >> 1) * gridPx + (gridPx >> 1); //gridSize / 2
    player->position.y = (gridSize >> 1) * gridPx + (gridPx >> 1);
    //by default, snake will go upwards
    player->speed.x = 0;
    player->speed.y = -SNAKE_SPEED;
    player->radius = (int)(gridPx * 0.4);
    player->next = NULL;
    player->direction[0] = 'N';
    player->direction[1] = '\0';
    return player;
}

/*
moves snake head and pauses snake body when creating a new segment
returns 0 if has lost and 1 otherwise
*/
int movePlayer(segm player, int frameCounter, int framePause) {
    player->position.x += player->speed.x;
    player->position.y += player->speed.y;
    segm p = player->next;
    if ((frameCounter / framePause) >= 1) {
        while (p != NULL) {
            p->position.x += p->speed.x;
            p->position.y += p->speed.y;
            p = p->next;
        }
    }
    //if it reaches end of screen or has hit itself
    if (player->position.x <= 0 || player->position.x >= GetRenderWidth() ||
        player->position.y <= 0 || player->position.y >= GetRenderHeight() ||
        hasHitBody(player))
        return 0;
    return 1;
}

void drawSnake (segm player) {
    segm p = player;
    do {
        DrawCircle(p->position.x, p->position.y, p->radius, BLACK);
        p = p->next;
    } while (p != NULL);
}

void directionChange (segm player, int gridSize, int gridPx) {
    //separate case for player head
    int k = strlen(player->direction);
    //memorise new direction only if it doesnt exceed 1 char
    if (k < 2) {
        /*
        memorise in the string the directions inputed only if
        it does not coincide with the direction already going
        (this way it wont fill with same direction when spamming
        which would create input lag by first "changing direction"
        to the same one, esentially blocking new valid inputs)
        */

        /*
        player->speed.x <= 0
        (happens when it is either moving up or down (= 0), or when 
        going 'W' (= snake_speed < 0). We also memorise going 'W' when 
        already going that direction because it can have a different
        direction in buffer (it can go up and then left again)). This
        condition has the OR operator in case there is a valid move
        in buffer in which case there can be added the 'E' direction
        (going W means it cannot go E immediately but it can go N/S and then E).
        etc. for rest
        */

       /*
       explanation of condition on 'W'
       */

        if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && !(!player->direction[0] && player->speed.x <0) &&
            (player->speed.x <= 0 || player->direction[0]) && (player->direction[0] != 'E') && player->direction[0] != 'W') {
            player->direction[k++] = 'W';
            player->direction[k++] = '\0';
        }
        else if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && !(!player->direction[0] && player->speed.y <0) &&
                 (player->speed.y <= 0 || player->direction[0]) && (player->direction[0] != 'S') && player->direction[0] != 'N') {
            player->direction[k++] = 'N';
            player->direction[k++] = '\0';
        }
        else if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && !(!player->direction[0] && player->speed.x >0) &&
                 (player->speed.x >= 0 || player->direction[0]) && (player->direction[0] != 'W') && player->direction[0] != 'E') {
            player->direction[k++] = 'E';
            player->direction[k++] = '\0';
        }
        else if ((IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) && !(!player->direction[0] && player->speed.y >0) &&
                 (player->speed.y >= 0 || player->direction[0]) && (player->direction[0] != 'N') && player->direction[0] != 'S') {
            player->direction[k++] = 'S';
            player->direction[k++] = '\0';
        }
    }
    //check whether the head is close enough to the center to turn
    if (isNearGridCenter(player, gridPx)) {
        printf("%s\n", player->direction);
        turnSegm(player, gridPx);
        //transmit the move executed to next segment
        if (player->next != NULL)
            player->next->direction[0] = player->direction[0];
        /*
        shift the chars in the string left, removing the first one
        (which has been executed above)
        */
        for (int i = 0; i < k; i++)
            player->direction[i] = player->direction[i + 1];
    }
    segm p = player->next;
    while (p != NULL) {
        if (isNearGridCenter(p, gridPx)) {
            turnSegm(p, gridPx);
            if (p->next != NULL)
                p->next->direction[0] = p->direction[0];
        }
        p = p->next;
    }
}

bool isNearGridCenter (segm player, int gridPx) {
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

void createFood(Food *food, segm player, int gridSize, int gridPx) {
    //get initial random values
    int posX = GetRandomValue(0, gridSize - 1) * gridPx + (gridPx >> 1);
    int posY = GetRandomValue(0, gridSize - 1) * gridPx + (gridPx >> 1);
    //check if position overlaps with snake
    bool isInvalid;
    segm p;
    do {
        //assume the coordinates are valid
        isInvalid = false;
        //start from snake head
        p = player;
        //check if food is inside any of the segm
        while (p != NULL) {
            if (abs(p->position.x - posX) <= (p->radius + food->radius) &&
                abs(p->position.y - posY) <= (p->radius + food->radius)) {
                    posX = GetRandomValue(0, gridSize - 1) * gridPx + (gridPx >> 1);
                    posY = GetRandomValue(0, gridSize - 1) * gridPx + (gridPx >> 1);
                    /*
                    makes the while condition true so it redoes the checks
                    with the new coordinates
                    */
                    isInvalid = true;
                    break;
                }
            p = p->next;
        }
    } while (isInvalid);
    food->position.x = posX;
    food->position.y = posY;
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

bool hasHitFood(segm player, Food *food) {
    if (abs(player->position.x - food->position.x) <= player->radius + food->radius &&
        abs(player->position.y - food->position.y) <= player->radius + food->radius)
        return true;
    return false;
}

//add segment right after snake head
void addSegm(segm player) {
    segm newSegm = (segm)malloc(sizeof(snakeSegm));
    newSegm->position.x = player->position.x;
    newSegm->position.y = player->position.y;
    newSegm->radius = player->radius;
    /*
    the segments after head will only need to memorise one
    direction at a time (its just copying last head movement)
    */
    newSegm->direction[0] = player->direction[0];
    newSegm->speed.x = player->speed.x;
    newSegm->speed.y = player->speed.y;
    newSegm->next = player->next;
    player->next = newSegm;
}

void freeAll(segm player, Food *food) {
    segm p = player;
    segm tmp;
    do {
        tmp = p->next;
        free(p);
        p = tmp;
    } while (p != NULL);
    free(food);
}

void turnSegm (segm segm, int gridPx) {
    segm->position.x = segm->position.x - (int)segm->position.x % gridPx + (gridPx >> 1);
    segm->position.y = segm->position.y - (int)segm->position.y % gridPx + (gridPx >> 1);
    switch (segm->direction[0]) {
        case 'N':
            segm->speed.y = -SNAKE_SPEED;
            segm->speed.x = 0;
        break;
        case 'S':
            segm->speed.y = SNAKE_SPEED;
            segm->speed.x = 0;
        break;
        case 'E':
            segm->speed.y = 0;
            segm->speed.x = SNAKE_SPEED;
        break;
        case 'W':
            segm->speed.y = 0;
            segm->speed.x = -SNAKE_SPEED;
        break;
        default: break;
    }
}

bool hasHitBody (segm player) {
    int i;
    segm p = player->next;
    /*
    move to 4th segm to check collisions since it's impossible to
    hit yourself within first 3 segm (also fixes false positives when
    creating a new segment)
    */
    for (i = 0; i < 3 && p != NULL; i++)
        p = p->next;
    while (p != NULL) {
        if (abs(p->position.x - player->position.x) <= player->radius &&
            abs(p->position.y - player->position.y) <= player->radius)
            return true;
        p = p->next;
    }
    return false;
}