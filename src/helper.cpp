#include "camera.cpp"
#include "raylib.h"
#include "rendering.cpp"
#include "variables.hpp"
#include <algorithm>
#include <iostream>
#include <vector>
#ifndef HELPER
#define HELPER
bool editing = false;
bool prediction = false;
double timer = 0;
int game_state = 0; // 0 start screen, 1 Edit Mode, 2 Run Mode
std::vector grid(ROWS, std::vector<int>(COLS));
std::vector<std::vector<int>> pred_grid;
int rate = 1;
struct grid_struct {
  std::vector<std::vector<int>> &grid;
  std::vector<std::vector<int>> &pred_grid;
  Camera2D &camera;
};
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
      if (grid[i][j] == 1 && n < 2) {
        killed.push_back({i, j});
      } else if (grid[i][j] == 1 && (n == 2 || n == 3))
        alive.push_back({i, j});
      else if (grid[i][j] == 1 && n > 3) {
        killed.push_back({i, j});
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

void spawnX(int x = RAND) {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  int rows = grid.size();
  int cols = grid[0].size();
  for (int k = 0; k < x; k++) {
    int r = (rand() % rows);
    int c = (rand() % cols);
    grid[r][c] = 1;
  }
}

void render_pred_grid() {
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
  for (int i = 0; i < COLS; i++) {
    for (int j = 0; j < ROWS; j++) {
      square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
      square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
      if (pred_grid[i][j] == 1) {
        DrawRectangleRounded(square, ROUNDED, CELL_SIZE, PRED_LIVING);
      } else {
        DrawRectangleRounded(square, ROUNDED, CELL_SIZE, PRED_DEAD);
      }
    }
  }
}

void update_cell(Vector2 cursor, std::vector<std::vector<int>> &grid,
                 int state) {
  int x = (cursor.x) / (CELL_SIZE + MARGIN);
  int y = (cursor.y) / (CELL_SIZE + MARGIN);
  if (x >= 0 && x < grid[0].size() && y >= 0 && y < grid.size()) {
    grid[x][y] = state;
  }
}

bool is_in(Vector2 &current, Vector2 &target, Vector2 source) {
  return (current.x <= target.x) &&
         ((current.y <= std::max(target.y, source.y) &&
           current.y >= std::min(target.y, source.y)));
}
void get_cursor_pos(std::vector<std::vector<int>> &grid, int state,
                    Camera2D &camera) {
  /* A function to take grid position using mouse positon and change it to
  the
   * desired state */
  Vector2 now = GetScreenToWorld2D(GetMousePosition(), camera);
  Vector2 delta = GetMouseDelta();
  delta.x /= camera.zoom;
  delta.y /= camera.zoom;
  Vector2 prev = now;
  prev.x -= delta.x;
  prev.y -= delta.y;
  Vector2 current, source, target;
  if (prev.x <= now.x) {
    source = prev;
    target = now;
  } else {
    source = now;
    target = prev;
  }
  current = source;
  double slope = abs(target.y - source.y) / (target.x - source.x);
  while (is_in(current, target, source)) {
    update_cell(current, grid, state);
    if (abs(target.x - source.x) <= abs(target.y - source.y)) {
      current.y += DDA_DELTA * ((target.y >= current.y) ? 1.0 : -1.0);
      double delta_x = DDA_DELTA / slope;
      current.x += delta_x * ((target.x >= current.x) ? 1.0 : -1.0);
    } else {
      current.x += DDA_DELTA * ((target.x >= current.x) ? 1.0 : -1.0);
      double delta_y = slope * DDA_DELTA;
      current.y += delta_y * ((target.y >= current.y) ? 1.0 : -1.0);
    }
  }
}

void handle_cursor(grid_struct grids) {
  while (game_state == 1) {
    std::cout << "HERE\n";
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !IsKeyDown(KEY_LEFT_SHIFT) &&
        !IsKeyDown(KEY_RIGHT_SHIFT)) {
      get_cursor_pos(grids.grid, 1, grids.camera);
      grids.pred_grid = grids.grid;
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && !IsKeyDown(KEY_LEFT_SHIFT) &&
        !IsKeyDown(KEY_RIGHT_SHIFT) && game_state == 1) {
      get_cursor_pos(grids.grid, 0, grids.camera);
      grids.pred_grid = grids.grid;
    }
  }
}
// // void edit_mode(std::vector<std::vector<int>> &grid, Camera2D &camera) {
// //   SetTargetFPS(INPUT_FPS);

// //   while (!WindowShouldClose()) {
// //     update_camera(camera);
// //   }
// //   editing = false;
// //   SetTargetFPS(FPS);
// //   cursor_thread.join();
// // }
//
grid_struct pass_data_to_main() {
  Camera2D c;
  return grid_struct{grid, pred_grid, c};
}

void handle_inputs(Camera2D camera) {
  if (IsKeyPressed(KEY_SPACE) && game_state == 0) {
    game_state = 1;
    // handle_cursor(grids);
  } else if (IsKeyPressed(KEY_SPACE) && game_state == 1) {
    std::cout << "Pressed on 1\n";
    game_state = 2;
  } else if (IsKeyPressed(KEY_SPACE) && game_state == 2) {
    std::cout << "Pressed on 2\n";
    game_state = 1;
  }
  if (IsKeyPressed(KEY_R) && game_state == 1) {
    for (auto &row : grid)
      std::fill(row.begin(), row.end(), 0);
    pred_grid = grid;
  }
  if (IsKeyPressed(KEY_ENTER) && game_state == 1) {
    spawnX(RAND);
    pred_grid = grid;
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
  if (IsKeyPressed(KEY_T) && game_state == 1) {
    prediction = !prediction;
    pred_grid = grid;
  }
  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !IsKeyDown(KEY_LEFT_SHIFT) &&
      !IsKeyDown(KEY_RIGHT_SHIFT) && game_state == 1) {
    get_cursor_pos(grid, 1, camera);
  } else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) &&
             !IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT) &&
             game_state == 1) {
    get_cursor_pos(grid, 0, camera);
  }
}
void render_grid() {
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
}
void init_game() { game_state = 0; }
void render_game(Camera2D camera) {
  grid_struct grids{grid, pred_grid, camera};
  std::cout << game_state << "\n";
  handle_inputs(camera);
  if (game_state == 0) {
    start_screen();
  } else {
    //  allow zoom
    if (game_state == 1) {
      render_grid();
      // allow editing
      // allow prediction
    } else if (game_state == 2) {
      prediction = false;
      timer += GetFrameTime();
      if ((timer * rate) >= 1) {
        updMap(grid);
        timer = 0;
      }
      render_grid();
      // allow game to run
      // dont allow editing
      // dont allow prediction
    }
    if (prediction) {
      timer += GetFrameTime();
      if ((timer * rate) >= 1) {
        updMap(pred_grid);
        timer = 0;
      }
      render_pred_grid();
    }
  }
}

#endif
