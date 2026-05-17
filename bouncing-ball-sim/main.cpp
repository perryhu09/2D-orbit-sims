#include "raylib.h"

int main(void) {

  const int screenWidth = 850;
  const int screenHeight = 400;

  InitWindow(screenWidth, screenHeight, "Bouncing Ball Sim");

  Vector2 ballPosition = {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};
  Vector2 ballSpeed = {5.0f, 4.0f};

  int ballRadius= 20;
  float gravity = 0.2f;

  bool useGravity = true;
  bool pause = 0;
  int framesCounter = 0;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    // Update variables
    if(IsKeyPressed(KEY_G)) useGravity = !useGravity;
    if(IsKeyPressed(KEY_SPACE)) pause = !pause;

    if (!pause) {
      ballPosition.x += ballSpeed.x;
      ballPosition.y += ballSpeed.y;

      // NOT ACCURATE GRAVITY PHYSICS
      if (useGravity) ballSpeed.y += gravity;

      if ((ballPosition.x >= GetScreenWidth() - ballRadius) || (ballPosition.x <= ballRadius)) ballSpeed.x *= -1.0f;
      if ((ballPosition.y >= GetScreenHeight() - ballRadius) || (ballPosition.y <= ballRadius)) ballSpeed.y *= -0.95f;
    } else {
      framesCounter++;
    }

    //Draw
    BeginDrawing();
      
      ClearBackground(BLACK);

      DrawCircleV(ballPosition, (float)ballRadius, MAROON);
      DrawText("Press SPACE to pause ball movement", 10, GetScreenHeight() - 25, 20, RAYWHITE);

      if (useGravity) DrawText("Gravity: ON (Press G to disable)", 10, GetScreenHeight() - 50, 20, GREEN);
      else DrawText("Gravity: OFF (Press G to enable)", 10, GetScreenHeight() - 50, 20, RED);

      int textWidth = MeasureText("PAUSED", 30);
      if (pause && ((framesCounter/30)%2)) DrawText("PAUSED", GetScreenWidth()/2 - textWidth/2, GetScreenHeight()/2 - 15, 30, GRAY);

      DrawFPS(10, 10);

    EndDrawing();

  }

  CloseWindow();

  return 0;
}
