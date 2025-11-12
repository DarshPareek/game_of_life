#include "camera.cpp"
#include "helper.cpp"
#include "raylib.h"
#include "start_screen.cpp"
#include "variables.hpp"
#include <vector>

enum class GameState { Idle, Running, Paused };

int main(void) {
  const int screenWidth = COLS * (CELL_SIZE) + (COLS + 1) * MARGIN;
  const int screenHeight = ROWS * (CELL_SIZE) + (ROWS + 1) * MARGIN;

  InitWindow(screenWidth, screenHeight, "Game of Life");

  show_start_screen(screenWidth, screenHeight);

  std::vector<std::vector<int>> grid(ROWS, std::vector<int>(COLS));

  Camera2D camera;
  init_camera(camera, screenWidth, screenHeight);

  take_grid_pos(grid, camera, screenWidth, screenHeight);

  int rate = 5;
  double time = 0;
  SetTargetFPS(FPS);

  GameState state = GameState::Idle;

  while (!WindowShouldClose()) {
    BeginDrawing();

    update_camera(camera, screenWidth, screenHeight);

    if (IsKeyPressed(KEY_S)) {
      take_grid_pos(grid, camera, screenWidth, screenHeight);
      state = GameState::Idle;
    }

    if (IsKeyPressed(KEY_SPACE)) {
      if (state == GameState::Idle)
        state = GameState::Running;
      else if (state == GameState::Running)
        state = GameState::Paused;
      else if (state == GameState::Paused)
        state = GameState::Running;
    }

    if (IsKeyPressed(KEY_R)) {
      for (auto &row : grid)
        std::fill(row.begin(), row.end(), 0);
      state = GameState::Idle;
    }

    if (IsKeyPressed(KEY_DOWN))
      rate = std::max(0, rate - 1);
    if (IsKeyPressed(KEY_UP))
      rate = std::min(MAX_RATE, rate + 1);
    if (IsKeyPressed(KEY_ENTER))
      spawnX(grid, RAND);

    time += GetFrameTime();
    if (state == GameState::Running && (time * rate) >= 1) {
      updMap(grid);
      time = 0;
    }

    ClearBackground(BLACK);

    BeginMode2D(camera);
    render_grid(grid);
    EndMode2D();

    UpdateHelpDialog();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
