#include "dijkstra.h"

#include "logger.h"
#include "ds/heap.h"

#include "grid.h"
#include "animate.h"

static Heap     g_dijkstra_heap          = {0};
static b8       g_dijkstra_is_running    = 0;
static b8       g_dijkstra_has_finished  = 0;

static b8
dijkstra_comparator(void* a, void* b)
{
    Cell** cell_a = (Cell**)a;
    Cell** cell_b = (Cell**)b;

    return (*cell_b)->distance < (*cell_a)->distance;
}

void 
dijkstraInit(u64 capacity)
{
    g_dijkstra_heap = heapCreate(sizeof(Cell*), capacity, dijkstra_comparator);

    // Assign distance from root node to itself
    Cell** start        = gridGetStart();
    (*start)->distance  = 0;

    // Add root node to a priority queue
    heapInsert(&g_dijkstra_heap, start);

    g_dijkstra_is_running    = 1;
    g_dijkstra_has_finished  = 0;
}

void 
dijkstraStep(void)
{
    if (g_dijkstra_is_running == 0 || g_dijkstra_has_finished == 1) return;

    // Loop on the heap as long as it's not empty
    if (heapIsEmpty(&g_dijkstra_heap) == WIM_TRUE)
    {
        LOG_INFO("Could not find path!");
        g_dijkstra_is_running    = 0;
        g_dijkstra_has_finished  = 1;

        heapDestroy(&g_dijkstra_heap);

        return;
    }

    // Choose the node with the minimum distance from the root node in the heap (root node will be selected first)
    Cell* cell;
    heapExtract(&g_dijkstra_heap, &cell);
    if (cell->is_visited == 1) return;
    cell->is_visited = 1;

    if (cell != gridGetStart() && cell != gridGetGoal()) cell->color = CELL_VISITED_COLOR;

    if (cell->is_goal == 1)
    {
        LOG_INFO("Found path!");
        LOG_INFO("Distance %d", cell->distance);
        g_dijkstra_is_running    = 0;
        g_dijkstra_has_finished  = 1;

        buildAnimationPath(cell);
        heapDestroy(&g_dijkstra_heap);

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
                heapInsert(&g_dijkstra_heap, &neighbor);
            }
        }
    }
}

b8  
dijkstraShouldStop(void)
{
    return (g_dijkstra_is_running == 0) && (g_dijkstra_has_finished == 1);
}