#ifndef PF_DIJKSTRA_H
#define PF_DIJKSTRA_H

#include "common.h"

void dijkstraInit(u64 capacity);
void dijkstraStep(void);

b8  dijkstraShouldStop(void);

#endif // PF_DIJKSTRA_H