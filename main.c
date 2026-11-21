#include "raylib.h"

#define screenWidth 1000
#define screenHeight 1000
#define bulletsMax 10
#define playerHealth 100
#define scoreToWin 3
#define menu 0
#define playing 1
#define gameOver 2

typedef struct
{
    float x, y;
    bool inUse;
} bullet;

typedef struct
{
    float x, y;
    int health, score, up, down, left, right, shoot;
    Color color;
    bullet bullets[bulletsMax];
} Player;

void resetPlayer(Player *player, float x, Color color, int up, int down, int left, int right, int shoot)
{
    int i;

    (*player).x=x;
    (*player).y=480;
    (*player).health=playerHealth;
    (*player).color=color;
    (*player).up=up;
    (*player).down=down;
    (*player).left=left;
    (*player).right=right;
    (*player).shoot=shoot;

    for (i=0; i<bulletsMax; i++)
    {
        (*player).bullets[i].inUse=false;
    }
}

void movePlayer(Player *player)
{
    if (IsKeyDown((*player).up)==true)
        (*player).y=(*player).y-5;

    if (IsKeyDown((*player).down)==true)
        (*player).y=(*player).y+5;

    if (IsKeyDown((*player).left)==true)
        (*player).x=(*player).x-5;

    if (IsKeyDown((*player).right)==true)
        (*player).x=(*player).x+5;

    if ((*player).x<0)
        (*player).x=0;

    if ((*player).x > screenWidth-40)
        (*player).x = screenWidth-40;

    if ((*player).y<60)
        (*player).y=60;

    if ((*player).y > screenHeight-40)
        (*player).y = screenHeight-40;
}

void shoot(Player *player)
{
    int i;

    for (i=0; i<bulletsMax; i++)
    {
        if ((*player).bullets[i].inUse == false)
        {
            (*player).bullets[i].x = (*player).x+40;
            (*player).bullets[i].y = (*player).y+20;
            (*player).bullets[i].inUse=true;
            break;
        }
    }
}

void updateBullets(Player *shooter, Player *target, int direction)
{
    int i;

    for (i=0; i<bulletsMax; i++)
    {
        if ((*shooter).bullets[i].inUse==true)
        {
            (*shooter).bullets[i].x=(*shooter).bullets[i].x + (5*direction);

            if ((*shooter).bullets[i].x < (*target).x + 40 && (*shooter).bullets[i].x + 12 > (*target).x &&
                (*shooter).bullets[i].y < (*target).y + 40 && (*shooter).bullets[i].y + 6 > (*target).y)
            {
                (*target).health= (*target).health-10;
                (*shooter).bullets[i].inUse=false;
            }
        }
        else if ((*shooter).bullets[i].x<0 || (*shooter).bullets[i].x>screenWidth)
            (*shooter).bullets[i].inUse=false;
    }
}

void drawPlayer(Player player)
{
    DrawRectangle((int)player.x, (int)player.y, 40, 40, player.color);
    DrawRectangle((int)player.x, (int)player.y-10, player.health*40/playerHealth, 6, GREEN);
}

void drawBullets(Player player)
{
    int i;

    for (i=0; i<bulletsMax; i++)
    {
        if (player.bullets[i].inUse==true)
            DrawRectangle((int)player.bullets[i].x, (int)player.bullets[i].y, 12, 6, BLACK);
    }
}

void resetRound(Player *player1, Player *player2)
{
    int i;

    (*player1).x=230;
    (*player1).y=480;
    (*player2).x=730;
    (*player2).y=480;

    (*player1).health=playerHealth;
    (*player2).health=playerHealth;

    for (i=0; i<bulletsMax; i++)
    {
        (*player1).bullets[i].inUse=false;
        (*player2).bullets[i].inUse=false;
    }
}

void drawMenu()
{
    DrawText("BATTLE ARENA", 330, 120, 40, DARKBLUE);
    DrawText("Two Player Battle", 345, 190, 24, DARKGRAY);
    DrawText("Player 1: W A Z S to move, D to shoot", 220, 280, 20, BLUE);
    DrawText("Player 2: Arrow keys to move, ENTER to shoot", 190, 320, 20, RED);
    DrawText("Press ENTER to start", 330, 420, 24, BLACK);
}

void drawGame(Player player1, Player player2, int screen, int winner)
{
    if (screen==menu)
        drawMenu();

    else if (screen==playing)
    {
        DrawText("BATTLE ARENA", 370, 15, 25, DARKGRAY);
        DrawText(TextFormat("Player 1: %d", player1.score), 40, 20, 20, BLUE);
        DrawText(TextFormat("Player 2: %d", player2.score), 740, 20, 20, RED);
        DrawText("ESC: Back to menu", 350, 570, 18, BLACK);

        drawPlayer(player1);
        drawPlayer(player2);
        drawBullets(player1);
        drawBullets(player2);
    }
    else if (screen==gameOver)
    {
        if (winner==1)
            DrawText("PLAYER 1 WINS!", 300, 200, 40, BLUE);
        else
            DrawText("PLAYER 2 WINS!", 300, 200, 40, RED);

        DrawText("Press ENTER to return to menu", 235, 300, 24, BLACK);
    }
}

int main()
{
    Player player1, player2;
    int screen=menu, winner=0;

    InitWindow(screenWidth, screenHeight, "BATTLE ARENA");
    SetTargetFPS(60);

    resetPlayer(&player1, 230, BLUE, KEY_W, KEY_Z, KEY_A, KEY_S, KEY_D);
    resetPlayer(&player2, 730, RED, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER);

    while (WindowShouldClose()==false)
    {
        if (screen==menu)
        {
            if (IsKeyPressed(KEY_ENTER)==true)
            {
                resetRound(&player1, &player2);
                player1.health=playerHealth;
                player2.health=playerHealth;
                player1.score=0;
                player2.score=0;
                screen=playing;
            }
        }
        else if (screen==playing)
        {
            if (IsKeyPressed(KEY_ESCAPE)==true)
                screen=menu;
            else
            {
                movePlayer(&player1);
                movePlayer(&player2);

                if (IsKeyDown(player1.shoot)==true)
                    shoot(&player1);

                if (IsKeyDown(player2.shoot)==true)
                    shoot(&player2);

                updateBullets(&player1, &player2, 1);
                updateBullets(&player2, &player1, -1);

                if (player1.health <= 0)
                {
                    player2.score++;

                    if (player2.score==scoreToWin)
                    {
                        winner=2;
                        screen=gameOver;
                    }
                    else
                        resetRound(&player1, &player2);

                }
                else if (player2.health <= 0)
                {
                    player1.score++;

                    if (player1.score==scoreToWin)
                    {
                        winner=1;
                        screen=gameOver;
                    }
                    else
                        resetRound(&player1, &player2);

                }
            }
        }
        else if (screen==gameOver)
        {
            if (IsKeyPressed(KEY_ENTER)==true)
            {
                player1.score=0;
                player2.score=0;
                player1.health=playerHealth;
                player2.health=playerHealth;
                winner=0;
                screen=menu;
            }
        }

        BeginDrawing();
        ClearBackground(BEIGE);
        drawGame(player1, player2, screen, winner);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
