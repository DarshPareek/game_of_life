#ifndef GAME
#define GAME

#include "camera.cpp"
#include "raylib.h"
#include "variables.hpp"
#include <ctime>
#include <iostream>
#include <ostream>
#include <utility>
#include <vector>
class Grid {
public:
  Grid();
  Grid(Grid &&) = default;
  ~Grid();
  std::vector<std::vector<int>> grid;
  std::vector<std::vector<int>> pred_grid;
  std::vector<std::vector<int>> alive = {{1, 2}};
  std::vector<std::vector<int>> dead;
  std::vector<std::vector<int>> pred_alive;
  std::vector<std::vector<int>> pred_dead;
  Rectangle square;
  void update_grid();
  void update_pred_grid();
  void render_alive();
  void render_pred_alive();
  void render_dead();
  void render_pred_dead();
  void spawnX(int x = RAND);

private:
};

Grid::Grid() {
  grid.resize(ROWS, std::vector<int>(COLS));
  pred_grid.resize(ROWS, std::vector<int>(COLS));
  square.height = square.width = CELL_SIZE;
}
Grid::~Grid() {}

void Grid::update_grid() {
  alive = {};
  dead = {};
  int rows = grid.size();
  int cols = grid[0].size();
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
        dead.push_back({i, j});
      } else if (grid[i][j] == 1 && (n == 2 || n == 3))
        alive.push_back({i, j});
      else if (grid[i][j] == 1 && n > 3) {
        dead.push_back({i, j});
      } else if (grid[i][j] != 1 && n == 3)
        alive.push_back({i, j});
    }
  }
  for (int i = 0; i < dead.size(); i++) {
    int x = dead[i][0];
    int y = dead[i][1];
    grid[x][y] = 0;
  }
  for (int i = 0; i < alive.size(); i++) {
    int x = alive[i][0];
    int y = alive[i][1];
    grid[x][y] = 1;
  }
}
void Grid::update_pred_grid() {
  int rows = pred_grid.size();
  int cols = pred_grid[0].size();
  pred_dead = {};
  pred_alive = {};
  std::vector<std::vector<int>> neighbours = {{-1, -1}, {-1, +1}, {-1, 0},
                                              {+1, 0},  {+1, -1}, {+1, +1},
                                              {0, +1},  {0, -1}};
  for (int i = 0; i < pred_grid.size(); i++) {
    for (int j = 0; j < pred_grid[i].size(); j++) {
      int n = 0;
      for (int k = 0; k < neighbours.size(); k++) {
        int nx = i + neighbours[k][0];
        int ny = j + neighbours[k][1];
        if (nx < 0 || nx > rows - 1 || ny < 0 || ny > cols - 1)
          continue;
        else {
          if (pred_grid[nx][ny] == 1) {
            n += 1;
          }
        }
      }
      if (pred_grid[i][j] == 1 && n < 2) {
        pred_dead.push_back({i, j});
      } else if (pred_grid[i][j] == 1 && (n == 2 || n == 3))
        pred_alive.push_back({i, j});
      else if (pred_grid[i][j] == 1 && n > 3) {
        pred_dead.push_back({i, j});
      } else if (pred_grid[i][j] != 1 && n == 3)
        pred_alive.push_back({i, j});
    }
  }
  for (int i = 0; i < pred_dead.size(); i++) {
    int x = pred_dead[i][0];
    int y = pred_dead[i][1];
    pred_grid[x][y] = 0;
  }
  for (int i = 0; i < pred_alive.size(); i++) {
    int x = pred_alive[i][0];
    int y = pred_alive[i][1];
    pred_grid[x][y] = 1;
  }
}
void Grid::render_alive() {
  for (int k = 0; k < alive.size(); k++) {
    int i = alive[k][0];
    int j = alive[k][1];
    square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
    square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
    DrawRectangleRounded(square, ROUNDED, CELL_SIZE, LIVING);
  }
}
void Grid::render_dead() {
  for (int k = 0; k < dead.size(); k++) {
    int i = dead[k][0];
    int j = dead[k][1];
    square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
    square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
    DrawRectangleRounded(square, ROUNDED, CELL_SIZE, DEAD);
  }
}
void Grid::render_pred_alive() {
  for (int k = 0; k < pred_alive.size(); k++) {
    int i = pred_alive[k][0];
    int j = pred_alive[k][1];
    square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
    square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
    DrawRectangleRounded(square, ROUNDED, CELL_SIZE, PRED_LIVING);
  }
}
void Grid::render_pred_dead() {
  for (int k = 0; k < pred_dead.size(); k++) {
    int i = pred_dead[k][0];
    int j = pred_dead[k][1];
    square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
    square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
    DrawRectangleRounded(square, ROUNDED, CELL_SIZE, PRED_DEAD);
  }
}
void Grid::spawnX(int x) {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  int rows = grid.size();
  int cols = grid[0].size();
  for (int k = 0; k < x; k++) {
    int r = (rand() % rows);
    int c = (rand() % cols);
    grid[r][c] = 1;
    pred_grid[r][c] = 1;
    alive.push_back({r, c});
    pred_alive.push_back({r, c});
  }
}
class Game {
public:
  Camera2D camera;
  float screenWidth;
  float screenHeight;
  RenderTexture2D main;
  RenderTexture2D hs;
  RenderTexture2D main_grid;
  Rectangle sourceRec;
  Rectangle destRec;
  int mode;
  int currentShader;
  int predict = 0;
  Shader shaders[MAX_POSTPRO_SHADERS];
  Rectangle square;
  Grid grids;
  float time = 0;
  int rate = 1;
  Game();
  ~Game();
  void init_game();
  void run();
  void handle_inputs();
  void handle_drawing();
  void home_screen_texture();
  void grid_texture();

private:
};

