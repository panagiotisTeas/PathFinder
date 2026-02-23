#include "bfs.h"

#include "logger.h"
#include "ds/queue.h"

#include "grid.h"
#include "animate.h"

static Queue    g_bfs_queue         = {0};
static b8       g_bfs_is_running    = 0;
static b8       g_bfs_has_finished  = 0;

void 
bfsInit(u64 capacity)
{
    g_bfs_queue = qCreate(capacity);

    // Get root node (start)
    Cell** cell = gridGetStart();
    // Add root node to the queue
    qEnqueue(&g_bfs_queue, cell, sizeof(Cell*));
    // Mark it as visited
    (*cell)->is_visited = 1;

    g_bfs_is_running    = 1;
    g_bfs_has_finished  = 0;
}

void 
bfsStep(void)
{
    if (g_bfs_is_running == 0 || g_bfs_has_finished == 1) return;

    // Loop on the queue as long as it's not empty
    if (qIsEmpty(&g_bfs_queue) == WIM_TRUE)
    {
        LOG_INFO("Could not find path!");
        g_bfs_is_running    = 0;
        g_bfs_has_finished  = 1;

        return;
    }

    // Get and remove the node at the top of the queue
    Cell* cell;
    qDequeue(&g_bfs_queue, &cell, sizeof(Cell*));

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
            Cell* neighbor = gridGetCell(new_row, new_col);

            if (neighbor->is_visited == 0 && neighbor->is_wall != 1)
            {
                // Mark it as visited
                neighbor->is_visited = 1;

                // Get the parent for visualization
                neighbor->parent = cell;

                // Check if it's the goal node, If so, then return it
                if (neighbor->is_goal)
                {
                    LOG_INFO("Found path!");
                    g_bfs_is_running    = 0;
                    g_bfs_has_finished  = 1;

                    buildAnimationPath(neighbor);

                    return;
                }

                // Change color to yellow
                neighbor->color = CELL_VISITED_COLOR;

                // Otherwise, push it to the queue
                qEnqueue(&g_bfs_queue, &neighbor, sizeof(Cell*));
            }
        }

    }
}

b8  
bfsShouldStop(void)
{
    return (g_bfs_is_running == 0) && (g_bfs_has_finished == 1);
}