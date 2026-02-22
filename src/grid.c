#include "grid.h"

#include "logger.h"
#include "raylib.h"
#include "ds/dynamic_array.h"

#define WINDOW_MARGIN 100

#define CELL_PATH_COLOR     RAYWHITE
#define CELL_WALL_COLOR     GRAY
#define CELL_START_COLOR    RED
#define CELL_GOAL_COLOR     GREEN
#define CELL_VISITED_COLOR  YELLOW
#define CELL_SOLUTION_COLOR MAGENTA

typedef struct Cell
{
    u16     x_pos;
    u16     y_pos;
    u16     width;
    u16     height;
    Color   color;

    u8 row;
    u8 col;
} Cell;

static DynamicArray g_grid = {0};

void 
gridCreate(u16 window_width, u16 window_height, u8 grid_rows, u8 grid_cols)
{
    u32 available_window_width  = window_width - 2 * WINDOW_MARGIN;
    u32 available_window_height = window_height - 2 * WINDOW_MARGIN;

    u16 cell_width    = available_window_width / grid_cols;
    u16 cell_height   = available_window_height / grid_rows;   

    g_grid = daCreate(sizeof(Cell), grid_rows * grid_cols);

    for (u64 row = 0; row < grid_rows; ++row)
    {
        for (u64 col = 0; col < grid_cols; ++col)
        {
            daPushBack(&g_grid, &(Cell){
                .x_pos  = WINDOW_MARGIN + cell_width * col,
                .y_pos  = WINDOW_MARGIN + cell_height * row,
                .width  = cell_width,
                .height = cell_height,
                .color  = WHITE,
                .row    = row,
                .col    = col
            });
        }
    }
}

void 
gridDestroy(void)
{
    daDestroy(&g_grid);
}

void 
gridUpdate(void)
{
    // Path
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        LOG_DEBUG("SHIFT + LMB: Path");

        for (u64 i = 0; i < daGetSize(&g_grid); ++i)
        {
            Cell* cell = (Cell*)daGet(&g_grid, i);

            u16 left    = cell->x_pos;
            u16 top     = cell->y_pos;
            u16 right   = cell->x_pos + cell->width;
            u16 bottom  = cell->y_pos + cell->height;

            u32 mouse_x_pos = GetMouseX();
            u32 mouse_y_pos = GetMouseY();

            if (left < mouse_x_pos && mouse_x_pos < right &&
                top  < mouse_y_pos && mouse_y_pos < bottom)
            {
                cell->color = CELL_PATH_COLOR;
            }
        }
    }
    
    // Wall
    if (!IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT) && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        LOG_DEBUG("LMB: Wall");

        for (u64 i = 0; i < daGetSize(&g_grid); ++i)
        {
            Cell* cell = (Cell*)daGet(&g_grid, i);

            u16 left    = cell->x_pos;
            u16 top     = cell->y_pos;
            u16 right   = cell->x_pos + cell->width;
            u16 bottom  = cell->y_pos + cell->height;

            u32 mouse_x_pos = GetMouseX();
            u32 mouse_y_pos = GetMouseY();

            if (left < mouse_x_pos && mouse_x_pos < right &&
                top  < mouse_y_pos && mouse_y_pos < bottom)
            {
                cell->color = CELL_WALL_COLOR;
            }
        }
    }

    // Start
    if (!IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        LOG_DEBUG("RMB: Start");

        for (u64 i = 0; i < daGetSize(&g_grid); ++i)
        {
            Cell* cell = (Cell*)daGet(&g_grid, i);

            u16 left    = cell->x_pos;
            u16 top     = cell->y_pos;
            u16 right   = cell->x_pos + cell->width;
            u16 bottom  = cell->y_pos + cell->height;

            u32 mouse_x_pos = GetMouseX();
            u32 mouse_y_pos = GetMouseY();

            if (left < mouse_x_pos && mouse_x_pos < right &&
                top  < mouse_y_pos && mouse_y_pos < bottom)
            {
                cell->color = CELL_START_COLOR;
            }
        }
    }

    // Goal
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        LOG_DEBUG("SHIFT + RMB: Goal");

        for (u64 i = 0; i < daGetSize(&g_grid); ++i)
        {
            Cell* cell = (Cell*)daGet(&g_grid, i);

            u16 left    = cell->x_pos;
            u16 top     = cell->y_pos;
            u16 right   = cell->x_pos + cell->width;
            u16 bottom  = cell->y_pos + cell->height;

            u32 mouse_x_pos = GetMouseX();
            u32 mouse_y_pos = GetMouseY();

            if (left < mouse_x_pos && mouse_x_pos < right &&
                top  < mouse_y_pos && mouse_y_pos < bottom)
            {
                cell->color = CELL_GOAL_COLOR;
            }
        }
    }
}

void 
gridDraw(void)
{
    for (u64 i = 0; i < daGetSize(&g_grid); ++i)
    {
        Cell* cell = (Cell*)daGet(&g_grid, i);
        DrawRectangle(cell->x_pos, cell->y_pos, cell->width - 1, cell->height - 1, cell->color);
    }
}