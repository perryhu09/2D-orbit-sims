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
  int screenWidth = 1500;
  int screenHeight = 1000;

  int gridSpacing = 50;

  std::vector<Body> bodies;

  Vector2 center = {screenWidth / 2.0f, screenHeight / 2.0f };
  float G = 1.0f;

 
  // 3-body rotating triangle
  // NEED TO HAVE ALL 4 PRESETS
  float R = 180.0f;
  float v = 4.004f;

  bodies.push_back({
    {center.x + R, center.y},
    {0.0f, v},
    {0.0f, 0.0f},
    5000.0f,
    18.0f,
    RED
  });

  bodies.push_back({
    {center.x - 0.5f * R, center.y + 0.8660254f * R},
    {-0.8660254f * v, -0.5f * v},
    {0.0f, 0.0f},
    5000.0f,
    18.0f,
    GREEN
  });

  bodies.push_back({
    {center.x - 0.5f * R, center.y - 0.8660254f * R},
    {0.8660254f * v, -0.5f * v},
    {0.0f, 0.0f},
    5000.0f,
    18.0f,
    BLUE
  });

  std::vector<std::vector<Vector2>> trails(bodies.size());
  int maxTrailLen = 100;

  int framesCounter = 0;

  InitWindow(screenWidth, screenHeight, "N-Body Simple Solar System Sim");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {

    float dt = 0.1f;
    int stepsPerFrame = 10;

    for (int step = 0; step < stepsPerFrame; step++) {
      for (Body& body : bodies) {
        body.acceleration = {0, 0};
      }

      for (int i = 0; i < bodies.size(); i++) {
        for (int j = i + 1; j < bodies.size(); ) {

          float dx = bodies[j].position.x - bodies[i].position.x;
          float dy = bodies[j].position.y - bodies[i].position.y;

          float r1 = bodies[i].radius;
          float r2 = bodies[j].radius;

          float distSq = dx * dx + dy * dy;
          float dist = std::sqrt(distSq);

          if (dist <= r1 + r2) {
            float m1 = bodies[i].mass;
            float m2 = bodies[j].mass;

            Vector2 p1 = bodies[i].position;
            Vector2 p2 = bodies[j].position;

            Vector2 v1 = bodies[i].velocity;
            Vector2 v2 = bodies[j].velocity;

            bodies[i].position = {
              (m1 * p1.x + m2 * p2.x) / (m1 + m2),
              (m1 * p1.y + m2 * p2.y) / (m1 + m2)
            };

            bodies[i].velocity= {
              (m1 * v1.x + m2 * v2.x) / (m1 + m2),
              (m1 * v1.y + m2 * v2.y) / (m1 + m2)
            };
            
            bodies[i].mass = m1 + m2;
            bodies[i].radius = std::sqrt(r1 * r1 + r2 * r2);

            bodies.erase(bodies.begin() + j);
            trails.erase(trails.begin() + j);

            trails[i].clear();

            continue;
          }

          Vector2 dir;
          dir.x = dx / dist;
          dir.y = dy / dist;

          float a_i = G * bodies[j].mass / distSq;
          float a_j = G * bodies[i].mass / distSq;

          bodies[i].acceleration.x += a_i * dir.x;
          bodies[i].acceleration.y += a_i * dir.y;

          bodies[j].acceleration.x -= a_j * dir.x;
          bodies[j].acceleration.y -= a_j * dir.y;

          j++;
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
