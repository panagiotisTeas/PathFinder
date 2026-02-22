#ifndef PF_GRID_H
#define PF_GRID_H

#include "common.h"

void gridCreate(u16 window_width, u16 window_height, u8 grid_rows, u8 grid_cols);
void gridDestroy(void);

void gridUpdate(void);
void gridDraw(void);

#endif // PF_GRID_H