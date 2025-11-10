 #include "raylib.h"
#include "variables.hpp"
#include <algorithm>

constexpr float CAMERA_ZOOM_SPEED = 0.1f;
constexpr float CAMERA_MAX_ZOOM = 3.0f;
constexpr float CAMERA_PAN_SPEED = 10.0f;

static float min_zoom(float screenWidth, float screenHeight) {
  float worldWidth = COLS * (CELL_SIZE + MARGIN) + MARGIN;
  float worldHeight = ROWS * (CELL_SIZE + MARGIN) + MARGIN;

  float zoomX = screenWidth / worldWidth;
  float zoomY = screenHeight / worldHeight;
  return std::min(zoomX, zoomY);
}

static Vector2 fix_camera(const Camera2D &camera, float screenWidth,
                          float screenHeight) {
  float worldWidth = COLS * (CELL_SIZE + MARGIN) + MARGIN;
  float worldHeight = ROWS * (CELL_SIZE + MARGIN) + MARGIN;

  float halfWidth = screenWidth / 2.0f / camera.zoom;
  float halfHeight = screenHeight / 2.0f / camera.zoom;

  Vector2 target = camera.target;

  target.x = std::clamp(target.x, halfWidth, worldWidth - halfWidth);
  target.y = std::clamp(target.y, halfHeight, worldHeight - halfHeight);

  return target;
}

void init_camera(Camera2D &camera, int screenWidth, int screenHeight) {
  camera.target = {(float)screenWidth / 2.0f, (float)screenHeight / 2.0f};
  camera.offset = {(float)screenWidth / 2.0f, (float)screenHeight / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
}

void update_camera(Camera2D &camera, int screenWidth, int screenHeight) {
  float minZoom = min_zoom(screenWidth, screenHeight);

  float wheel = GetMouseWheelMove();
  if (wheel != 0.0f)
    camera.zoom += wheel * CAMERA_ZOOM_SPEED * camera.zoom;

  camera.zoom = std::clamp(camera.zoom, minZoom, CAMERA_MAX_ZOOM);

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
      (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) {

    Vector2 delta = GetMouseDelta();
    delta.x /= camera.zoom;
    delta.y /= camera.zoom;
    camera.target.x -= delta.x;
    camera.target.y -= delta.y;
  }

  camera.target = fix_camera(camera, (float)screenWidth, (float)screenHeight);
}
