#include "raylib.h"
#include "variables.hpp"

#ifndef START_SCREEN
#define START_SCREEN

void show_start_screen(int screenWidth, int screenHeight) {
  while (!WindowShouldClose()) {
    BeginDrawing();
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

    EndDrawing();

    if (IsKeyPressed(KEY_SPACE))
      break;
  }
}

#endif
