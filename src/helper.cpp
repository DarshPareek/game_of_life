#include "raylib.h"
#include "variables.hpp"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "rendering.cpp"
#include <thread>
#include <vector>

#ifndef HELPER
#define HELPER
bool editing = false;

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
void quit(){
        CloseWindow();
        exit(0);
}

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

void update_cell(Vector2 cursor, std::vector<std::vector<int>>& grid,int state){
        int x = (cursor.x) / (CELL_SIZE + MARGIN);
        int y = (cursor.y) / (CELL_SIZE + MARGIN);
        if (x >= 0 && x < grid[0].size() && y >= 0 && y < grid.size()) {
                grid[x][y] = state;
        }
}

bool reset_random(std::vector<std::vector<int>>& grid){
        bool change = false;
        if (IsKeyPressed(KEY_R)) {
                for (auto &row : grid)
                std::fill(row.begin(), row.end(), 0);
                change = true;
        }
        if (IsKeyPressed(KEY_ENTER)) {
                spawnX(grid, RAND);
                change = true;
        }
        return change;
}
bool is_in(Vector2& current, Vector2& target, Vector2 source){
        return (current.x<=target.x) &&
                ((current.y <= std::max(target.y, source.y) && 
                current.y >= std::min(target.y, source.y)));
}
void get_cursor_pos(std::vector<std::vector<int>> &grid, int state) {
        /* A function to take grid position using mouse positon and change it to the
   * desired state */
        Vector2 now = GetMousePosition();
        Vector2 delta = GetMouseDelta();
        Vector2 prev = now;
        prev.x -= delta.x;
        prev.y -= delta.y;
        Vector2 current,source,target;
        if(prev.x <= now.x){
                source = prev;
                target = now;
        }
        else{
                source = now;
                target = prev;
        }
        current = source;
        double slope = abs(target.y - source.y) / (target.x - source.x);
        while(is_in(current,target,source))
        {
                update_cell(current,grid,state); 
                if(abs(target.x - source.x) <= abs(target.y - source.y)){
                        current.y += DDA_DELTA * ((target.y >= current.y) ? 1.0 : -1.0);
                        double delta_x = DDA_DELTA/slope;
                        current.x += delta_x * ((target.x >= current.x) ? 1.0 : -1.0);
                }
                else{
                        current.x += DDA_DELTA * ((target.x >= current. x)? 1.0 : -1.0);
                        double delta_y = slope*DDA_DELTA;
                        current.y += delta_y * ((target.y >= current.y ) ? 1.0 : -1.0);
                }
        }
}

struct grids{
std::vector<std::vector<int>>& grid;
std::vector<std::vector<int>>& pred_grid;
};
void handle_cursor(grids grids){
        while(editing){
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                        get_cursor_pos(grids.grid, 1);
                        grids.pred_grid = grids.grid;
                }
                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                        get_cursor_pos(grids.grid, 0);
                        grids.pred_grid = grids.grid;
                }
        }
}
void edit_mode(std::vector<std::vector<int>>& grid){
        SetTargetFPS(INPUT_FPS);
        editing = true;
        std::vector<std::vector<int>> pred_grid = grid;
        grids grids{grid,pred_grid};
        std::thread cursor_thread(handle_cursor, grids);
        while(!WindowShouldClose()){
                if(IsKeyPressed(QUIT)){
                        quit();
                }
                if(reset_random(grid)){
                        pred_grid = grid;
                }
                updMap(pred_grid);
                if(IsKeyPressed(KEY_D)){
                        editing = false;
                        break;
                }
                render_pred_grid(grid,pred_grid);
        }
        editing = false;
        SetTargetFPS(FPS);
        cursor_thread.join();
}

void handle_run_inputs(std::vector<std::vector<int>> &grid,
                       bool& running,int& rate) {
        if(IsKeyPressed(KEY_E)){
                edit_mode(grid);
        }
        if (IsKeyPressed(KEY_SPACE)) {
                running = !running;
        }
        if (IsKeyPressed(QUIT)) {
                quit();
        }
        reset_random(grid);
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
}

#endif
