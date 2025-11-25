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
const char *GRID_FS = R"(
#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Custom Uniforms we will send
uniform vec2 gridSize; // (Columns, Rows)
uniform float gap;     // 0.0 to 0.5 (Spacing)
uniform float radius;  // 0.0 to 0.5 (Roundness)

void main() {
    // 1. Calculate which Grid Cell this pixel belongs to
    vec2 cellCoord = floor(fragTexCoord * gridSize);
    vec2 cellUV = fract(fragTexCoord * gridSize); // 0.0 to 1.0 inside the cell

    // 2. Sample the color from the texture (Point filtering ensures we get the exact cell state)
    // We offset by 0.5 to sample the center of the texel
    vec4 texColor = texture(texture0, (cellCoord + 0.5) / gridSize);

    // If the cell is empty/dead (Alpha 0 or Black), just draw it as is
    if (texColor.a == 0.0) {
        finalColor = texColor;
        return;
    }

    // 3. SDF Logic for Rounded Box with Padding
    // Center the coordinates (-0.5 to 0.5)
    vec2 p = cellUV - 0.5;
    
    // Calculate the box size (0.5 is full cell, subtract gap)
    vec2 boxSize = vec2(0.5 - gap);
    
    // Signed Distance Field math for rounded box
    vec2 d = abs(p) - (boxSize - radius);
    float dist = length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;

    // 4. Output
    if (dist > 0.0) {
        // We are inside the gap/corner area -> Transparent
        finalColor = vec4(0.0); 
    } else {
        // We are inside the rounded shape -> Use the texture color (Living/Dead)
        finalColor = texColor;
    }
}
)";
class Grid {
public:
  Grid();
  ~Grid();

  // Logic Grids (Double Buffering)
  std::vector<std::vector<int>> grid;
  std::vector<std::vector<int>> temp_grid; // Helper for next state

  std::vector<std::vector<int>> pred_grid;
  std::vector<std::vector<int>> pred_temp_grid; // Helper for pred state

  // Rendering Assets
  Image gridImage;
  Texture2D gridTexture;

  Image predImage;
  Texture2D predTexture;

  void update_grid();
  void update_pred_grid();
  void Init();
  // Single draw function replaces all render_alive/dead
  void draw(int predictMode);

  void spawnX(int x = RAND);
  void reset_images(); // Helper to clear images
};
Grid::Grid() {
  // 1. Resize Logic Vectors
  grid.resize(ROWS, std::vector<int>(COLS, 0));
  temp_grid.resize(ROWS, std::vector<int>(COLS, 0));

  pred_grid.resize(ROWS, std::vector<int>(COLS, 0));
  pred_temp_grid.resize(ROWS, std::vector<int>(COLS, 0));

  // 2. Initialize Images (CPU RAM)
  // We create an image where 1 pixel = 1 cell.
}

Grid::~Grid() {
  UnloadImage(gridImage);
  UnloadTexture(gridTexture);
  UnloadImage(predImage);
  UnloadTexture(predTexture);
}
void Grid::Init() {
  gridImage = GenImageColor(COLS, ROWS, DEAD);
  predImage = GenImageColor(COLS, ROWS, BLANK); // BLANK so we can overlay it

  // 3. Initialize Textures (GPU VRAM)
  gridTexture = LoadTextureFromImage(gridImage);
  predTexture = LoadTextureFromImage(predImage);

  // IMPORTANT: Use Point filtering so pixels stay sharp when scaled up
  SetTextureFilter(gridTexture, TEXTURE_FILTER_POINT);
  SetTextureFilter(predTexture, TEXTURE_FILTER_POINT);
}
void Grid::update_grid() {
  int rows = grid.size();
  int cols = grid[0].size();

  // Reset temp_grid to 0 without reallocation
  for (auto &row : temp_grid)
    std::fill(row.begin(), row.end(), 0);

  // Standard Neighbor Logic
  // Optimization: We write to temp_grid, reading from grid
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      int n = 0;
      int val = grid[i][j];

      for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
          if (x == 0 && y == 0)
            continue;
          int nx = i + x;
          int ny = j + y;
          if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
            n += grid[nx][ny];
          }
        }
      }

      if (val == 1 && (n == 2 || n == 3)) {
        temp_grid[i][j] = 1;
      } else if (val == 0 && n == 3) {
        temp_grid[i][j] = 1;
      }

      Color color = (temp_grid[i][j] == 1) ? LIVING : DEAD;
      ImageDrawPixel(&gridImage, j, i, color); // Note: x=j (col), y=i (row)
    }
  }

  grid = temp_grid;

  UpdateTexture(gridTexture, gridImage.data);
}

