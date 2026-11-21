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

void ResetPlayer(Player *player, float x, Color color, int up, int down, int left, int right, int shoot)
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

void MovePlayer(Player *player)
{
    if (IsKeyDown((*player).up))
        (*player).y=(*player).y-5;

    if (IsKeyDown((*player).down))
        (*player).y=(*player).y+5;

    if (IsKeyDown((*player).left))
        (*player).x=(*player).x-5;

    if (IsKeyDown((*player).right))
        (*player).x=(*player).x-5;

    if ((*player).x<0)
        (*player).x=0;

    if ((*player).x > screenWidth-40)
        (*player).x = screenWidth-40;

    if ((*player).y<60)
        (*player).y=60;

    if ((*player).y > screenHeight-40)
        (*player).y = screenHeight-40;
}

void Shoot(Player *player)
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

void UpdateBullets(Player *shooter, Player *target, int direction)
{
    int i;

    for (i=0; i<bulletsMax; i++)
    {
        if ((*shooter).bullets[i].inUse)
        {
            (*shooter).bullets[i].x=((*shooter).bullets[i].x + 5)*direction;

            if ((*shooter).bullets[i].x < (*target).x + 40 && (*shooter).bullets[i].x + 12 > (*target).x &&
                (*shooter).bullets[i].y < (*target).y + 40 && (*shooter).bullets[i].y + 6 > (*target).y)
            {
                (*target).health= (*target).health-10;
                (*shooter).bullets[i].inUse=false;
            }
        }
    }
}

void DrawPlayer(Player player)
{
    DrawRectangle((int)player.x, (int)player.y, 40, 40, player.color);
    DrawRectangle((int)player.x, (int)player.y-10, player.health*40/playerHealth, 6, GREEN);
}

void DrawBullets(Player player)
{
    int i;

    for (i=0; i<bulletsMax; i++)
    {
        if (player.bullets[i].inUse==true)
            DrawRectangle((int)player.bullets[i].x, (int)player.bullets[i].y, 12, 6, BLACK);
    }
}

void ResetRound(Player *player1, Player *player2)
{
    int i;

    (*player1).x=230;
    (*player1).y=250;
    (*player2).x=230;
    (*player2).y=250;

    (*player1).health=playerHealth;
    (*player2).health=playerHealth;

    for (i=0; i<bulletsMax; i++)
    {
        (*player1).bullets[i].inUse=false;
        (*player2).bullets[i].inUse=false;
    }
}

void DrawMenu()
{
    DrawText("BATTLE ARENA", 330, 120, 40, DARKBLUE);
    DrawText("Two Player Battle", 345, 190, 24, DARKGRAY);
    DrawText("Player 1: W A Z S to move, D to shoot", 220, 280, 20, BLUE);
    DrawText("Player 2: Arrow keys to move, ENTER to shoot", 190, 320, 20, RED);
    DrawText("Press ENTER to start", 330, 420, 24, BLACK);
}
void DrawGame(Player player1, Player player2, int screen, int winner)
{
    if (screen == menu)
    {
        DrawMenu();
    }
    else if (screen == PLAYING)
    {
        DrawText("ARENA DUEL", 370, 15, 25, DARKGRAY);
        DrawText(TextFormat("Player 1: %d", player1.score), 40, 20, 20, BLUE);
        DrawText(TextFormat("Player 2: %d", player2.score), 740, 20, 20, RED);
        DrawText("ESC: Back to menu", 350, 570, 18, GRAY);

        DrawPlayer(player1);
        DrawPlayer(player2);
        DrawBullets(player1);
        DrawBullets(player2);
    }
    else if (screen == gameOver)
    {
        if (winner == 1)
            DrawText("PLAYER 1 WINS!", 300, 200, 40, BLUE);
        else
            DrawText("PLAYER 2 WINS!", 300, 200, 40, RED);

        DrawText("Press ENTER to return to menu", 235, 300, 24, DARKGRAY);
    }
}


int main()
{
    Player player1;
    Player player2;
    int screen=menu;
    int winner=0;

    window(screenWidth, screenHeight, "BATTLE ARENA");
    FPS(60);

    ResetPlayer(&player1, 150, PINK, KEY_W, KEY_Z, KEY_A, KEY_S, KEY_D);
    ResetPlayer(&player2, 850, GREEN, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER);

    bool runGame=true;

    while (runGame==true)
    {
        if (screen==menu)
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                ResetRound(&player1, &player2);
                player1.remainingHealth=playerHealth;
                player2.remainingHealth=playerHealth;
                player1.score=0;
                player2.score=0;
                screen=playing;
            }
        }
        else if (screen==playing)
        {
            if (IsKeyPressed(KEY_ESCAPE))
            {
                screen=menu;
            }
            else
            {
                MovePlayer(&player1);
                MovePlayer(&player2);

                if (player1.shootDelay > 0)
                    player1.shootDelay--;

                if (player2.shootDelay > 0)
                    player2.shootDelay--;

                if (IsKeyDown(player1.shoot) &&
                    player1.shootDelay == 0)
                {
                    Shoot(&player1);
                }

                if (IsKeyDown(player2.shoot) &&
                    player2.shootDelay == 0)
                {
                    Shoot(&player2);
                }

                UpdateBullets(&player1, &player2, 1);
                UpdateBullets(&player2, &player1, -1);

                if (player1.remainingHealth <= 0)
                {
                    player2.score++;

                    if (player2.score == scoreToWin)
                    {
                        winner = 2;
                        screen = gameOver;
                    }
                    else
                    {
                        ResetRound(&player1, &player2);
                    }
                }

                if (player2.remainingHealth <= 0)
                {
                    player1.score++;

                    if (player1.score == scoreToWin)
                    {
                        winner = 1;
                        screen = gameOver;
                    }
                    else
                    {
                        ResetRound(&player1, &player2);
                    }
                }
            }
        }
        else if (screen == gameOver)
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                player1.score = 0;
                player2.score = 0;
                player1.remainingHealth = playerHealth;
                player2.remainingHealth = playerHealth;
                winner = 0;
                screen = menu;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawGame(player1, player2, screen, winner);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

