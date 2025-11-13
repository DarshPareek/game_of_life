#include "raylib.h"
#include "variables.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>

#ifndef RENDER
#define RENDER
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
  DrawText("D - Run Mode", textX, textY, 16, text);
  textY += 20;
  DrawText("Run Mode Commands :", textX - heading_offset, textY, 16, text);
  textY += 20;
  DrawText("SPACE - Pause / Resume", textX, textY, 16, text);
  textY += 20;
  DrawText("E - Edit Mode", textX, textY, 16, text);
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
Rectangle square = GetShapesTextureRectangle();

void start_screen(int screenWidth = GetScreenWidth(),
                  int screenHeight = GetScreenHeight()) {
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
}
void ultimate_render(std::vector<std::vector<int>> &grid,
                     std::vector<std::vector<int>> &pred_grid,
                     Camera2D &camera) {}

#endif
