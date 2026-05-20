#include "raylib.h"
#include <vector>
#include <cmath>
#include <algorithm>

struct Body {
  Vector2 position;
  Vector2 velocity;
  Vector2 acceleration;
  float mass;
  float radius;
  Color color; 
};

int screenWidth = 1500;
int screenHeight = 1000;

Vector2 center = {screenWidth / 2.0f, screenHeight / 2.0f};

std::vector<Body> bodies;
std::vector<std::vector<Vector2>> trails;


float calculateEnergy(const std::vector<Body> bodies, const float G) {
  float kinetic = 0.0f;
  float potential = 0.0f;

  for (int i = 0; i < bodies.size(); i++) {
    float v_mag = std::sqrt(pow(bodies[i].velocity.x, 2) + pow(bodies[i].velocity.y, 2));
    kinetic += 0.5f * bodies[i].mass * pow(v_mag, 2);
  }

  for (int i = 0; i < bodies.size(); i++) {
    for (int j = i + 1; j < bodies.size(); j++) {
      float dx = bodies[j].position.x - bodies[i].position.x;
      float dy = bodies[j].position.y - bodies[i].position.y;
      float dist = std::sqrt(dx * dx + dy * dy);
      potential += -G * bodies[i].mass * bodies[j].mass / dist;
    }
  }
  return kinetic + potential;
}

void resetCamera(Camera2D& camera, Vector2 center) {
  camera.target = center;
  camera.offset = {
    GetScreenWidth() / 2.0f,
    GetScreenHeight() / 2.0f
  };
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
}

int main(void) {

  int gridSpacing = 50;

  float G = 1.0f;

  float energy;
 
  int maxTrailLen = 100;

  bool pause = 0;

  float initialEnergy, energyDrift;
  bool initialEnergySet = false;

  int framesCounter = 0;

  InitWindow(screenWidth, screenHeight, "N-Body Simple Solar System Sim");

  Camera2D camera = {0};
  resetCamera(camera, center);

  SetTargetFPS(60);

  int stepsPerFrame = 10;
  float dt = 1.0f / (float)stepsPerFrame;
  const int minStepsPerFrame = 1;
  const int maxStepsPerFrame = 500;

  while (!WindowShouldClose()) {
    if(IsKeyPressed(KEY_SPACE)) pause = !pause;

    if(IsKeyPressed(KEY_R)) {
      bodies.clear();
      initialEnergySet = false;
      resetCamera(camera, center);
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 mouseDelta = GetMouseDelta();

      camera.target.x -= mouseDelta.x / camera.zoom;
      camera.target.y -= mouseDelta.y / camera.zoom;
    }

    float cameraSpeed = 500.0f * GetFrameTime() / camera.zoom;

    if (IsKeyDown(KEY_A)) camera.target.x -= cameraSpeed;
    if (IsKeyDown(KEY_D)) camera.target.x += cameraSpeed;
    if (IsKeyDown(KEY_W)) camera.target.y -= cameraSpeed;
    if (IsKeyDown(KEY_S)) camera.target.y += cameraSpeed;

    float wheel = GetMouseWheelMove();

    if (wheel != 0.0f) {
      Vector2 mousePos = GetMousePosition();
      Vector2 mouseWorldBefore = GetScreenToWorld2D(mousePos, camera);

      float sensitivity = 0.03f;
      float factor = 1.0f + sensitivity;

      if (wheel > 0.0f) {
        camera.zoom *= std::pow(factor, wheel);
      } else {
        camera.zoom /= std::pow(factor, -wheel);
      }

      camera.zoom = std::clamp(camera.zoom, 0.1f, 10.0f);

      Vector2 mouseWorldAfter = GetScreenToWorld2D(mousePos, camera);

      camera.target.x += mouseWorldBefore.x - mouseWorldAfter.x;
      camera.target.y += mouseWorldBefore.y - mouseWorldAfter.y;
    }

    if (!pause) {
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
    } else {
      framesCounter++;
    }

    energy = calculateEnergy(bodies, G);

    if(!initialEnergySet) {
      initialEnergy = energy;
      initialEnergySet = true;
    }

    energyDrift = 100.0f * (energy - initialEnergy) / std::abs(initialEnergy);

    BeginDrawing();
      ClearBackground(BLACK);

      BeginMode2D(camera);
        Color gridColor = {20, 20, 25, 255};
        for (int x = -5000; x < 5000; x += gridSpacing) {
          DrawLine(x, 0, x, GetScreenHeight(), gridColor);
        }

        for (int y = -5000; y < 5000; y += gridSpacing) {
          DrawLine(0, y, GetScreenWidth(), y, gridColor);
        }

        for (int i = 0; i < bodies.size(); i++) {
          Color color = bodies[i].color;
          for (int j = 1; j < trails[i].size(); j++) {
            float t = (float)j / trails[i].size();
            color.a = (unsigned char)(t * 180);
            DrawLineEx(trails[i][j - 1], trails[i][j], 2.0f / camera.zoom, color);
          }
          color.a = 45;
          DrawCircleV(bodies[i].position, bodies[i].radius * 1.5f, color);
          DrawCircleV(bodies[i].position, bodies[i].radius, bodies[i].color);
        }
      EndMode2D();

      int textWidth = MeasureText("PAUSED", 30);
      if(pause && ((framesCounter/30) % 2)) DrawText("PAUSED", GetScreenWidth()/2 - textWidth/2, GetScreenHeight()/2 - 15, 30, GRAY);
      DrawText("Press SPACE to pause simulation", 10, GetScreenHeight() - 23, 20, RAYWHITE);

      DrawFPS(10, 10);

      DrawText(TextFormat("Energy: %.2f J", energy), 10, 30, 20, WHITE);
      DrawText(TextFormat("Energy drift: %.4f%%", energyDrift), 10, 52, 20, WHITE);

    EndDrawing();
  }
    
  CloseWindow();

  return 0;
}
