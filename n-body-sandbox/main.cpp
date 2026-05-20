#include "raylib.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <cctype>

struct Body {
  Vector2 position;
  Vector2 velocity;
  Vector2 acceleration;
  float mass;
  float radius;
  Color color; 
};

enum SpawnState {
  SPAWN_IDLE,
  SPAWN_RADIUS,
  SPAWN_MASS,
  SPAWN_COLOR,
  SPAWN_VX,
  SPAWN_VY
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

float distanceBetween(Vector2 a, Vector2 b) {
  float dx = a.x - b.x;
  float dy = a.y - b.y;

  return std::sqrt(dx * dx + dy * dy);
}

void updateNumberInput(std::string& text) {
  int ch = GetCharPressed();

  while (ch > 0) {
    bool isDigit = ch >= '0' && ch <= '9';
    bool isDecimal = ch == '.' && text.find('.') == std::string::npos;
    bool isMinus = ch == '-' && text.empty();

    if (isDigit || isDecimal || isMinus) {
      text += (char)ch;
    }

    ch = GetCharPressed();
  }

  if (IsKeyPressed(KEY_BACKSPACE) && !text.empty()) {
    text.pop_back();
  }
}

void updateTextInput(std::string& text) {
  int ch = GetCharPressed();

  while (ch > 0) {
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
      text += (char)std::tolower((unsigned char)ch);
    }

    ch = GetCharPressed();
  }

  if (IsKeyPressed(KEY_BACKSPACE) && !text.empty()) {
    text.pop_back();
  }
}

bool tryParseColor(const std::string& text, Color& color) {
  std::string c = text;
  if (c == "white") {
    color = WHITE;
    return true;
  }

  if (c == "yellow") {
    color = YELLOW;
    return true;
  }

  if (c == "blue") {
    color = BLUE;
    return true;
  }

  if (c == "skyblue") {
    color = SKYBLUE;
    return true;
  }

  if (c == "red") {
    color = RED;
    return true;
  }

  if (c == "green") {
    color = GREEN;
    return true;
  }

  if (c == "orange") {
    color = ORANGE;
    return true;
  }

  if (c == "purple") {
    color = PURPLE;
    return true;
  }

  if (c == "pink") {
    color = PINK;
    return true;
  }

  if (c == "gray" || c == "grey") {
    color = GRAY;
    return true;
  }

  return false;
}

