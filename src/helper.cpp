#include "raylib.h"
#include "variables.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>

#ifndef HELPER
#define HELPER
void updMap(std::vector<std::vector<int>> &grid) {
  int rows = grid.size();
  int cols = grid[0].size();
  std::vector<std::vector<int>> killed;
  std::vector<std::vector<int>> alive;
  std::vector<std::vector<int>> neighbours = {{-1, -1}, {-1, +1}, {-1, 0},
                                              {+1, 0},  {+1, -1}, {+1, +1},
                                              {0, +1},  {0, -1}};
  for (int i = 0; i < grid.size(); i++) {
    for (int j = 0; j < grid[i].size(); j++) {
      int n = 0;
      for (int k = 0; k < neighbours.size(); k++) {
        int nx = i + neighbours[k][0];
        int ny = j + neighbours[k][1];
        if (nx < 0 || nx > rows - 1 || ny < 0 || ny > cols - 1)
          continue;
        else if (grid[nx][ny] == 1)
          n += 1;
      }
      if (grid[i][j] == 1 && n < 2)
        killed.push_back({i, j});
      else if (grid[i][j] == 1 && (n == 2 || n == 3))
        alive.push_back({i, j});
      else if (grid[i][j] == 1 && n > 3)
        killed.push_back({i, j});
      else if (grid[i][j] == 0 && n == 3)
        alive.push_back({i, j});
    }
  }
  for (int i = 0; i < killed.size(); i++)
    grid[killed[i][0]][killed[i][1]] = 0;
  for (int i = 0; i < alive.size(); i++)
    grid[alive[i][0]][alive[i][1]] = 1;
}

void spawnX(std::vector<std::vector<int>> &grid, int x) {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  int rows = grid.size();
  int cols = grid[0].size();
  for (int k = 0; k < x; k++) {
    int r = (rand() % rows);
    int c = (rand() % cols);
    grid[r][c] = 1;
  }
}

void render_grid(std::vector<std::vector<int>> &grid) {
  ClearBackground(BG_COLOR);
  Rectangle square = GetShapesTextureRectangle();
  square.height = square.width = CELL_SIZE;
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
      square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
      if (grid[i][j])
        DrawRectangleRounded(square, ROUNDED, CELL_SIZE, LIVING);
      else
        DrawRectangleRounded(square, ROUNDED, CELL_SIZE, DEAD);
    }
  }
}
#endif

#ifndef USER_INPUT
#define USER_INPUT

void UpdateHelpDialog();

bool inEditMode = false;

void change_cell(std::vector<std::vector<int>> &grid, int state,
                 Camera2D &camera) {
  Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);
  int x = mouseWorld.x / (CELL_SIZE + MARGIN);
  int y = mouseWorld.y / (CELL_SIZE + MARGIN);
  if (x >= 0 && x < grid[0].size() && y >= 0 && y < grid.size()) {
    grid[x][y] = state;
  }
}

void take_grid_pos(std::vector<std::vector<int>> &grid, Camera2D &camera,
                   int screenWidth, int screenHeight) {
  inEditMode = true;
  SetTargetFPS(INPUT_FPS);
  while (!WindowShouldClose()) {
    BeginDrawing();

    update_camera(camera, screenWidth, screenHeight);

    if (IsKeyPressed(KEY_SPACE))
      break;
    if (IsKeyPressed(QUIT)) {
      EndDrawing();
      CloseWindow();
      exit(0);
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
      change_cell(grid, 1, camera);
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
      change_cell(grid, 0, camera);

    BeginMode2D(camera);
    render_grid(grid);
    EndMode2D();

    if (!inEditMode)
      UpdateHelpDialog();

    EndDrawing();
  }
  inEditMode = false;
  SetTargetFPS(FPS);
}
#endif

float dialogAlpha = 1.0f;
double lastActivityTime = 0.0f;

void DrawHelpDialog() {
  float width = DIALOG_WIDTH;
  float height = DIALOG_HEIGHT;
  Rectangle box = {(float)GetScreenWidth() - width - DIALOG_MARGIN,
                   DIALOG_MARGIN, width, height};

  Color bg = DIALOG_BG;
  bg.a = (unsigned char)(255 * DIALOG_SEMI_TRANSPARENCY * dialogAlpha);
  Color border = Fade(DIALOG_BORDER, dialogAlpha);
  Color text = Fade(DIALOG_TEXT, dialogAlpha);

  Rectangle borderRect = {box.x - DIALOG_BORDER_THICKNESS,
                          box.y - DIALOG_BORDER_THICKNESS,
                          box.width + 2 * DIALOG_BORDER_THICKNESS,
                          box.height + 2 * DIALOG_BORDER_THICKNESS};
  DrawRectangleRounded(borderRect, DIALOG_ROUNDNESS, 10, border);
  DrawRectangleRounded(box, DIALOG_ROUNDNESS, 10, bg);

  float textX = box.x + DIALOG_PADDING;
  float textY = box.y + DIALOG_PADDING;
  DrawText("Controls:", textX, textY, 18, text);
  textY += 25;
  DrawText("Mouse Right - Add Cell", textX, textY, 16, text);
  textY += 20;
  DrawText("Mouse Left - Kill Cell", textX, textY, 16, text);
  textY += 20;
  DrawText("SPACE - Start / Pause / Resume", textX, textY, 16, text);
  textY += 20;
  DrawText("S - Edit Grid", textX, textY, 16, text);
  textY += 20;
  DrawText("R - Clear Grid", textX, textY, 16, text);
  textY += 20;
  DrawText("Up / Down - Speed +/-", textX, textY, 16, text);
  textY += 20;
  DrawText("RETURN - Spawn Random Cells", textX, textY, 16, text);
  textY += 20;
  DrawText("Mouse Wheel- Zoom +/-", textX, textY, 16, text);
  textY += 20;
  DrawText("SHIFT + Mouse Left - Pan", textX, textY, 16, text);
  textY += 20;
  DrawText("Q - Quit", textX, textY, 16, text);
}

void UpdateHelpDialog() {
  if (inEditMode)
    return;
  bool active = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_R) || IsKeyDown(KEY_Q) ||
                IsKeyDown(KEY_S) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) ||
                GetMouseDelta().x != 0 || GetMouseDelta().y ||
                IsKeyDown(KEY_ENTER) != 0;
  double now = GetTime();
  if (active)
    lastActivityTime = now;
  if (now - lastActivityTime < DIALOG_INACTIVITY_TIME)
    dialogAlpha += GetFrameTime() * DIALOG_FADE_SPEED;
  else
    dialogAlpha -= GetFrameTime() * DIALOG_FADE_SPEED;
  if (dialogAlpha < 0)
    dialogAlpha = 0;
  if (dialogAlpha > 1)
    dialogAlpha = 1;
  if (dialogAlpha > 0.01f)
    DrawHelpDialog();
}
