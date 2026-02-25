#ifndef PF_GRID_H
#define PF_GRID_H

#include "common.h"
#include "raylib.h"

#define CELL_PATH_COLOR     RAYWHITE
#define CELL_WALL_COLOR     GRAY
#define CELL_START_COLOR    RED
#define CELL_GOAL_COLOR     GREEN
#define CELL_VISITED_COLOR  YELLOW
#define CELL_SOLUTION_COLOR MAGENTA

typedef struct Cell
{
    // 8 bytes
    u32     distance;
    u32     weight;

    // 8 bytes
    u16     x_pos;
    u16     y_pos;
    u16     width;
    u16     height;

    // 8 bytes
    struct Cell* parent;

    // 4 bytes
    Color   color;

    // 4 bytes
    u8      is_start;
    u8      is_goal;
    u8      is_visited;
    u8      is_wall;

    // 4 bytes
    u32     heuristic;

    // 2 bytes (+ 2 bytes)
    u8      row;
    u8      col;
} Cell; // 40 bytes

void gridCreate(u16 window_width, u16 window_height, u8 grid_rows, u8 grid_cols);
void gridDestroy(void);

void gridUpdate(void);
void gridDraw(void);

void*   gridGetStart(void);
void*   gridGetGoal(void);
void*   gridGetCell(u8 row, u8 col);
u8      gridGetRows(void);
u8      gridGetCols(void);

#endif // PF_GRID_H