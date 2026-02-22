#include "bfs.h"

#include "logger.h"
#include "ds/queue.h"

#include "grid.h"

static Queue    g_bfs_queue       = {0};
static b8       g_bfs_initialized = 0;

void 
bfsInit(u64 capacity)
{
    if (g_bfs_initialized == 1) qDestroy(&g_bfs_queue);

    g_bfs_queue = qCreate(capacity);

    // Get root node (start)
    Cell** cell = gridGetStart();
    // Add root node to the queue
    qEnqueue(&g_bfs_queue, cell, sizeof(Cell*));
    // Mark it as visited
    (*cell)->is_visited = 1;
}

void 
bfsStep(void)
{

}

b8  
bfsIsInitialized(void)
{
    return g_bfs_initialized;
}