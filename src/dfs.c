#include "dfs.h"

#include "logger.h"
#include "ds/stack.h"

#include "grid.h"
#include "animate.h"

static Stack    g_dfs_stack         = {0};
static b8       g_dfs_is_running    = 0;
static b8       g_dfs_has_finished  = 0;

void 
dfsInit(u64 capacity)
{
    g_dfs_stack = stackCreate(sizeof(Cell*), capacity, 1);

    // Get root node (start)
    Cell** cell = gridGetStart();
    // Add root node to the queue
    stackPush(&g_dfs_stack, cell);
    
    g_dfs_is_running    = 1;
    g_dfs_has_finished  = 0;
}

void 
dfsStep(void)
{
    if (g_dfs_is_running == 0 || g_dfs_has_finished == 1) return;

    // Loop on the stack as long as it's not empty
    if (stackIsEmpty(&g_dfs_stack) == WIM_TRUE)
    {
        LOG_INFO("Could not find path!");
        g_dfs_is_running    = 0;
        g_dfs_has_finished  = 1;

        stackDestroy(&g_dfs_stack);

        return;
    }

    // Get and remove the node at the top of the stack
    Cell* cell;
    stackPop(&g_dfs_stack, &cell);
    // Mark it as visited
    cell->is_visited = 1;

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
                    g_dfs_is_running    = 0;
                    g_dfs_has_finished  = 1;

                    buildAnimationPath(neighbor);
                    stackDestroy(&g_dfs_stack);

                    return;
                }

                // Change color to yellow
                neighbor->color = CELL_VISITED_COLOR;

                // Otherwise, push it to the stack
                stackPush(&g_dfs_stack, &neighbor);
            }
        }
    }
}

b8  
dfsShouldStop(void)
{
    return (g_dfs_is_running == 0) && (g_dfs_has_finished == 1);
}