void Grid::update_pred_grid() {
  // Similar logic for prediction
  int rows = pred_grid.size();
  int cols = pred_grid[0].size();

  for (auto &row : pred_temp_grid)
    std::fill(row.begin(), row.end(), 0);

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      int n = 0;
      int val = pred_grid[i][j];

      for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
          if (x == 0 && y == 0)
            continue;
          int nx = i + x;
          int ny = j + y;
          if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
            n += pred_grid[nx][ny];
          }
        }
      }

      if (val == 1 && (n == 2 || n == 3))
        pred_temp_grid[i][j] = 1;
      else if (val == 0 && n == 3)
        pred_temp_grid[i][j] = 1;

      Color color = (pred_temp_grid[i][j] == 1) ? PRED_LIVING : PRED_DEAD;
      ImageDrawPixel(&predImage, j, i, color); // Note: x=j (col), y=i (row)
    }
  }
  pred_grid = pred_temp_grid;
  UpdateTexture(predTexture, predImage.data);
}
// void Grid::render_alive() {
// for (int k = 0; k < alive.size(); k++) {
//   int i = alive[k][0];
//   int j = alive[k][1];
//   square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
//   square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
//   DrawRectangleRounded(square, ROUNDED, CELL_SIZE, LIVING);
// }
// }
// void Grid::render_dead() {
//   for (int k = 0; k < dead.size(); k++) {
//     int i = dead[k][0];
//     int j = dead[k][1];
//     square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
//     square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
//     DrawRectangleRounded(square, ROUNDED, CELL_SIZE, DEAD);
//   }
// }
// void Grid::render_pred_alive() {
//   for (int k = 0; k < pred_alive.size(); k++) {
//     int i = pred_alive[k][0];
//     int j = pred_alive[k][1];
//     square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
//     square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
//     DrawRectangleRounded(square, ROUNDED, CELL_SIZE, PRED_LIVING);
//   }
// }
// void Grid::render_pred_dead() {
//   for (int k = 0; k < pred_dead.size(); k++) {
//     int i = pred_dead[k][0];
//     int j = pred_dead[k][1];
//     square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
//     square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
//     DrawRectangleRounded(square, ROUNDED, CELL_SIZE, PRED_DEAD);
//   }
// }
void Grid::draw(int predictMode) {
  Rectangle source = {0, 0, (float)gridTexture.width,
                      (float)gridTexture.height};
  Rectangle dest = {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()};

  DrawTexturePro(gridTexture, source, dest, {0, 0}, 0.0f, WHITE);
  if (predictMode == 1) {
    DrawTexturePro(predTexture, source, dest, {0, 0}, 0.0f, WHITE);
  }
}
void Grid::spawnX(int x) {
  // Reset image
  ImageClearBackground(&gridImage, DEAD);

  for (auto &row : grid)
    std::fill(row.begin(), row.end(), 0);

  for (int k = 0; k < x; k++) {
    int r = GetRandomValue(0, ROWS - 1);
    int c = GetRandomValue(0, COLS - 1);
    grid[r][c] = 1;
    pred_grid[r][c] = 1;

    // Update pixel immediately
    ImageDrawPixel(&gridImage, c, r, LIVING);
  }
  UpdateTexture(gridTexture, gridImage.data);
}
// void Grid::spawnX(int x) {
//   std::srand(static_cast<unsigned int>(std::time(nullptr)));
//   int rows = grid.size();
//   int cols = grid[0].size();
//   for (int k = 0; k < x; k++) {
//     int r = (rand() % rows);
//     int c = (rand() % cols);
//     grid[r][c] = 1;
//     pred_grid[r][c] = 1;
//     alive.push_back({r, c});
//     pred_alive.push_back({r, c});
//   }
// }
class Game {
public:
  Camera2D camera;
  float screenWidth;
  float screenHeight;
  RenderTexture2D main;
  RenderTexture2D hs;
  int mode;
  int predict = 0;
  Grid grids;
  float time = 0;
  int rate = 1;
  Shader gridShader;
  int resolutionLoc;
  int gridSizeLoc;
  int gapLoc;
  int radiusLoc;
  Game();
  ~Game();
  void init_game();
  void run();
  void handle_inputs();
  void handle_drawing();
  void home_screen_texture();
};

Game::Game() {
  screenWidth = COLS * CELL_SIZE;
  screenHeight = ROWS * CELL_SIZE;
  mode = 0;
}
Game::~Game() {}
void Game::init_game() {
  main = LoadRenderTexture(screenWidth, screenHeight);
  hs = LoadRenderTexture(screenWidth, screenHeight);
  home_screen_texture();
  grids.Init();
  gridShader = LoadShaderFromMemory(0, GRID_FS);

  // Get the locations of the variables in the shader
  gridSizeLoc = GetShaderLocation(gridShader, "gridSize");
  gapLoc = GetShaderLocation(gridShader, "gap");
  radiusLoc = GetShaderLocation(gridShader, "radius");

  // Set constant values (Adjust these to taste!)
  float gapVal = 0.1f;    // Size of spacing (0.05 is nice)
  float radiusVal = 0.2f; // Roundness (0.5 is a circle, 0.0 is square)

  SetShaderValue(gridShader, gapLoc, &gapVal, SHADER_UNIFORM_FLOAT);
  SetShaderValue(gridShader, radiusLoc, &radiusVal, SHADER_UNIFORM_FLOAT);
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
// void Game::grid_texture() {
//   BeginTextureMode(main_grid);
//   ClearBackground(BG_COLOR);
//   square.height = square.width = CELL_SIZE;
//   for (int i = 0; i < COLS; i++) {
//     for (int j = 0; j < ROWS; j++) {
//       square.x = i * (CELL_SIZE) + (i + 1) * MARGIN;
//       square.y = j * (CELL_SIZE) + (j + 1) * MARGIN;
//       DrawRectangleRounded(square, ROUNDED, CELL_SIZE, DEAD);
//     }
//   }
//   EndTextureMode();
// }
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

  if (mode == 0) {
    // Draw Home Screen
    Rectangle source = {0, 0, (float)hs.texture.width,
                        (float)-hs.texture.height};
    Rectangle dest = {0, 0, (float)screenWidth, (float)screenHeight};
    DrawTexturePro(hs.texture, source, dest, {0, 0}, 0.0f, WHITE);
  } else {
    BeginMode2D(camera);
    float gridDims[2] = {(float)COLS, (float)ROWS};
    SetShaderValue(gridShader, gridSizeLoc, gridDims, SHADER_UNIFORM_VEC2);

    // --- ACTIVATE SHADER ---
    BeginShaderMode(gridShader);

    // Draw the grid (The shader will round the corners!)
    grids.draw(predict);

    EndShaderMode();
  }
  EndTextureMode();
}
#endif // !GAME
