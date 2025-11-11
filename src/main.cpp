#include "helper.cpp"
#include "raylib.h"
#include "variables.hpp"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif
int main(void) {
  const int screenWidth = COLS * (CELL_SIZE) + (COLS + 1) * MARGIN;
  const int screenHeight = ROWS * (CELL_SIZE) + (ROWS + 1) * MARGIN;
  InitWindow(screenWidth, screenHeight, "Game of Life");
  std::vector grid(ROWS, std::vector<int>(COLS));
  std::vector pred_grid(ROWS, std::vector<int>(COLS));
  int rate = 1;
  int game_state = 2; // 1 start 2 pause 0 stop
  SetTargetFPS(FPS);
  // take_grid_pos(grid, pred_grid);
  auto bloom = LoadShader(0, TextFormat("./resources/bloom.fs", GLSL_VERSION));
  RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
  double time = 0;
  while (!WindowShouldClose() && game_state) {
    BeginDrawing();
    BeginTextureMode(target);
    render_grid(grid);
    handle_all_inputs(grid, pred_grid, game_state, rate);
    if (game_state == 1) {
      time += GetFrameTime();
      if ((time * rate) >= 1) {
        updMap(grid);
        time = 0;
      }
    } else if (game_state == 2) {
      time += GetFrameTime();
      if ((time * rate) >= 1) {
        updMap(pred_grid);
        time = 0;
      }
      render_pred_grid(pred_grid);
    }
    EndTextureMode();
    BeginShaderMode(bloom);
    DrawTextureRec(target.texture,
                   (Rectangle){0, 0, (float)target.texture.width,
                               (float)-target.texture.height},
                   (Vector2){0, 0}, WHITE);
    EndShaderMode();
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