Game::Game() {
  screenWidth = COLS * (CELL_SIZE) + (COLS + 1) * MARGIN;
  screenHeight = ROWS * (CELL_SIZE) + (ROWS + 1) * MARGIN;
  sourceRec = {0.0f, 0.0f, (float)main.texture.width,
               (float)-main.texture.height};
  destRec = {0.0f, 0.0f, (float)screenWidth, (float)screenHeight};
  mode = 0;
  shaders[MAX_POSTPRO_SHADERS] = {0};
  currentShader = 1;
  // shaders[1] =
  //     LoadShader(0, TextFormat("./../resources/bloom.fs", GLSL_VERSION));
}
Game::~Game() {}
void Game::init_game() {
  main = LoadRenderTexture(screenWidth, screenHeight);
  hs = LoadRenderTexture(screenWidth, screenHeight);
  main_grid = LoadRenderTexture(screenWidth, screenHeight);
  home_screen_texture();
  grid_texture();
}
void Game::run() {
  InitWindow(screenWidth, screenHeight, "GAME OF LIFE");
  init_game();
  SetTargetFPS(FPS);
  init_camera(camera);
  while (!WindowShouldClose()) {
    handle_inputs();
    time += GetFrameTime();
    if ((time * rate) >= 1 && mode == 1 && predict == 1) {
      grids.update_pred_grid();
      time = 0;
    } else if ((time * rate) >= 1 && mode == 2) {
      grids.update_grid();
      time = 0;
    }
    handle_drawing();
    BeginDrawing();
    DrawTextureRec(main.texture,
                   (Rectangle){0, 0, (float)main.texture.width,
                               (float)-main.texture.height},
                   (Vector2){0, 0}, WHITE);
    EndDrawing();
  }
  CloseWindow();
}
void Game::home_screen_texture() {
  BeginTextureMode(hs);
  ClearBackground(BG_COLOR);
  const char *title = "Conway's Game of Life";
  const char *startText = "Press SPACE to Start";
  const char *hintText =
      "Use arrow keys to adjust speed, R to reset, Q to quit";

  int titleWidth = MeasureText(title, 60);
  int startWidth = MeasureText(startText, 25);
  int hintWidth = MeasureText(hintText, 15);

  DrawText(title, (screenWidth - titleWidth) / 2, screenHeight / 2 - 100, 60,
           LIVING);
  DrawText(startText, (screenWidth - startWidth) / 2, screenHeight / 2, 25,
           LIVING);
  DrawText(hintText, (screenWidth - hintWidth) / 2, screenHeight / 2 + 50, 15,
           DEAD);
  EndTextureMode();
}
void Game::grid_texture() {
  BeginTextureMode(main_grid);
  ClearBackground(BG_COLOR);
  square.height = square.width = CELL_SIZE;
  for (int i = 0; i < COLS; i++) {
    for (int j = 0; j < ROWS; j++) {
      square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
      square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
      DrawRectangleRounded(square, ROUNDED, CELL_SIZE, DEAD);
    }
  }
  EndTextureMode();
}
void Game::handle_inputs() {
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
  if (mode == 0) {
    if (IsKeyPressed(KEY_SPACE)) {
      mode = 1;
    }
  } else if (mode == 1) {
    if (IsKeyPressed(KEY_SPACE)) {
      mode = 2;
      predict = 0;
    }
    if (IsKeyPressed(KEY_R)) {
      grids.spawnX();
    }
    if (IsKeyPressed(KEY_T)) {
      predict = 1;
    }
  } else {
    if (IsKeyPressed(KEY_SPACE)) {
      mode = 1;
      predict = 0;
    }
  }
}
void Game::handle_drawing() {
  BeginTextureMode(main);
  ClearBackground(BG_COLOR);
  BeginMode2D(camera);
  Rectangle source = {0, 0, (float)hs.texture.width, (float)-hs.texture.height};
  Rectangle dest = {0, 0, (float)screenWidth, (float)screenHeight};
  if (mode == 0) {
    DrawTexturePro(hs.texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
  } else {
    DrawTexturePro(main_grid.texture, source, dest, (Vector2){0, 0}, 0.0f,
                   WHITE);
    grids.render_alive();
    grids.render_dead();

    if (predict == 1) {
      grids.render_pred_alive();
      grids.render_pred_dead();
    }
  }
  EndMode2D();
  EndTextureMode();
}
#endif // !GAME
