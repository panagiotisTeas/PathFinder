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
    // 8 bytes
    u32     distance;
    u32     weight;

    // 8 bytes
    u16     x_pos;
    u16     y_pos;
    u16     width;
    u16     height;

    // 4 bytes
    Color   color;

    // 4 bytes
    u8 is_start;
    u8 is_goal;
    u8 is_visited;
    u8 is_wall;

    // 2 bytes (+ 2 bytes)
    u8 row;
    u8 col;
} Cell; // 28 bytes

static DynamicArray g_grid  = {0};
static Cell*        g_start = NULL;
static Cell*        g_goal  = NULL;

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
                .distance   = INT32_MAX,
                .weight     = 1,

                .x_pos      = WINDOW_MARGIN + cell_width * col,
                .y_pos      = WINDOW_MARGIN + cell_height * row,
                .width      = cell_width,
                .height     = cell_height,

                .color      = CELL_PATH_COLOR,

                .is_goal    = 0,
                .is_start   = 0,
                .is_visited = 0,
                .is_wall    = 0,

                .row        = row,
                .col        = col
            });
        }
    }

    LOG_DEBUG("Size of cell: %lu bytes", sizeof(Cell));
    LOG_DEBUG("Number of cells: %lu", grid_rows * grid_cols);
    LOG_DEBUG("Total memory for grid: %lu bytes", grid_rows * grid_cols * sizeof(Cell));
}

void 
gridDestroy(void)
{
    daDestroy(&g_grid);
}

static void 
gridEdit(Color color, u8 is_start, u8 is_goal, u8 is_wall, u8 is_visited)
{
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
            cell->color         = color;
            cell->is_start      = is_start;
            cell->is_goal       = is_goal;
            cell->is_wall       = is_wall;
            cell->is_visited    = is_visited;

            if (is_goal     == 1) g_goal    = cell;
            if (is_start    == 1) g_start   = cell;
        }
    }
}

static void gridClear(void)
{
    for (u64 i = 0; i < daGetSize(&g_grid); ++i)
    {
        Cell* cell = (Cell*)daGet(&g_grid, i);

        cell->distance   = INT32_MAX;
        cell->weight     = 1;
        cell->color      = CELL_PATH_COLOR;
        cell->is_goal    = 0;
        cell->is_start   = 0;
        cell->is_visited = 0;
        cell->is_wall    = 0;
    }
}

static void gridReset(void)
{
    for (u64 i = 0; i < daGetSize(&g_grid); ++i)
    {
        Cell* cell = (Cell*)daGet(&g_grid, i);

        if (cell->is_visited == 1)
        {
            cell->distance   = INT32_MAX;
            cell->color      = CELL_PATH_COLOR;
            cell->is_goal    = 0;
            cell->is_start   = 0;
            cell->is_visited = 0;
            cell->is_wall    = 0;
        }
    }
}

void 
gridUpdate(void)
{
    // Path
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        LOG_DEBUG("SHIFT + LMB: Path");
        gridEdit(CELL_PATH_COLOR, 0, 0, 0, 0);
    }
    
    // Wall
    if (!IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT) && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        LOG_DEBUG("LMB: Wall");
        gridEdit(CELL_WALL_COLOR, 0, 0, 1, 0);
    }

    // Start
    if (!IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        LOG_DEBUG("RMB: Start");
        if (g_start != NULL) { g_start->color = CELL_PATH_COLOR; g_start->is_start = 0; }
        gridEdit(CELL_START_COLOR, 1, 0, 0, 0);
    }

    // Goal
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        LOG_DEBUG("SHIFT + RMB: Goal");
        if (g_goal != NULL) { g_goal->color = CELL_PATH_COLOR; g_start->is_goal = 0; }
        gridEdit(CELL_GOAL_COLOR, 0, 1, 0, 0);
    }

    // Clear 
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_C))
    {
        LOG_DEBUG("SHIFT + C: Clear Grid");
        gridClear();
    }

    // Reset
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_R))
    {
        LOG_DEBUG("SHIFT + R: Reset Grid");
        gridReset();
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