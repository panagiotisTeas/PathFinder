#ifndef PF_BFS_H
#define PF_BFS_H

#include "common.h"

void bfsInit(u64 capacity);
void bfsStep(void);

b8  bfsShouldStop(void);

#endif // PF_BFS_H