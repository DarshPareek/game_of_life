

#include "raylib.h"
#include <iostream>
#include <ostream>
#include <vector>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int margin = 3;
  const int sH = 960;
  const int sW = 960;
  const int screenWidth = sW + margin * 2;
  const int screenHeight = sH + margin * 2;
  const int cell_size = 10;
  const int grid_rows = sH / cell_size;
  const int grid_cols = sW / cell_size;
  InitWindow(screenWidth, screenHeight, "raylib [core] example - input keys");
  Vector2 ballPosition = {(float)screenWidth / 2, (float)screenHeight / 2};
  int sensitivity = 100;
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------
  std::vector<std::vector<int>> grid;
  // Init Grid
  std::vector<int> temp;
  for (int i = 0; i < grid_rows; i++) {
    temp.clear();
    for (int j = 0; j < grid_cols; j++) {
      temp.push_back(0);
    }
    grid.push_back(temp);
  }

  grid[5][5] = 1;
  grid[6][7] = 1;
  grid[7][4] = 1;
  grid[7][5] = 1;
  grid[7][8] = 1;
  grid[7][9] = 1;
  grid[7][10] = 1;
  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    if (IsKeyDown(KEY_RIGHT))
      ballPosition.x += GetFrameTime() * 2.0f * sensitivity;
    if (IsKeyDown(KEY_LEFT))
      ballPosition.x -= GetFrameTime() * 2.0f * sensitivity;
    if (IsKeyDown(KEY_UP))
      ballPosition.y -= GetFrameTime() * 2.0f * sensitivity;
    if (IsKeyDown(KEY_DOWN))
      ballPosition.y += GetFrameTime() * 2.0f * sensitivity;
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(DARKGRAY);

    DrawText("move the ball with arrow keys", 10, 10, 20, DARKGRAY);

    // DrawCircleV(ballPosition, 50, MAROON);
    for (int i = 0; i < grid_rows; i++) {
      for (int j = 0; j < grid_cols; j++) {
        std::cout << i * cell_size << " " << j * cell_size << std::endl;
        if (grid[i][j] == 1) {
          DrawRectangle(j * cell_size + margin, i * cell_size + margin,
                        cell_size, cell_size, DARKBLUE);
        } else {
          DrawRectangle(j * cell_size + margin, i * cell_size + margin,
                        cell_size, cell_size, GRAY);
        }
        DrawRectangleLines(j * cell_size + margin, i * cell_size + margin,
                           cell_size, cell_size, DARKGRAY);
      }
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
