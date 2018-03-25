#ifndef __screen_h
#define __screen_h
#include "SDL.h"
#define CONTENT_SIZE 248

typedef struct screen_def *h_screen;
h_screen create_screen();
void destroy_screen(h_screen screen);

void clear_screen(h_screen screen, Uint8 r, Uint8 g, Uint8 b);

void setDemo_screen(h_screen screen);

void setPixel_screen(h_screen screen, Uint8 x, Uint8 y, Uint8 r, Uint8 g, Uint8 b, SDL_bool translucent);
void setPixelPacked_screen(h_screen screen, Uint8 x, Uint8 y, Uint16 color);
void setPixelRoundTrip_screen(h_screen screen, Uint8 x, Uint8 y, Uint8 r, Uint8 g, Uint8 b, SDL_bool translucent);

void blt_screen(h_screen screen, Uint8 scale, Uint32 *dest, Uint32 stride);

#endif
