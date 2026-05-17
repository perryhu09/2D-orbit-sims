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
  float planetMass = 400.0f;

  float energy, kinetic, potential;

  Vector2 starPosition = {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};
  Vector2 planetPosition = {starPosition.x - r, starPosition.y};

  float v_i = std::sqrt(G * starMass / r);
  float esc = std::sqrt(2) * v_i;

  Vector2 planetVelocity = {0.0f, v_i};
  Vector2 planetAcceleration = {0.0f, 0.0f};

  float star_vi = (planetMass / starMass) * planetVelocity.y * -1;

  Vector2 starVelocity = {0.0f, star_vi};
  Vector2 starAcceleration = {0.0f, 0.0f};

  Vector2 barycenter;

  barycenter.x = (starMass * starPosition.x + planetMass * planetPosition.x) / (planetMass + starMass);
  barycenter.y = (starMass * starPosition.y + planetMass * planetPosition.y) / (planetMass + starMass);

  std::vector<Vector2> planetTrail;
  std::vector<Vector2> starTrail;
  int maxTrailLen = 1250;
  int maxStarTrailLen = 1500;

  float dy, dx, dstSq, dst;

  bool pause = 0;
  int framecounter = 0;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {

    if(IsKeyPressed(KEY_SPACE)) pause = !pause;

    float dt = 0.1f;
    int stepsPerFrame = 10;

    if(!pause) {
      for (int step = 0; step < stepsPerFrame; step++) {
        dy = starPosition.y - planetPosition.y;
        dx = starPosition.x - planetPosition.x;

        dstSq = dy * dy + dx * dx;
        dst = std::sqrt(dstSq);

        potential = -1 * G * planetMass * starMass / dst;
        energy = potential + kinetic;

        float xdir = dx / dst;
        float ydir = dy / dst;

        float p_accel = G * starMass / dstSq;
        float s_accel = G * planetMass / dstSq;
        
        planetAcceleration.x = xdir * p_accel;
        planetAcceleration.y = ydir * p_accel;

        starAcceleration.x = -1 * xdir * s_accel;
        starAcceleration.y = -1 * ydir * s_accel;

        planetVelocity.x += planetAcceleration.x * dt;
        planetVelocity.y += planetAcceleration.y * dt;

        starVelocity.x += starAcceleration.x * dt;
        starVelocity.y += starAcceleration.y * dt;

        planetPosition.x += planetVelocity.x * dt;
        planetPosition.y += planetVelocity.y * dt;

        starPosition.x += starVelocity.x * dt;
        starPosition.y += starVelocity.y * dt;
      }

      float pv_mag = std::sqrt(pow(planetVelocity.x, 2) + pow(planetVelocity.y, 2));
      float sv_mag = std::sqrt(pow(starVelocity.x, 2) + pow(starVelocity.y, 2));
      float p_KE = 0.5f * planetMass * pow(pv_mag, 2);
      float s_KE = 0.5f * starMass * pow(sv_mag, 2);

      kinetic = p_KE + s_KE;

      potential = -1 * G * planetMass * starMass / dst;
      energy = potential + kinetic;

      planetTrail.push_back(planetPosition);
      starTrail.push_back(starPosition);

      if (planetTrail.size() > maxTrailLen) {
        planetTrail.erase(planetTrail.begin());
      }

      if (starTrail.size() > maxStarTrailLen) {
        starTrail.erase(starTrail.begin());
      }
    } else {
      framecounter++;
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

      // DrawCircleV(barycenter, 1.0f, WHITE);
      
      DrawCircleV(starPosition, starRadius * 1.5f, Color{255, 220, 80, 80});
      DrawCircleV(starPosition, starRadius, YELLOW);

      DrawCircleV(planetPosition, planetRadius * 2.0f, Color{50, 100, 255, 40});
      DrawCircleV(planetPosition, planetRadius, BLUE);
      DrawCircleV({planetPosition.x - 3, planetPosition.y - 3}, planetRadius * 0.35f, Color{120, 180, 255, 180});

      DrawText(TextFormat("Energy: %.0f J", energy), 10, 30, 20, WHITE);

      DrawText("Press SPACE to pause simulation", 10, GetScreenHeight() - 25, 20, RAYWHITE);

      int textWidth = MeasureText("PAUSED", 30);
      if(pause && ((framecounter/30)%2)) DrawText("PAUSED", GetScreenWidth()/2 - textWidth/2, GetScreenHeight()/2 - 15, 30, GRAY);

      DrawFPS(10, 10);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
