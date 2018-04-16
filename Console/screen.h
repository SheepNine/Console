#ifndef __screen_h
#define __screen_h
#include "SDL.h"
#define CONTENT_SIZE 248

typedef struct screen_def *h_screen;
h_screen create_screen();
void destroy_screen(h_screen screen);

void toggleCrtMode_screen(h_screen screen);

void clear_screen(h_screen screen, Uint8 r, Uint8 g, Uint8 b);

void setDemo_screen(h_screen screen);

void setPixel_screen(h_screen screen, Uint8 x, Uint8 y, Uint8 r, Uint8 g, Uint8 b, SDL_bool translucent);
void setPixelPacked_screen(h_screen screen, Uint8 x, Uint8 y, Uint16 color);
void setPixelRoundTrip_screen(h_screen screen, Uint8 x, Uint8 y, Uint8 r, Uint8 g, Uint8 b, SDL_bool translucent);

void blt_screen(h_screen screen, Uint8 scale, Uint32 *dest, Uint32 stride);

void drawSprite_screen(h_screen screen, Sint16 targetX, Sint16 targetY, Uint8 sizeX, Uint8 sizeY, Uint8* glyphPage, Uint8 glyphIndex, Uint16* palette, SDL_bool hFlip, SDL_bool vFlip, SDL_bool drawIndexZero);
void drawGlyph_screen(h_screen screen, Uint8 targetX, Uint8 targetY, Uint8* bitPlanes, Uint16* palette, SDL_bool hFlip, SDL_bool vFlip, SDL_bool drawIndexZero);

#endif
