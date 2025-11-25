#include "camera.cpp"
#include "game.cpp"
#include "helper.cpp"
#include "raylib.h"
#include "rendering.cpp"
#include "start_screen.cpp"
#include "variables.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>
bool running = true;
int main(void) {
  // InitWindow(screenWidth, screenHeight, "Game of Life");
  // std::vector grid(ROWS, std::vector<int>(COLS));
  // int rate = 1;
  // SetTargetFPS(FPS);
  // show_start_screen(screenWidth, screenHeight);
  // double time = 0;
  // Camera2D camera;
  // init_camera(camera);
  // target = draw_grid_texture(); // LoadRenderTexture(GetScreenWidth(),
  //                               // GetScreenHeight());
  // bloom = LoadShader(0, TextFormat("./resources/bloom.fs", GLSL_VERSION));
  //// edit_mode(grid, camera);
  ////  this loop only runs the 'run' mode
  // Rectangle destRec = {0.0f, 0.0f, (float)screenWidth, (float)screenHeight};
  // Rectangle sourceRec = {0.0f, 0.0f, (float)target.texture.width,
  //                        (float)-target.texture.height};
  // while (!WindowShouldClose()) {
  //   handle_run_inputs(grid, running, rate, camera, target);
  //   update_camera(camera);
  //   time += GetFrameTime();
  //   if ((time * rate) >= 1 && running) {
  //     updMap(grid);
  //     time = 0;
  //   }
  //   render_grid(grid, camera, target);
  //   BeginDrawing();
  //   ClearBackground(BG_COLOR);
  //   DrawTexturePro(target.texture, sourceRec, destRec, {0.0f, 0.0f}, 0.0f,
  //                  WHITE);
  //   EndDrawing();
  // }
  // CloseWindow();
  // Game game;
  // game.run();
  // float screenWidth = COLS * (CELL_SIZE) + (COLS + 1) * MARGIN;
  // float screenHeight = ROWS * (CELL_SIZE) + (ROWS + 1) * MARGIN;

  // InitWindow(screenWidth, screenHeight, "GAME OF LIFE");
  // SetTargetFPS(FPS);
  // Camera2D camera;
  // init_camera(camera); // Assuming this sets zoom to 1.0f and rotation to 0
  // RenderTexture2D target =
  //     LoadRenderTexture((int)screenWidth, (int)screenHeight);
  // int mode = 0;
  // while (!WindowShouldClose()) {
  //   if (mode == 0) {
  //     if (IsKeyPressed(KEY_SPACE)) {
  //       mode = 1;
  //     }
  //   }
  //   BeginTextureMode(target);
  //   ClearBackground(BG_COLOR);
  //   DrawRectangle(screenHeight / 2, screenWidth / 2, 50, 50, GREEN);
  //   EndTextureMode();
  //   BeginDrawing();
  //   ClearBackground(BLACK); // Clear screen behind the texture
  //   Rectangle sourceRec = {0.0f, 0.0f, (float)target.texture.width,
  //                          (float)-target.texture.height};
  //   Rectangle destRec = {0.0f, 0.0f, (float)screenWidth,
  //   (float)screenHeight}; DrawTexturePro(target.texture, sourceRec, destRec,
  //   (Vector2){0, 0}, 0.0f,
  //                  WHITE);
  //   EndDrawing();
  // }
  // UnloadRenderTexture(target);
  // CloseWindow();
  Game game;
  game.run();
  return 0;
  return 0;
}
