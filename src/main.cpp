#include "helper.cpp"
#include "raylib.h"
#include "variables.hpp"
#include <vector>

int main(void) {
  const int screenWidth = COLS * (CELL_SIZE) + (COLS + 1) * MARGIN;
  const int screenHeight = ROWS * (CELL_SIZE) + (ROWS + 1) * MARGIN;
  InitWindow(screenWidth, screenHeight, "Game of Life");
  std::vector grid(ROWS, std::vector<int>(COLS));
  int rate = 5;
  SetTargetFPS(FPS);
  take_grid_pos(grid);
  double time = 0;
  bool paused = true;
  bool started = false;

  while (!WindowShouldClose()) {
    BeginDrawing();

    if (IsKeyPressed(KEY_S)) {
      take_grid_pos(grid);
      paused = true;
    }

    if (IsKeyPressed(KEY_SPACE)) {
      if (!started) {
        started = true;
        paused = false;
      } else {
        paused = !paused;
      }
    }

    if (IsKeyPressed(KEY_R)) {
      for (auto &row : grid)
        std::fill(row.begin(), row.end(), 0);
      paused = true;
      started = false;
    }

    if (IsKeyPressed(QUIT)) {
      EndDrawing();
      break;
    }

    if (IsKeyPressed(KEY_DOWN)) {
      rate--;
      if (rate < 0)
        rate = 0;
    }
    if (IsKeyPressed(KEY_UP)) {
      rate++;
      if (rate > MAX_RATE)
        rate = MAX_RATE;
    }
    if (IsKeyPressed(KEY_ENTER)) {
      spawnX(grid, RAND);
    }

    time += GetFrameTime();
    if (started && !paused && (time * rate) >= 1) {
      updMap(grid);
      time = 0;
    }

    render_grid(grid);

    UpdateHelpDialog();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
