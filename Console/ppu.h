#ifndef __ppu_h
#define __ppu_h
#include "SDL.h"
#include "screen.h"

typedef struct ppu_def *h_ppu;
h_ppu create_ppu();
void destroy_ppu(h_ppu ppu);

void render_ppu(h_ppu ppu, h_screen screen);

void setBackgroundPage_ppu(h_ppu ppu, Uint8 pageIndex, Uint8 *data);
void setSpritePage_ppu(h_ppu ppu, Uint8 pageIndex, Uint8 *data);

void setBackgroundPalette_ppu(h_ppu ppu, Uint8 paletteIndex, Uint16 *data);
void setSpritePalette_ppu(h_ppu ppu, Uint8 paletteIndex, Uint16 *data);

void setBackgroundPaletteColor_ppu(h_ppu ppu, Uint8 paletteIndex, Uint8 colorIndex,
	Uint8 r, Uint8 g, Uint8 b, SDL_bool transparent);
void setSpritePaletteColor_ppu(h_ppu ppu, Uint8 paletteIndex, Uint8 colorIndex,
	Uint8 r, Uint8 g, Uint8 b, SDL_bool transparent);

void setBackgroundTile_ppu(h_ppu ppu, Uint8 layerIndex, Uint8 x, Uint8 y,
	Uint8 pageIndex, Uint8 glyphIndex,
	SDL_bool hFlip, SDL_bool vFlip,
	Uint8 paletteIndex, SDL_bool drawColorZero);

void setLayerClip_ppu(h_ppu ppu, Uint8 layerIndex,
	Uint8 hClipY, SDL_bool clipBelow,
	Uint8 vClipX, SDL_bool clipRight);

void setLayerControl_ppu(h_ppu ppu, Uint8 layerIndex,
	SDL_bool enabled, SDL_bool drawSpritesFirst,
	Uint8 xOffset, Uint8 yOffset);

void setSpriteTile_ppu(h_ppu ppu, Uint8 spriteIndex,
	Uint8 pageIndex, Uint8 glyphIndex,
	SDL_bool hFlip, SDL_bool vFlip,
	Uint8 paletteIndex, SDL_bool drawColorZero);

void setSpriteControl_ppu(h_ppu ppu, Uint8 spriteIndex,
	SDL_bool enabled,
	Uint8 xOffset, SDL_bool negateXOffset,
	Uint8 yOffset, SDL_bool negateYOffset,
	Uint8 layer, Uint8 size);

#endif
