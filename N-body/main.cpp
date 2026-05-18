#include "raylib.h"
#include <vector>
#include <cmath>

struct Body {
  Vector2 position;
  Vector2 velocity;
  Vector2 acceleration;
  float mass;
  float radius;
  Color color;
};

int main(void) {

  const int screenWidth = 1500;
  const int screenHeight = 1000;

  int gridSpacing = 50;

  std::vector<Body> bodies;

  Vector2 center = {screenWidth / 2.0f, screenHeight / 2.0f};

  float G = 1.0f;

  float starMass = 10000.0f;

  float r1 = 250.0f;
  float r2 = 450.0f;

  float v1 = std::sqrt(G * starMass / r1);
  float v2 = std::sqrt(G * starMass / r2);

  // one star + asteroid belt
  bodies.push_back({
    center,
    {0.0f, 0.0f},
    {0.0f, 0.0f},
    12000.0f,
    28.0f,
    YELLOW
  });

  bodies.push_back({{center.x + 100.0f, center.y}, {0.0f, 10.95f}, {0.0f, 0.0f}, 1.0f, 3.0f, GRAY});
  bodies.push_back({{center.x + 92.0f, center.y + 92.0f}, {-6.80f, 6.80f}, {0.0f, 0.0f}, 1.0f, 3.0f, LIGHTGRAY});
  bodies.push_back({{center.x, center.y + 160.0f}, {-8.66f, 0.0f}, {0.0f, 0.0f}, 1.0f, 3.0f, GRAY});
  bodies.push_back({{center.x - 134.0f, center.y + 134.0f}, {-5.62f, -5.62f}, {0.0f, 0.0f}, 1.0f, 3.0f, LIGHTGRAY});
  bodies.push_back({{center.x - 220.0f, center.y}, {0.0f, -7.39f}, {0.0f, 0.0f}, 1.0f, 3.0f, GRAY});
  bodies.push_back({{center.x - 177.0f, center.y - 177.0f}, {4.90f, -4.90f}, {0.0f, 0.0f}, 1.0f, 3.0f, LIGHTGRAY});
  bodies.push_back({{center.x, center.y - 280.0f}, {6.55f, 0.0f}, {0.0f, 0.0f}, 1.0f, 3.0f, GRAY});
  bodies.push_back({{center.x + 219.0f, center.y - 219.0f}, {4.40f, 4.40f}, {0.0f, 0.0f}, 1.0f, 3.0f, LIGHTGRAY});
  bodies.push_back({{center.x + 310.0f, center.y}, {0.0f, 6.22f}, {0.0f, 0.0f}, 1.0f, 3.0f, GRAY}); 
  std::vector<std::vector<Vector2>> trails(bodies.size());
  int maxTrailLen = 100;
  
  InitWindow(screenWidth, screenHeight, "N-Body Orbit Sim");

  int framecounter = 0;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {

    // symplectic euler integration
    float dt = 0.1f;
    int stepsPerFrame = 10;

    for (int step = 0; step < stepsPerFrame; step++) {
      for (Body& body : bodies) {
        body.acceleration = {0, 0};
      }

      for (int i = 0; i < bodies.size(); i++) {
        for (int j = 0; j < bodies.size(); j++) {
          if (i == j) continue;
          float dx = bodies[j].position.x - bodies[i].position.x; 
          float dy = bodies[j].position.y - bodies[i].position.y; 

          float distSq = dx * dx + dy * dy;
          float dist = std::sqrt(distSq);

          Vector2 dir;
          dir.x = dx / dist;
          dir.y = dy / dist;

          float a = G * bodies[j].mass / distSq;

          bodies[i].acceleration.x += a * dir.x;
          bodies[i].acceleration.y += a * dir.y;
        }
      }

      for (int i = 0; i < bodies.size(); i++) {
        bodies[i].velocity.x += bodies[i].acceleration.x * dt;
        bodies[i].velocity.y += bodies[i].acceleration.y * dt;

        bodies[i].position.x += bodies[i].velocity.x * dt;
        bodies[i].position.y += bodies[i].velocity.y * dt;

      }
    }

    for (int i = 0; i < bodies.size(); i++) {
      trails[i].push_back(bodies[i].position);

      if (trails[i].size() > maxTrailLen) {
        trails[i].erase(trails[i].begin());
      }
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

      for (int i = 0; i < bodies.size(); i++) {
        Color color = bodies[i].color;
        for (int j = 1; j < trails[i].size(); j++) {
          float t = (float)j / trails[i].size();
          color.a = (unsigned char)(t * 180);
          DrawLineEx(trails[i][j - 1], trails[i][j], 2.0f, color);
        }
        color.a = 45;
        DrawCircleV(bodies[i].position, bodies[i].radius * 1.5f, color);
        DrawCircleV(bodies[i].position, bodies[i].radius, bodies[i].color);
      }

      DrawFPS(10, 10);

    EndDrawing();
  }
  
  CloseWindow();

  return 0;
}
