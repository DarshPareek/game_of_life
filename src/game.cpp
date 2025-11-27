#ifndef GAME
#define GAME

#include "camera.cpp"
#include "raylib.h"
#include "variables.hpp"
#include <cstdlib>
#include <vector>
float dialogAlpha = 1.0f;
double lastActivityTime = 0.0f;
bool inEditMode = true;
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

  int heading_offset = 40;
  float textX = box.x + DIALOG_PADDING + heading_offset;
  float textY = box.y + DIALOG_PADDING;
  DrawText("Controls :", textX - heading_offset, textY, 18, text);
  textY += 25;
  DrawText("Edit Mode Commands :", textX - heading_offset, textY, 18, text);
  textY += 25;
  DrawText("Mouse Left - Add Cell", textX, textY, 16, text);
  textY += 20;
  DrawText("Mouse Right - Kill Cell", textX, textY, 16, text);
  textY += 20;
  DrawText("T - Start Prediction", textX, textY, 16, text);
  textY += 20;
  DrawText("Space - Run Mode", textX, textY, 16, text);
  textY += 20;
  DrawText("Run Mode Commands :", textX - heading_offset, textY, 16, text);
  textY += 20;
  DrawText("SPACE - Edit Mode", textX, textY, 16, text);
  textY += 20;
  DrawText("Common Commands :", textX - heading_offset, textY, 16, text);
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
  void reset();
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
void Grid::reset() {
  // Reset image
  ImageClearBackground(&gridImage, DEAD);

  for (auto &row : grid)
    std::fill(row.begin(), row.end(), 0);
  UpdateTexture(gridTexture, gridImage.data);
}

bool is_in(Vector2 &current, Vector2 &target, Vector2 source) {
  return (current.x <= target.x) &&
         ((current.y <= std::max(target.y, source.y) &&
           current.y >= std::min(target.y, source.y)));
}
void update_cell(Vector2 cursor, std::vector<std::vector<int>> &grid,
                 int state) {
  int x = (cursor.x) / (CELL_SIZE + MARGIN);
  int y = (cursor.y) / (CELL_SIZE + MARGIN);
  if (x >= 0 && x < grid[0].size() && y >= 0 && y < grid.size()) {
    grid[x][y] = state;
  }
}
void get_cursor_pos(Grid &grid, int state, Camera2D &camera) {

  Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
  Vector2 delta = GetMouseDelta();

  Vector2 prevPos = {mousePos.x - delta.x / camera.zoom,
                     mousePos.y - delta.y / camera.zoom};

  int x0 = (int)(prevPos.x /
                 (CELL_SIZE)); // Assuming margins are handled in shader/logic
  int y0 = (int)(prevPos.y / (CELL_SIZE));
  int x1 = (int)(mousePos.x / (CELL_SIZE));
  int y1 = (int)(mousePos.y / (CELL_SIZE));

  int rows = grid.grid.size();
  int cols = grid.grid[0].size();

  int dx = abs(x1 - x0);
  int sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0);
  int sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;

  while (true) {
    if (x0 >= 0 && x0 < cols && y0 >= 0 && y0 < rows) {
      grid.grid[y0][x0] = state;
      grid.pred_grid[y0][x0] = state;
      Color color = (state == 1) ? LIVING : DEAD;
      Color color1 = (state == 1) ? PRED_LIVING : PRED_DEAD;
      ImageDrawPixel(&grid.gridImage, x0, y0, color);
      ImageDrawPixel(&grid.predImage, x0, y0, color1);
    }

    if (x0 == x1 && y0 == y1)
      break;

    int e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
  UpdateTexture(grid.gridTexture, grid.gridImage.data);
}
void handle_cursor(Grid &grids, int mode, Camera2D camera) {
  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !IsKeyDown(KEY_LEFT_SHIFT) &&
      !IsKeyDown(KEY_RIGHT_SHIFT)) {
    get_cursor_pos(grids, 1, camera);
    grids.pred_grid = grids.grid;
  }
  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && !IsKeyDown(KEY_LEFT_SHIFT) &&
      !IsKeyDown(KEY_RIGHT_SHIFT)) {
    get_cursor_pos(grids, 0, camera);
    grids.pred_grid = grids.grid;
  }
}

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
  Shader bloom;
  Shader def;
  Shader main_shader;
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
  gridShader =
      LoadShader(0, TextFormat("resources/grid_shader.fs", GLSL_VERSION));
  bloom = LoadShader(0, TextFormat("resources/bloom.fs", GLSL_VERSION));
  def = LoadShader(0, 0);
  gridSizeLoc = GetShaderLocation(gridShader, "gridSize");
  gapLoc = GetShaderLocation(gridShader, "gap");
  radiusLoc = GetShaderLocation(gridShader, "radius");
  int sizeLoc = GetShaderLocation(bloom, "size");
  main_shader = def;
  float gapVal = 0.1f;
  float radiusVal = 0.2f;
  float size_dims[] = {screenWidth, screenHeight};
  SetShaderValue(gridShader, gapLoc, &gapVal, SHADER_UNIFORM_FLOAT);
  SetShaderValue(gridShader, radiusLoc, &radiusVal, SHADER_UNIFORM_FLOAT);
  SetShaderValue(bloom, sizeLoc, &size_dims, SHADER_UNIFORM_VEC2);
}
void Game::run() {
  InitWindow(screenWidth, screenHeight, "GAME OF LIFE");
  init_game();
  init_camera(camera);
  while (!WindowShouldClose()) {
    update_camera(camera);
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
    BeginShaderMode(main_shader);
    DrawTextureRec(main.texture,
                   (Rectangle){0, 0, (float)main.texture.width,
                               (float)-main.texture.height},
                   (Vector2){0, 0}, WHITE);
    EndShaderMode();
    UpdateHelpDialog();
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
  if (IsKeyPressed(KEY_Q)) {
    CloseWindow();
    exit(0);
  }
  if (IsKeyPressed(KEY_S)) {
    if (main_shader.id == def.id)
      main_shader = bloom;
    else
      main_shader = def;
  }
  if (mode == 0) {
    if (IsKeyPressed(KEY_SPACE)) {
      mode = 1;
      inEditMode = true;
    }
  } else if (mode == 1) {
    handle_cursor(grids, mode, camera);
    if (IsKeyPressed(KEY_SPACE)) {
      mode = 2;
      predict = 0;
      inEditMode = false;
    }
    if (IsKeyPressed(KEY_ENTER)) {
      grids.spawnX();
    }
    if (IsKeyPressed(KEY_R)) {
      grids.reset();
      grids.pred_grid = grids.grid;
    }
    if (IsKeyPressed(KEY_T)) {
      predict = !predict;
      grids.pred_grid = grids.grid;
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
    Rectangle source = {0, 0, (float)hs.texture.width,
                        (float)-hs.texture.height};
    Rectangle dest = {0, 0, (float)screenWidth, (float)screenHeight};
    DrawTexturePro(hs.texture, source, dest, {0, 0}, 0.0f, WHITE);
  } else {
    BeginMode2D(camera);
    float gridDims[2] = {(float)COLS, (float)ROWS};
    SetShaderValue(gridShader, gridSizeLoc, gridDims, SHADER_UNIFORM_VEC2);
    BeginShaderMode(gridShader);
    grids.draw(predict);

    EndShaderMode();
  }
  EndTextureMode();
}

#endif // !GAME
