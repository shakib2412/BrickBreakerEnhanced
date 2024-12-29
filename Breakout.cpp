#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 6
#define COLS 5
#define NUM_BONUSES 2

int bricks[ROWS][COLS];
int bonusX = -1, bonusY = -1; // Position for the bonus
int bonusType = -1; // 0: larger paddle, 1: extra balls
bool bonusActive = false;
int fallingSpeed = 5; // Speed at which the bonus falls

void initializeBricks() {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            bricks[i][j] = 1; // 1 means brick exists
}

void drawBricks() {
    int w = 90, h = 40, gap = 5, xs, ys;
    COLORREF colors[] = {
        RGB(255, 0, 0),   // Red
        RGB(0, 255, 0),   // Green
        RGB(0, 0, 255),   // Blue
        RGB(255, 255, 0), // Yellow
        RGB(255, 165, 0), // Orange
        RGB(128, 0, 128)  // Purple
    };
    int numColors = sizeof(colors) / sizeof(colors[0]);

    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++) {
            if (bricks[i][j]) {
                setfillcolor(colors[(i + j) % numColors]); // Cycle through colors
                xs = i * w;
                ys = j * h;
                fillrectangle(gap * (i + 1) + xs, gap * (j + 1) + ys, gap * (i + 1) + xs + w, gap * (j + 1) + ys + h);
            }
        }

    // Draw the bonus if it's active
    if (bonusActive) {
        setfillcolor(RGB(255, 215, 0)); // Gold color for the bonus
        fillrectangle(bonusX, bonusY, bonusX + 30, bonusY + 30); // Draw the bonus square
    }
}

bool checkBrickCollision(int ballX, int ballY, int r) {
    int w = 90, h = 40, gap = 5;
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++) {
            if (bricks[i][j]) {
                int brickX = gap * (i + 1) + i * w;
                int brickY = gap * (j + 1) + j * h;
                if (ballX + r > brickX && ballX - r < brickX + w && ballY + r > brickY && ballY - r < brickY + h) {
                    bricks[i][j] = 0; // Clear the brick

                    // Randomly decide to drop a bonus
                    if (rand() % 5 == 0) { // 20% chance to drop a bonus
                        bonusX = brickX + w / 2 - 15; // Center the bonus above the brick
                        bonusY = brickY + h; // Position it right below the brick
                        bonusType = rand() % 2; // Randomly choose the bonus type
                        bonusActive = true;
                    }

                    return true;
                }
            }
        }
    return false;
}

bool checkPaddleCollision(int ballX, int ballY, int r, int paddleX, int paddleLength) {
    return (ballY + r >= 560 && ballY + r <= 560 + 15 && ballX + r >= paddleX && ballX - r <= paddleX + paddleLength);
}

void handleInput(int& bx, bool& gameStarted) {
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        gameStarted = true; // Game start
    }
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        bx -= 12; // Increased speed from 8 to 12
        if (bx < 0) bx = 0;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        bx += 12; // Increased speed from 8 to 12
        if (bx > 600 - 100) bx = 600 - 100; // 100 is the length of the board
    }
}

void restartGame(bool& gameOver, bool& s, int& x, int& y, bool& f, int& len) {
    gameOver = false;
    s = false;
    f = false;
    initializeBricks();
    bonusActive = false; // Reset bonus
    bonusX = -1;
    bonusY = -1;
    x = 300; // Reset ball position
    y = 560; // Reset ball position
    len = 100; // Reset paddle length
}

