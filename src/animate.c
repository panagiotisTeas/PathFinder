#include "animate.h"

#include "logger.h"
#include "ds/linked_list.h"

#include "grid.h"

static SLinkedList  g_path              = {0};
static b8           g_should_animate    = 0;

void
buildAnimationPath(void* cell)
{
    g_path = sllCreate();

    Cell* cell_ptr = (Cell*)cell;
    while (cell_ptr != NULL)
    {
        sllInsertTail(&g_path, &cell_ptr, sizeof(Cell*));
        cell_ptr = cell_ptr->parent;
    }

    g_should_animate = 1;
}

void
animatePath(void)
{
    if (g_should_animate == 0) return;

    if (sllGetNodeCount(&g_path) == 0)
    {
        g_should_animate = 0;
        sllDelete(&g_path);
        return;
    }

    Cell** cell_ptr     = (Cell**)g_path.tail->data;
    (*cell_ptr)->color  = CELL_SOLUTION_COLOR;
    sllDeleteTail(&g_path);
}