int main(void) {

  int gridSpacing = 50;

  int maxTrailLen = 100;

  float G = 1.0f;

  float energy;

  bool pause = 0;

  int framesCounter = 0;

  SpawnState spawnState = SPAWN_IDLE;

  Vector2 spawnCenter = {0.0f, 0.0f};
  Vector2 spawnMouseWorld = {0.0f, 0.0f};

  Body pendingBody;
  bool spawnDragging = false;

  std::string massText = "";
  std::string colorText = "";
  std::string vxText = "";
  std::string vyText = "";

  float minSpawnRadius = 4.0f;

  InitWindow(screenWidth, screenHeight, "N-Body Simple Solar System Sim");

  Camera2D camera = {0};
  resetCamera(camera, center);

  SetTargetFPS(60);

  int stepsPerFrame = 10;
  float dt = 1.0f / (float)stepsPerFrame;
  const int minStepsPerFrame = 1;
  const int maxStepsPerFrame = 500;

  while (!WindowShouldClose()) {
    bool typingSpawnInput =
      spawnState == SPAWN_MASS ||
      spawnState == SPAWN_COLOR ||
      spawnState == SPAWN_VX ||
      spawnState == SPAWN_VY;

    if (!typingSpawnInput) {
      if(IsKeyPressed(KEY_SPACE)) pause = !pause;

      if(IsKeyPressed(KEY_R)) {
        bodies.clear();
        resetCamera(camera, center);
      }

      if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 mouseDelta = GetMouseDelta();

        camera.target.x -= mouseDelta.x / camera.zoom;
        camera.target.y -= mouseDelta.y / camera.zoom;
      }

      float cameraSpeed = 500.0f * GetFrameTime() / camera.zoom;

      if (IsKeyDown(KEY_A)) camera.target.x -= cameraSpeed;
      if (IsKeyDown(KEY_D)) camera.target.x += cameraSpeed;
      if (IsKeyDown(KEY_W)) camera.target.y -= cameraSpeed;
      if (IsKeyDown(KEY_S)) camera.target.y += cameraSpeed;
    }

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
    };

    if(IsKeyPressed(KEY_N) && spawnState == SPAWN_IDLE) {
      spawnState = SPAWN_RADIUS;
      spawnDragging = false;

      massText.clear();
      colorText.clear();
      vxText.clear();
      vyText.clear();

      pendingBody = {};
      pendingBody.color = WHITE;
    }

    if (IsKeyPressed(KEY_C)) {
      spawnState = SPAWN_IDLE;
      spawnDragging = false;

      massText.clear();
      colorText.clear();
      vxText.clear();
      vyText.clear();
    }

    spawnMouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);

    if (spawnState == SPAWN_RADIUS) {
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        spawnCenter = spawnMouseWorld;
        spawnDragging = true;
      }

      if (spawnDragging && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        float radius = distanceBetween(spawnCenter, spawnMouseWorld);

        if (radius < minSpawnRadius) {
          radius = minSpawnRadius;
        }

        pendingBody.position = spawnCenter;
        pendingBody.velocity = {0.0f, 0.0f};
        pendingBody.acceleration = {0.0f, 0.0f};;
        pendingBody.position = spawnCenter;
        pendingBody.radius = radius;
        pendingBody.mass = 0;
        pendingBody.color = WHITE;
      }

      if (spawnDragging && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        spawnDragging = false;
        spawnState = SPAWN_MASS;
      }
    }

    if (spawnState == SPAWN_MASS) {
      updateNumberInput(massText);

      if (IsKeyPressed(KEY_ENTER) && !massText.empty()) {
        pendingBody.mass = std::stof(massText);
        spawnState = SPAWN_COLOR;
      }
    }

    if (spawnState == SPAWN_COLOR) {
      updateTextInput(colorText);

      if (IsKeyPressed(KEY_ENTER) && !colorText.empty()) {
        Color chosen;
        bool good = tryParseColor(colorText, chosen);
        if(good) pendingBody.color = chosen;
        spawnState = SPAWN_VX;
      }
    }

    if (spawnState == SPAWN_VX) {
      updateNumberInput(vxText);

      if (IsKeyPressed(KEY_ENTER) && !vxText.empty()) {
        pendingBody.velocity.x = std::stof(vxText);
        spawnState = SPAWN_VY;
      }
    }

    if (spawnState == SPAWN_VY) {
      updateNumberInput(vyText);

      if (IsKeyPressed(KEY_ENTER) && !vyText.empty()) {
        pendingBody.velocity.y = std::stof(vyText);
        bodies.push_back(pendingBody);
        trails.push_back({ pendingBody.position });

        spawnState = SPAWN_IDLE;
        spawnDragging = false;

        massText.clear();
        colorText.clear();
        vxText.clear();
        vyText.clear();
      }
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

        Color previewColor = pendingBody.color;
        previewColor.a = 80;

        bool showPendingBody =
          spawnState == SPAWN_MASS ||
          spawnState == SPAWN_COLOR ||
          spawnState == SPAWN_VX ||
          spawnState == SPAWN_VY ||
          (spawnState == SPAWN_RADIUS && spawnDragging);

        if (showPendingBody) {
          DrawCircleV(pendingBody.position, pendingBody.radius, previewColor);

          DrawCircleLines(
            (int)pendingBody.position.x,
            (int)pendingBody.position.y,
            pendingBody.radius,
            pendingBody.color
          );
        }

        if (spawnState == SPAWN_RADIUS && spawnDragging) {
          DrawLineV(spawnCenter, spawnMouseWorld, WHITE);
        }
      EndMode2D();

      int textWidth = MeasureText("PAUSED", 30);
      if(pause && ((framesCounter/30) % 2)) DrawText("PAUSED", GetScreenWidth()/2 - textWidth/2, GetScreenHeight()/2 - 15, 30, GRAY);
      DrawText("Press SPACE to pause simulation", 10, GetScreenHeight() - 23, 20, RAYWHITE);

      DrawFPS(10, 10);

      DrawText(TextFormat("Energy: %.2f J", energy), 10, 45, 20, WHITE);

      int uiX = 10;
      int uiY = 80;
      int fontSize = 14;
      int lineGap = 18;

      DrawText("Controls", uiX, uiY, fontSize, RAYWHITE);
      DrawText("N - Spawn body", uiX, uiY + lineGap * 1, fontSize, GRAY);
      DrawText("C - Cancel spawn", uiX, uiY + lineGap * 2, fontSize, GRAY);
      DrawText("Right mouse - Pan camera", uiX, uiY + lineGap * 3, fontSize, GRAY);
      DrawText("Mouse wheel - Zoom", uiX, uiY + lineGap * 4, fontSize, GRAY);

      int spawnUIY = uiY + lineGap * 6;

      if (spawnState == SPAWN_RADIUS) {
        DrawText("Spawn: left click + drag to set radius",
                 uiX, spawnUIY, fontSize, ORANGE);

        if (spawnDragging) {
          DrawText(TextFormat("Radius: %.2f", pendingBody.radius),
                   uiX, spawnUIY + lineGap, fontSize, GRAY);
        }
      }

      if (spawnState == SPAWN_MASS) {
        DrawText(TextFormat("Enter mass: %s_", massText.c_str()),
                 uiX, spawnUIY, fontSize, ORANGE);
        DrawText("Press ENTER when done",
                 uiX, spawnUIY + lineGap, fontSize, GRAY);
      }

      if (spawnState == SPAWN_COLOR) {
        DrawText(TextFormat("Enter color: %s_", colorText.c_str()),
                 uiX, spawnUIY, fontSize, ORANGE);
        DrawText("Options: white yellow blue skyblue red green orange purple pink gray",
                 uiX, spawnUIY + lineGap, fontSize, GRAY);
      }

      if (spawnState == SPAWN_VX) {
        DrawText(TextFormat("Enter velocity x: %s_", vxText.c_str()),
                 uiX, spawnUIY, fontSize, ORANGE);
        DrawText("Press ENTER when done",
                 uiX, spawnUIY + lineGap, fontSize, GRAY);
      }

      if (spawnState == SPAWN_VY) {
        DrawText(TextFormat("Enter velocity y: %s_", vyText.c_str()),
                 uiX, spawnUIY, fontSize, ORANGE);
        DrawText("Press ENTER to add body",
                 uiX, spawnUIY + lineGap, fontSize, GRAY);
      }

    EndDrawing();
  }
    
  CloseWindow();

  return 0;
}