int main() {
    srand(time(0));
    int r = 10;
    int x = 300, y = 560; // Start the ball at the paddle's position
    int vx = 5, vy = -7;
    int bx = 250, len = 100, th = 15; // Paddle dimensions
    bool s = false, f = false, gameOver = false;
    int extraBalls = 0; // Track extra balls

    initializeBricks();

    initgraph(600, 600);
    setbkcolor(RGB(200, 200, 200));
    cleardevice();

    BeginBatchDraw();
    while (1) {
        if (!gameOver) {
            setfillcolor(RGB(0, 128, 128));
            drawBricks();
            setfillcolor(RGB(0, 0, 128));
            fillrectangle(bx, 560, bx + len, 560 + th); // Draw the board

            handleInput(bx, s);

            if (!s) { // Before game has been started
                setfillcolor(RGB(128, 0, 0));
                fillcircle(bx + len / 2, 560 - r, r);
            }
            else { // After game has been started
                if (!f) { // If it's the first time starting the game
                    x = bx + len / 2; // Set ball position on paddle
                    y = 560 - r; // Start above the paddle
                    f = true; // Ball is now in play
                }

                if (checkBrickCollision(x, y, r)) {
                    vy = -vy; // Bounce the ball
                }
                if ((x > 600 - r) || (x < r)) vx = -vx;

                x += vx; // Move the ball horizontally
                y += vy; // Move the ball vertically

                // Check if ball hits the top edge
                if (y <= 0) {
                    vy = -vy; // Reverse direction
                    y = 0; // Keep the ball within screen bounds
                }

                // Check for collision with the paddle
                if (checkPaddleCollision(x, y, r, bx, len)) {
                    vy = -vy; // Bounce the ball off the paddle
                    y = 560 - r; // Adjust ball position to be right above the paddle
                }

                // Move the bonus down
                if (bonusActive) {
                    bonusY += fallingSpeed;
                    if (bonusY >= 600) {
                        bonusActive = false; // Remove bonus if it falls out of screen
                    }
                }

                // Check if ball goes off the bottom of the screen
                if (y >= 600) {
                    gameOver = true;
                    settextcolor(RGB(0, 0, 255));
                    settextstyle(60, 0, _T("Arial"));
                    outtextxy(220, 400, _T("Game Over")); // Centered message

                    settextcolor(RGB(0, 0, 0)); // Change to desired color
                    settextstyle(30, 0, _T("Arial")); // Smaller text size
                    outtextxy(210, 480, _T("Press R to restart")); // Centered instruction

                    // Keep displaying until 'R' is pressed
                    while (gameOver) {
                        if (GetAsyncKeyState('R') & 0x8000) {
                            restartGame(gameOver, s, x, y, f, len);
                            break; // Exit the loop
                        }
                        FlushBatchDraw();
                        Sleep(100); // Delay to prevent high CPU usage
                    }
                }

                setfillcolor(RGB(128, 0, 0));
                fillcircle(x, y, r);

                // Check for bonus collection
              // Add this global variable at the top
                int activeBalls = 1; // Track the number of active balls

                // Update bonus collection logic in the main loop
                if (bonusActive && bonusY + 30 >= 560 && bonusY <= 560 + 15 && bonusX >= bx && bonusX <= bx + len) {
                    if (bonusType == 0) { // Larger paddle
                        len += 50; // Increase paddle length
                    }
                    else if (bonusType == 1) { // Extra balls
                        activeBalls += 2; // Add extra balls
                        for (int i = 0; i < 2; i++) { // Launch additional balls
                            // Adjust the starting position and velocity for new balls
                            // Here you may need to create a way to store and manage the new balls
                        }
                    }
                    bonusActive = false; // Remove the bonus after collecting
                }


                // Check for success
                bool allCleared = true;
                for (int i = 0; i < ROWS; i++)
                    for (int j = 0; j < COLS; j++)
                        if (bricks[i][j]) allCleared = false;

                if (allCleared) {
                    gameOver = true;
                    settextcolor(RGB(0, 255, 0));
                    settextstyle(60, 0, _T("Arial"));
                    outtextxy(180, 400, _T("Congratulations!")); // Centered success message

                    settextcolor(RGB(0, 0, 0)); // Change to desired color
                    settextstyle(30, 0, _T("Arial")); // Smaller text size
                    outtextxy(190, 480, _T("Press R to restart")); // Centered restart instruction

                    // Keep displaying until 'R' is pressed
                    while (gameOver) {
                        if (GetAsyncKeyState('R') & 0x8000) {
                            restartGame(gameOver, s, x, y, f, len);
                            break; // Exit the loop
                        }
                        FlushBatchDraw();
                        Sleep(100); // Delay to prevent high CPU usage
                    }
                }
            }
        }
        else {
            // Display "Lose" or "Success" and check for restart input
            if (GetAsyncKeyState('R') & 0x8000) {
                restartGame(gameOver, s, x, y, f, len);
            }
        }

        FlushBatchDraw();
        Sleep(30);
        cleardevice();
    }
    closegraph();
    return 0;
}