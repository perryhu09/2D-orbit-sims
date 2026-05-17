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

  float starMass = 2000;
  float planetMass = 100.0f;

  Vector2 starPosition = {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};
  Vector2 planetPosition = {starPosition.x - r, starPosition.y};

  float v_i = std::sqrt(G * starMass / r);
  float esc = std::sqrt(2) * v_i;

  Vector2 planetVelocity = {0.0f, v_i};
  Vector2 planetAcceleration = {0.0f, 0.0f};

  float star_vi = (planetMass / starMass) * planetVelocity.y * -1;

  Vector2 starVelocity = {0.0f, star_vi};
  Vector2 starAcceleration = {0.0f, 0.0f};

  std::vector<Vector2> planetTrail;
  std::vector<Vector2> starTrail;
  int maxTrailLen = 1250;
  int maxStarTrailLen = 1500;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    float dy = starPosition.y - planetPosition.y;
    float dx = starPosition.x - planetPosition.x;

    float dstSq = dy * dy + dx * dx;
    float dst = std::sqrt(dstSq);

    float xdir = dx / dst;
    float ydir = dy / dst;

    float p_accel = G * starMass / dstSq;
    float s_accel = G * planetMass / dstSq;
    
    planetAcceleration.x = xdir * p_accel;
    planetAcceleration.y = ydir * p_accel;

    starAcceleration.x = -1 * xdir * s_accel;
    starAcceleration.y = -1 * ydir * s_accel;

    planetVelocity.x += planetAcceleration.x;
    planetVelocity.y += planetAcceleration.y;

    starVelocity.x += starAcceleration.x;
    starVelocity.y += starAcceleration.y;

    planetPosition.x += planetVelocity.x;
    planetPosition.y += planetVelocity.y;

    starPosition.x += starVelocity.x;
    starPosition.y += starVelocity.y;

    planetTrail.push_back(planetPosition);
    starTrail.push_back(starPosition);

    if (planetTrail.size() > maxTrailLen) {
      planetTrail.erase(planetTrail.begin());
    }

    if (starTrail.size() > maxStarTrailLen) {
      starTrail.erase(starTrail.begin());
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

      for (int i = 1; i < planetTrail.size(); i++) {
          float t = (float)i / planetTrail.size();
          Color trailColor = {80, 160, 255, (unsigned char)(t * 255)};
          DrawLineEx(planetTrail[i - 1], planetTrail[i], 2.0f, trailColor);
      }

      for (int i = 1; i < starTrail.size(); i++) {
          float t = (float)i / starTrail.size();
          Color trailColor = {255, 180, 60, (unsigned char)(t * 180)};
          DrawLineEx(starTrail[i - 1], starTrail[i], 2.0f, trailColor);
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
