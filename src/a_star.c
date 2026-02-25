#include "a_star.h"

#include "logger.h"
#include "ds/heap.h"

#include "grid.h"
#include "animate.h"

#include <stdlib.h>

static Heap     g_a_star_heap          = {0};
static b8       g_a_star_is_running    = 0;
static b8       g_a_star_has_finished  = 0;

static u32
manhattan_heuristic(u8 row, u8 col, u8 goal_row, u8 goal_col)
{
    return (u32)(abs(row - goal_row) + abs(col - goal_col));
}

static b8
a_star_comparator(void* a, void* b)
{
    Cell** cell_a = (Cell**)a;
    Cell** cell_b = (Cell**)b;

    u64 f_a = (*cell_a)->distance + (*cell_a)->heuristic;
    u64 f_b = (*cell_b)->distance + (*cell_b)->heuristic;

    return f_b < f_a;   // min-heap → smaller f first
}

void 
aStarInit(u64 capacity)
{
    g_a_star_heap = heapCreate(sizeof(Cell*), capacity, a_star_comparator);

    Cell** start        = gridGetStart();
    Cell** goal         = gridGetGoal();
    u8     goal_row     = (*goal)->row;
    u8     goal_col     = (*goal)->col;

    for (i16 row = 0; row < gridGetRows(); ++row) 
    {
        for (i16 col = 0; col < gridGetCols(); ++col) 
        {
            Cell* cell      = gridGetCell(row, col);
            cell->heuristic = manhattan_heuristic(row, col, goal_row, goal_col);
        }
    }

    (*start)->distance  = 0;

    // Add root node to a priority queue
    heapInsert(&g_a_star_heap, start);

    g_a_star_is_running    = 1;
    g_a_star_has_finished  = 0;
}

void 
aStarStep(void)
{
    if (g_a_star_is_running == 0 || g_a_star_has_finished == 1) return;

    // Loop on the heap as long as it's not empty
    if (heapIsEmpty(&g_a_star_heap) == WIM_TRUE)
    {
        LOG_INFO("Could not find path!");
        g_a_star_is_running    = 0;
        g_a_star_has_finished  = 1;

        heapDestroy(&g_a_star_heap);

        return;
    }

    // Choose the node with the minimum distance from the root node in the heap (root node will be selected first)
    Cell* cell;
    heapExtract(&g_a_star_heap, &cell);
    if (cell->is_visited == 1) return;
    cell->is_visited = 1;

    if (cell != gridGetStart() && cell != gridGetGoal()) cell->color = CELL_VISITED_COLOR;

    if (cell->is_goal == 1)
    {
        LOG_INFO("Found path!");
        LOG_INFO("Distance %d", cell->distance);
        g_a_star_is_running    = 0;
        g_a_star_has_finished  = 1;

        buildAnimationPath(cell);
        heapDestroy(&g_a_star_heap);

        return;
    }

    // For every non-visited child of the current node
    i16 directions[4][2] = {
        { 0, -1}, // top
        {-1,  0}, // left
        { 0,  1}, // bottom
        { 1,  0}  // right
    };

    i16 row = cell->row;
    i16 col = cell->col;

    for (u16 i = 0; i < 4; ++i)
    {
        i16 new_row = row + directions[i][0];
        i16 new_col = col + directions[i][1];

        if (new_row >= 0 && new_row < gridGetRows() &&
            new_col >= 0 && new_col < gridGetCols())
        {
            LOG_DEBUG("New row: %d | New col: %d", new_row, new_col);
            Cell* neighbor = gridGetCell(new_row, new_col);

            if (neighbor->is_visited == 1 || neighbor->is_wall == 1) continue;

            u64 temp = cell->distance + neighbor->weight;
            if (temp < neighbor->distance) 
            {
                neighbor->distance  = temp;
                neighbor->parent    = cell;
                heapInsert(&g_a_star_heap, &neighbor);
            }
        }
    }
}

b8  
aStarShouldStop(void)
{
    return (g_a_star_is_running == 0) && (g_a_star_has_finished == 1);
}