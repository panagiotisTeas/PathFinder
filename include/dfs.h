#ifndef PF_DFS_H
#define PF_DFS_H

#include "common.h"

void dfsInit(u64 capacity);
void dfsStep(void);

b8  dfsShouldStop(void);

#endif // PF_DFS_H