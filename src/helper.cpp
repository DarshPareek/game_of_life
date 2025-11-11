#include "raylib.h"
#include "variables.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#ifndef HELPER
#define HELPER
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
void change_cell(std::vector<std::vector<int>> &grid, int state) {
  /* A function to take grid position using mouse positon and change it to the
   * desired state */
  Vector2 location = GetMousePosition();
  int x = (location.x) / (CELL_SIZE + MARGIN);
  int y = (location.y) / (CELL_SIZE + MARGIN);
  if (x >= 0 && x < grid[0].size() && y >= 0 && y < grid.size()) {
    grid[x][y] = state;
  }
}
void handle_all_inputs(std::vector<std::vector<int>> &grid,
                       std::vector<std::vector<int>> &pred_grid,
                       int &game_state, int &rate) {
  if (IsKeyPressed(KEY_F)) {
    game_state = 1;
  }
  if (IsKeyPressed(KEY_P)) {
    game_state = 2;
    pred_grid = grid;
  }
  if (IsKeyPressed(QUIT)) {
    game_state = 0;
  }
  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && game_state == 2) {
    pred_grid = grid;
    change_cell(grid, 1);
    change_cell(pred_grid, 1);
  }
  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && game_state == 2) {
    pred_grid = grid;
    change_cell(grid, 0);
    change_cell(pred_grid, 0);
  }
  if (IsKeyPressed(KEY_DOWN)) {
    rate--;
    if (rate < 0) {
      rate = 0;
    }
  }
  if (IsKeyPressed(KEY_UP)) {
    rate++;
    if (rate > MAX_RATE) {
      rate = MAX_RATE;
    }
  }
  if (IsKeyPressed(KEY_R)) {
    for (auto &row : grid)
      std::fill(row.begin(), row.end(), 0);
    game_state = 2;
    pred_grid = grid;
  }
  if (IsKeyPressed(KEY_ENTER)) {
    spawnX(grid, RAND);
    pred_grid = grid;
  }
}
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
        else {
          if (grid[nx][ny] == 1) {
            n += 1;
          }
        }
      }
      // std::cout << n << "|";
      if (grid[i][j] == 1 && n < 2) {
        // grid[i][j] = 0;
        killed.push_back({i, j});
        // std::cout << "Killed ";
      } else if (grid[i][j] == 1 && (n == 2 || n == 3))
        alive.push_back({i, j});
      else if (grid[i][j] == 1 && n > 3) {
        killed.push_back({i, j});
        // std::cout << "Killed ";
      } else if (grid[i][j] != 1 && n == 3)
        alive.push_back({i, j});
    }
  }
  for (int i = 0; i < killed.size(); i++) {
    int x = killed[i][0];
    int y = killed[i][1];
    grid[x][y] = 2;
  }
  for (int i = 0; i < alive.size(); i++) {
    int x = alive[i][0];
    int y = alive[i][1];
    grid[x][y] = 1;
  }
}

void render_grid(std::vector<std::vector<int>> &grid) {
  // BeginDrawing();
  // ClearBackground(BG_COLOR);
  Rectangle square = GetShapesTextureRectangle();
  square.height = square.width = CELL_SIZE;
  for (int i = 0; i < COLS; i++) {
    for (int j = 0; j < ROWS; j++) {
      square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
      square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
      if (grid[i][j] == 1) {
        DrawRectangleRounded(square, ROUNDED, CELL_SIZE, LIVING);
      } else if (grid[i][j] == 0 || grid[i][j] == 2) {
        DrawRectangleRounded(square, ROUNDED, CELL_SIZE, DEAD);
      }
    }
  }
  // EndDrawing();
}
void render_pred_grid(std::vector<std::vector<int>> &grid) {
  // BeginDrawing();
  // ClearBackground(BG_COLOR);
  Rectangle square = GetShapesTextureRectangle();
  square.height = square.width = CELL_SIZE;
  for (int i = 0; i < COLS; i++) {
    for (int j = 0; j < ROWS; j++) {
      square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
      square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
      if (grid[i][j] == 2) {
        DrawRectangleRounded(square, ROUNDED, CELL_SIZE, PRED_DEADR);
      } else if (grid[i][j] == 1) {
        DrawRectangleRounded(square, ROUNDED, CELL_SIZE, PRED_LIVING);
      } else {
        DrawRectangleRounded(square, ROUNDED, CELL_SIZE, PRED_DEAD);
      }
    }
  }
  // EndDrawing();
}

void print_grid(std::vector<std::vector<int>> &grid) {
  for (std::vector<int> &row : grid) {
    // Fill the entire row (from its beginning to its end) with the value 0
    // std::fill(row.begin(), row.end(), 0);
    for (int &i : row) {
      std::cout << i << " ";
    }
    std::cout << std::endl;
  }
}
#endif

// #ifndef USER_INPUT
// #define USER_INPUT
float dialogAlpha = 1.0f;
double lastActivityTime = 0.0f;
bool inEditMode = false;
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
// void take_grid_pos(std::vector<std::vector<int>> &grid,
//                    std::vector<std::vector<int>> &pred_grid) {
//   /* A function to take handles mouse input and cell state  */
//   SetTargetFPS(INPUT_FPS);
//   while (!WindowShouldClose()) {

//     pred_grid = grid;
//     // render_grid(grid);
//     render_grid(pred_grid);
//     // EndDrawing();
//   }
//   SetTargetFPS(FPS);
// }
// #endif
