#include "camera.cpp"
#include "helper.cpp"
#include "raylib.h"
#include "rendering.cpp"
#include "screens.cpp"
#include "variables.hpp"
#include <thread>
int main(void) {
  InitWindow(screenWidth, screenHeight, "Game of Life");
  SetTargetFPS(FPS);
  Camera2D camera;
  init_camera(camera);
  auto grids = pass_data_to_main();
  grids.camera = camera;
  grids.grid[1][1] = 1;
  bool shader_toggle = true;
  // std::thread cursor_thread(handle_cursor, grids);
  Shader bloom =
      LoadShader(0, TextFormat("./resources/bloom.fs", GLSL_VERSION));
  Shader deft = LoadShader(
      0,
      TextFormat("./resources/bl.fs",
                 GLSL_VERSION)); // IDK how to load default shader and too lazy
  RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
  while (!WindowShouldClose()) {
    if (IsKeyPressed(QUIT)) {
      break;
    }
    if (IsKeyPressed(KEY_S)) {
      shader_toggle = !shader_toggle;
    }
    BeginTextureMode(target);
    BeginMode2D(camera);
    ClearBackground(BG_COLOR);
    update_camera(camera);
    render_game(camera);
    EndMode2D();
    EndTextureMode();
    BeginDrawing();
    if (shader_toggle) {
      BeginShaderMode(bloom);
      DrawTextureRec(target.texture,
                     (Rectangle){0, 0, (float)target.texture.width,
                                 (float)-target.texture.height},
                     (Vector2){0, 0}, WHITE);
      EndShaderMode();
    } else {
      BeginShaderMode(deft);
      DrawTextureRec(target.texture,
                     (Rectangle){0, 0, (float)target.texture.width,
                                 (float)-target.texture.height},
                     (Vector2){0, 0}, WHITE);
      EndShaderMode();
    }
    UpdateHelpDialog();
    EndDrawing();
  }
  // cursor_thread.join();
  CloseWindow();
  return 0;
}
