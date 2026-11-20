#include "raylib.h"

#define screenWidth 1000
#define screenHeight 1000
#define maxBullets 10
#define playerHealth 100
#define scoreToWin 3
#define playerWidth 30
#define playerHeight 30

#define menu 0
#define playing 1
#define gameOver 2

float player1_x, player1_y, player2_x, player2_y;
int player1Health, player2Health, player1Score, player2Score, player1Up, player1Down, player1Left, player1Right, player1Shoot;
int player2Up, player2Down, player2Left, player2Right, player2Shoot, player1ShootDelay, player2ShootDelay;
Color player1Color, player2Color;

float player1BulletX[maxBullets], player2BulletX[maxBullets], player1BulletY[maxBullets], player2BulletY[maxBullets];
bool player1BulletActive[maxBullets], player2BulletActive[maxBullets];

void ResetPlayer(float *x, float *y, int *health, Color *color, int *up, int *down, int *left, int *right, int *shoot, 
    int *shootDelay, float startX, Color playerColor, int keyUp, int keyDown, int keyLeft, int keyRight, int keyShoot)
{
    *x=startX;
    *y=200;
    *health=playerHealth;
    *color=playerColor;
    *up=keyUp;
    *down=keyDown;
    *left=keyLeft;
    *right=keyRight;
    *shoot=keyShoot;
    *shootDelay=0;
}

void MovePlayer(float *x, float *y, int up, int down, int left, int right)
{
    if (IsKeyDown(up))
        *y=*y-5;

    if (IsKeyDown(down))
        *y=*y+5;

    if (IsKeyDown(left))
        *x=*x-5;

    if (IsKeyDown(right))
        *x=*x+5;

    if (*x<0)
        *x=0;

    if (*x>screenWidth-playerWidth)
        *x=screenWidth-playerWidth;

    if (*y<0)
        *y=0;

    if (*y>screenHeight-playerHeight)
        *y=screenHeight-playerHeight;
}

void Shoot(Player *player)
{
    int i;

    for (i = 0; i < MAX_BULLETS; i++)
    {
        if (player->bullets[i].active == false)
        {
            player->bullets[i].x = player->x + 15;
            player->bullets[i].y = player->y + 15;
            player->bullets[i].active = true;
            player->shootDelay = 20;
            break;
        }
    }

}

/* Moves bullets and checks whether they hit the other player. */
void UpdateBullets(Player *shooter, Player *target, int direction)
{
    int i;
    Rectangle targetBox = {target->x, target->y, 40, 40};

    for (i = 0; i < MAX_BULLETS; i++)
    {
        if (shooter->bullets[i].active)
        {
            shooter->bullets[i].x += 8 * direction;

            Rectangle bulletBox = {
                shooter->bullets[i].x,
                shooter->bullets[i].y,
                12,
                6
            };

            if (CheckCollisionRecs(bulletBox, targetBox))
            {
                target->health -= 10;
                shooter->bullets[i].active = false;
            }

            if (shooter->bullets[i].x < 0 ||
                shooter->bullets[i].x > SCREEN_WIDTH)
            {
                shooter->bullets[i].active = false;
            }
        }
    }
}

/* Draws one player and its health bar. */
void DrawPlayer(Player player)
{
    DrawRectangle((int)player.x, (int)player.y, 40, 40, player.color);
    DrawRectangle((int)player.x, (int)player.y - 12,
                  player.health * 40 / PLAYER_HEALTH, 6, GREEN);
    DrawRectangleLines((int)player.x, (int)player.y - 12,
                       40, 6, BLACK);
}

/* Draws all active bullets stored in the player's array. */
void DrawBullets(Player player)
{
    int i;

    for (i = 0; i < MAX_BULLETS; i++)
    {
        if (player.bullets[i].active)
        {
            DrawRectangle((int)player.bullets[i].x,
                          (int)player.bullets[i].y,
                          12, 6, BLACK);
        }
    }
}

/* Starts the next round without changing the scores. */
void ResetRound(Player *player1, Player *player2)
{
    int i;

    player1->x = 100;
    player1->y = 270;
    player2->x = 750;
    player2->y = 270;

    player1->health = PLAYER_HEALTH;
    player2->health = PLAYER_HEALTH;
    player1->shootDelay = 0;
    player2->shootDelay = 0;

    for (i = 0; i < MAX_BULLETS; i++)
    {
        player1->bullets[i].active = false;
        player2->bullets[i].active = false;
    }
}

/* Shows the starting menu. */
void DrawMenu(void)
{
    DrawText("ARENA DUEL", 330, 120, 40, DARKBLUE);
    DrawText("Two Player Battle", 345, 190, 24, DARKGRAY);
    DrawText("Player 1: W A S D to move, F to shoot", 220, 280, 20, BLUE);
    DrawText("Player 2: Arrow keys to move, ENTER to shoot", 190, 320, 20, RED);
    DrawText("Press ENTER to start", 330, 420, 24, BLACK);
}

/* Draws the correct screen and game information. */
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

