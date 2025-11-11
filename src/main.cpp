#include "helper.cpp"
#include "rendering.cpp"
#include "raylib.h"
#include "variables.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>
bool running = true;
int main(void) {
        const int screenWidth = COLS * (CELL_SIZE) + (COLS + 1) * MARGIN;
        const int screenHeight = ROWS * (CELL_SIZE) + (ROWS + 1) * MARGIN;
        InitWindow(screenWidth, screenHeight, "Game of Life");
        std::vector grid(ROWS, std::vector<int>(COLS));
        int rate = 1;
        SetTargetFPS(FPS);
        double time = 0;
        target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        bloom = LoadShader(0, TextFormat("./resources/bloom.fs", GLSL_VERSION));
        edit_mode(grid);
        // this loop only runs the 'run' mode
        while (!WindowShouldClose()) {
                handle_run_inputs(grid, running,rate);
                time += GetFrameTime();
                if ((time * rate) >= 1 && running){
                        updMap(grid);
                        time = 0;
                }
                render_grid(grid);
        }
        CloseWindow();
        return 0;
}
