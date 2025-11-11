#include "raylib.h"
#include "variables.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
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

        float textX = box.x + DIALOG_PADDING;
        float textY = box.y + DIALOG_PADDING;
        DrawText("Controls:", textX, textY, 18, text);
        textY += 25;
        DrawText("Mouse Left - Add Cell", textX, textY, 16, text);
        textY += 20;
        DrawText("Mouse Right - Kill Cell", textX, textY, 16, text);
        textY += 20;
        DrawText("SPACE - Pause / Resume", textX, textY, 16, text);
        textY += 20;
        DrawText("E - Edit Mode", textX, textY, 16, text);
        textY += 20;
        DrawText("D - Run Mode", textX, textY, 16, text);
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
Shader bloom;
RenderTexture2D target;
Rectangle square = GetShapesTextureRectangle();
void render_grid(std::vector<std::vector<int>> &grid) {
        square.height = square.width = CELL_SIZE;
        BeginDrawing();
        BeginTextureMode(target);
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
        EndTextureMode();
        BeginShaderMode(bloom);
        DrawTextureRec(target.texture,
                       (Rectangle){0, 0, (float)target.texture.width,
                       (float)-target.texture.height},
                       (Vector2){0, 0}, WHITE);
        EndShaderMode();
        UpdateHelpDialog();
        EndDrawing();
}

void render_pred_grid(std::vector<std::vector<int>> &grid,std::vector<std::vector<int>>& pred_grid) {
        square.height = square.width = CELL_SIZE;
        BeginDrawing();
        BeginTextureMode(target);
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
                        if (pred_grid[i][j] == 2) {
                                DrawRectangleRounded(square, ROUNDED, CELL_SIZE, PRED_DEADR);
                        } else if (pred_grid[i][j] == 1) {
                                DrawRectangleRounded(square, ROUNDED, CELL_SIZE, PRED_LIVING);
                        } else {
                                DrawRectangleRounded(square, ROUNDED, CELL_SIZE, PRED_DEAD);
                        }
                }
        }
        EndTextureMode();
        BeginShaderMode(bloom);
        DrawTextureRec(target.texture,
                       (Rectangle){0, 0, (float)target.texture.width,
                       (float)-target.texture.height},
                       (Vector2){0, 0}, WHITE);
        EndShaderMode();
        UpdateHelpDialog();
        EndDrawing();
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
