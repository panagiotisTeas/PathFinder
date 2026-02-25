#ifndef PF_A_STAR_H
#define PF_A_STAR_H

#include "common.h"

void aStarInit(u64 capacity);
void aStarStep(void);

b8  aStarShouldStop(void);

#endif // PF_A_STAR_H