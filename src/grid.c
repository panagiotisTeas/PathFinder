#include "grid.h"

#include "logger.h"
#include "raylib.h"
#include "ds/dynamic_array.h"

#define WINDOW_MARGIN 100

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
gridDraw(void)
{
    for (u64 i = 0; i < daGetSize(&g_grid); ++i)
    {
        Cell* cell = (Cell*)daGet(&g_grid, i);
        DrawRectangle(cell->x_pos, cell->y_pos, cell->width - 1, cell->height - 1, cell->color);
    }
}