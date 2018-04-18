#ifndef __ppu_h
#define __ppu_h
#include "SDL.h"
#include "screen.h"

typedef struct ppu_def *h_ppu;
h_ppu create_ppu();
void destroy_ppu(h_ppu ppu);

void render_ppu(h_ppu ppu, h_screen screen);

#endif
