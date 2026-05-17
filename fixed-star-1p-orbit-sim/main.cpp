#include "raylib.h"
#include <cmath>
#include <vector>

int main(void) {
  
  const int screenWidth = 1500;
  const int screenHeight= 1000;

  int gridSpacing = 50;

  InitWindow(screenWidth, screenHeight, "Planet Orbit Sim");

  float G = 1.0;
  float r = 400;

  float starRadius = 50;
  float planetRadius = 10;

  float starMass = 3300;
  float planetMass = 0.01f;

  Vector2 starPosition = {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};
  Vector2 planetPosition = {starPosition.x - r, starPosition.y};

  float v_i = std::sqrt(G * starMass / r);
  float esc = std::sqrt(2) * v_i;

  //Vector2 planetVelocity = {0.0f, 0.0f};           // fall into star
  //Vector2 planetVelocity = {0.0f, 0.5f * v_i};     // narrow ellipse/crash
  Vector2 planetVelocity = {0.0f, v_i};              // circle
  //Vector2 planetVelocity = {0.0f, 1.2f * v_i};     // ellipse
  //Vector2 planetVelocity = {0.0f, esc};            // escape
  //Vector2 planetVelocity = {0.0f, 1.2f * esc};     // faster escape 
                                                     
  Vector2 planetAcceleration = {0.0f, 0.0f};

  std::vector<Vector2> trail;
  int maxTrailLen = 100;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    float dy = starPosition.y - planetPosition.y;
    float dx = starPosition.x - planetPosition.x;

    float dstSq = dy * dy + dx * dx;
    float dst = std::sqrt(dstSq);

    float xdir = dx / dst;
    float ydir = dy / dst;

    float accelerationMagnitude = G * starMass / dstSq;
    
    planetAcceleration.x = xdir * accelerationMagnitude;
    planetAcceleration.y = ydir * accelerationMagnitude;

    planetVelocity.x += planetAcceleration.x;
    planetVelocity.y += planetAcceleration.y;

    planetPosition.x += planetVelocity.x;
    planetPosition.y += planetVelocity.y;

    trail.push_back(planetPosition);

    if (trail.size() > maxTrailLen) {
      trail.erase(trail.begin());
    }

    BeginDrawing();
      
      ClearBackground(BLACK);

      Color gridColor = {20, 20, 25, 255};
      for (int x = 0; x < GetScreenWidth(); x += gridSpacing) {
        DrawLine(x, 0, x, GetScreenHeight(), gridColor);
      }

      for (int y = 0; y < GetScreenHeight(); y += gridSpacing) {
        DrawLine(0, y, GetScreenWidth(), y, gridColor);
      }

      for (int i = 1; i < trail.size(); i++) {
          float t = (float)i / trail.size();

          Color trailColor = {
              80,
              160,
              255,
              (unsigned char)(t * 255)
          };

          DrawLineEx(trail[i - 1], trail[i], 2.0f, trailColor);
      }

      DrawCircleV(starPosition, starRadius * 1.5f, Color{255, 220, 80, 80});
      DrawCircleV(starPosition, starRadius, YELLOW);

      DrawCircleV(planetPosition, planetRadius * 2.0f, Color{50, 100, 255, 40});
      DrawCircleV(planetPosition, planetRadius, BLUE);
      DrawCircleV({planetPosition.x - 3, planetPosition.y - 3}, planetRadius * 0.35f, Color{120, 180, 255, 180});

      DrawFPS(10, 10);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
