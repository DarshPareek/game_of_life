#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include "helper.cpp"

#define LIVING CLITERAL(Color){203,166,247,255}
#define DEAD CLITERAL(Color){88,91,112,255}
#define BG_COLOR CLITERAL(Color){ 30, 30, 46, 255}
#define FPS 15
#define CELL_SIZE 10
#define MARGIN 3
#define ROUNDED 0.3
#define ROWS 80
#define COLS 80
#define RAND 1000

int main(void) {
        const int screenWidth = COLS*(CELL_SIZE) + (COLS+1)*MARGIN;
        const int screenHeight = ROWS*(CELL_SIZE) + (ROWS+1)*MARGIN;
        Rectangle square = GetShapesTextureRectangle();
        square.height=square.width=CELL_SIZE;
        InitWindow(screenWidth, screenHeight, "Game of Life");
        SetTargetFPS(FPS);
        std::vector grid(ROWS,std::vector<int>(COLS));

        grid[0][0]=grid[0][2]=grid[1][1]=grid[1][2]=grid[2][1]=1;
        grid[0][COLS-1]=grid[1][COLS-2]=grid[1][COLS-3]=grid[2][COLS-1]=grid[2][COLS-2]=1;
        // 001
        // 110
        // 011
        // grid[rows][cols]
        // grid[cols]
        while (!WindowShouldClose()) {
                BeginDrawing();
                ClearBackground(BG_COLOR);

                updMap(grid);
                for (int i = 0; i < ROWS; i++) {
                        for (int j = 0; j < COLS; j++) {
                                square.x = i*(CELL_SIZE) + (i+1)*MARGIN;
                                square.y = j*(CELL_SIZE) + (j+1)*MARGIN ;
                                if(grid[i][j]){
                                        DrawRectangleRounded(square,ROUNDED,CELL_SIZE,LIVING);
                                }
                                else{
                                        DrawRectangleRounded(square,ROUNDED,CELL_SIZE,DEAD);
                                }
                        }
                }
                EndDrawing();
        }
        CloseWindow();

        return 0;
}
