#include "grid.h"

#include "logger.h"
#include "ds/dynamic_array.h"

#include "bfs.h"
#include "dfs.h"
#include "dijkstra.h"
#include "a_star.h"

#define WINDOW_MARGIN 100

static DynamicArray g_grid  = {0};
static Cell*        g_start = NULL;
static Cell*        g_goal  = NULL;

static u8           g_grid_rows = 0;
static u8           g_grid_cols = 0;

typedef enum {
    ALGO_NONE     = 0,
    ALGO_BFS      = 1,
    ALGO_DFS      = 2,
    ALGO_DIJKSTRA = 3,
    ALGO_ASTAR    = 4
} ActiveAlgo;

static ActiveAlgo g_active_algo = ALGO_NONE;

void 
gridCreate(u16 window_width, u16 window_height, u8 grid_rows, u8 grid_cols)
{
    g_grid_rows = grid_rows;
    g_grid_cols = grid_cols;

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

                .parent     = NULL,

                .color      = CELL_PATH_COLOR,

                .is_goal    = 0,
                .is_start   = 0,
                .is_visited = 0,
                .is_wall    = 0,

                .heuristic  = 0,

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

static void 
gridWeight(i64 weight)
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
            if (cell->weight == 1 && weight < 0) return;

            cell->weight = cell->weight + weight;
            
            LOG_DEBUG("Row: %d | Col: %d | Weight: %d", cell->row, cell->col, cell->weight);
        }
    }
}

static void gridClear(void)
{
    g_start = NULL;
    g_goal  = NULL;

    for (u64 i = 0; i < daGetSize(&g_grid); ++i)
    {
        Cell* cell = (Cell*)daGet(&g_grid, i);

        cell->distance   = INT32_MAX;
        cell->weight     = 1;
        cell->parent     = NULL;
        cell->color      = CELL_PATH_COLOR;
        cell->is_goal    = 0;
        cell->is_start   = 0;
        cell->is_visited = 0;
        cell->is_wall    = 0;
        cell->heuristic  = 0;
    }
}

static void gridReset(void)
{
    g_start->is_visited = 0;
    g_start->color      = CELL_START_COLOR;
    g_start->distance   = INT32_MAX;
    g_start->heuristic  = 0;
    g_start->parent     = NULL;
    g_goal->is_visited  = 0;
    g_goal->color       = CELL_GOAL_COLOR;
    g_goal->distance    = INT32_MAX;
    g_goal->heuristic   = 0;
    g_goal->parent      = NULL;

    for (u64 i = 0; i < daGetSize(&g_grid); ++i)
    {
        Cell* cell = (Cell*)daGet(&g_grid, i);

        if (cell->is_visited == 1)
        {
            cell->distance   = INT32_MAX;
            cell->parent     = NULL;
            cell->color      = CELL_PATH_COLOR;
            cell->is_visited = 0;
            cell->heuristic  = 0;
        }
    }
}

void 
gridUpdate(void)
{
    // Increase weight
    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_EQUAL))
    {
        LOG_DEBUG("SHIFT + =: Increase Weight");
        gridWeight(1);
        return;
    }

    // Decrease weight
    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_MINUS))
    {
        LOG_DEBUG("SHIFT + -: Decrease Weight");
        gridWeight(-1);
        return;
    }

    // Path
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        LOG_DEBUG("SHIFT + LMB: Path");
        gridEdit(CELL_PATH_COLOR, 0, 0, 0, 0);
        return;
    }
    
    // Wall
    if (!IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT) && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        LOG_DEBUG("LMB: Wall");
        gridEdit(CELL_WALL_COLOR, 0, 0, 1, 0);
        return;
    }

    // Start
    if (!IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        LOG_DEBUG("RMB: Start");
        if (g_start != NULL) { g_start->color = CELL_PATH_COLOR; g_start->is_start = 0; }
        gridEdit(CELL_START_COLOR, 1, 0, 0, 0);
        return;
    }

    // Goal
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        LOG_DEBUG("SHIFT + RMB: Goal");
        if (g_goal != NULL) { g_goal->color = CELL_PATH_COLOR; g_start->is_goal = 0; }
        gridEdit(CELL_GOAL_COLOR, 0, 1, 0, 0);
        return;
    }

    // Clear 
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_C))
    {
        LOG_DEBUG("SHIFT + C: Clear Grid");
        gridClear();
        return;
    }

    // Reset
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_R))
    {
        LOG_DEBUG("SHIFT + R: Reset Grid");
        gridReset();
        return;
    }

    // Breadth First Search
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_ONE) && g_start != NULL && g_goal != NULL)
    {
        LOG_DEBUG("SHIFT + 1: Breadth First Search");
        gridReset();
        g_active_algo = ALGO_BFS;
        bfsInit(daGetSize(&g_grid));       
    }

    if (g_active_algo == ALGO_BFS && !bfsShouldStop()) bfsStep();

    // Depth First Search
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_TWO) && g_start != NULL && g_goal != NULL)
    {
        LOG_DEBUG("SHIFT + 2: Depth First Search");
        gridReset();
        g_active_algo = ALGO_DFS;
        dfsInit(daGetSize(&g_grid));       
    }

    if (g_active_algo == ALGO_DFS && !dfsShouldStop()) dfsStep();

    // Dijkstra
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_THREE) && g_start != NULL && g_goal != NULL)
    {
        LOG_DEBUG("SHIFT + 3: Dijkstra");
        gridReset();
        g_active_algo = ALGO_DIJKSTRA;
        dijkstraInit(daGetSize(&g_grid));       
    }

    if (g_active_algo == ALGO_DIJKSTRA && !dijkstraShouldStop()) dijkstraStep();

    // A*
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_FOUR) && g_start != NULL && g_goal != NULL)
    {
        LOG_DEBUG("SHIFT + 4: A*");
        gridReset();
        g_active_algo = ALGO_ASTAR;
        aStarInit(daGetSize(&g_grid));       
    }

    if (g_active_algo == ALGO_ASTAR && !aStarShouldStop()) aStarStep();
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

void*  
gridGetStart(void)
{
    return &g_start;
}

void*   
gridGetGoal(void)
{
    return &g_goal;
}

void*   
gridGetCell(u8 row, u8 col)
{
    return (Cell*)daGet(&g_grid, col + g_grid_cols * row);
}

u8      
gridGetRows(void)
{
    return g_grid_rows;
}

u8      
gridGetCols(void)
{
    return g_grid_cols;
}
