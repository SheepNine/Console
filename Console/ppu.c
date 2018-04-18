#include "ppu.h"
#define RAM_SIZE 74752
#include "resources.h"

typedef struct ppu_def ppu, *h_ppu;

struct ppu_def {
	Uint8 ram[74752];
	Uint8 *bgPages[4];
	Uint8 *spritePages[4];
	Uint16 *tileMaps[4];
	Uint16 *bgPalettes[8];
	Uint16 *spritePalettes[8];
	Uint8 *spriteControls;
	Uint8 *spriteX;
	Uint8 *spriteY;
	Uint16 *spriteTiles;
	Uint8 *layerHClip;
	Uint8 *layerVClip;
	Uint8 *layerControls;
};

h_ppu create_ppu() {
	h_ppu result = (h_ppu)SDL_malloc(sizeof(ppu));
	SDL_memset(result, 0, 74752);
	Uint32 offset = 0;
	for (Uint8 i = 0; i < 4; i++) {
		result->bgPages[i] = &result->ram[offset];
		offset += 8 * 1024;
	}
	for (Uint8 i = 0; i < 4; i++) {
		result->spritePages[i] = &result->ram[offset];
		offset += 8 * 1024;
	}
	for (Uint8 i = 0; i < 4; i++) {
		result->tileMaps[i] = (Uint16*)&result->ram[offset];
		offset += 2 * 1024;
	}
	for (Uint8 i = 0; i < 8; i++) {
		result->bgPalettes[i] = (Uint16*)&result->ram[offset];
		offset += 32;
	}
	for (Uint8 i = 0; i < 8; i++) {
		result->bgPalettes[i] = (Uint16*)&result->ram[offset];
		offset += 32;
	}
	result->spriteControls = &result->ram[offset];
	offset += 100;
	result->spriteX = &result->ram[offset];
	offset += 100;
	result->spriteY = &result->ram[offset];
	offset += 100;
	result->spriteTiles = (Uint16*)&result->ram[offset];
	offset += 200;
	result->layerHClip = &result->ram[offset];
	offset += 4;
	result->layerVClip = &result->ram[offset];
	offset += 4;
	result->layerControls = &result->ram[offset];
	offset += 4;
	SDL_assert(offset == 74752);
	return result;
}

void destroy_ppu(h_ppu ppu) {
	SDL_free(ppu);
}

void renderGlyph(h_screen screen, Sint16 targetX, Sint16 targetY, Uint8* bitPlanes, Uint16* palette, SDL_bool hFlip, SDL_bool vFlip, SDL_bool drawIndexZero, Uint8 hClip, Uint8 vClip) {
	if (targetX >= CONTENT_SIZE || targetX <= -8) {
		return;
	}
	if (targetY >= CONTENT_SIZE || targetY <= -8) {
		return;
	}

	SDL_bool clipBelow = (hClip & 0x80) != 0;
	hClip <<= 1;
	SDL_bool clipRight = (vClip & 0x80) != 0;
	vClip <<= 1;

	for (Uint8 y = 0; y < 8; y++) {
		if (targetY >= CONTENT_SIZE - y || targetY + y < 0) {
			continue;
		}
		if (clipBelow && targetY + y >= hClip || !clipBelow && targetY + y < hClip) {
			continue;
		}
		Uint8 yOffset = vFlip ? 7 - y : y;
		Uint16 plane0 = ((Uint16)bitPlanes[yOffset + 0]) << 0;
		Uint16 plane1 = ((Uint16)bitPlanes[yOffset + 8]) << 1;
		Uint16 plane2 = ((Uint16)bitPlanes[yOffset + 16]) << 2;
		Uint16 plane3 = ((Uint16)bitPlanes[yOffset + 24]) << 3;
		for (Uint8 x = 0; x < 8; x++) {
			if (targetX >= CONTENT_SIZE - x || targetX + x < 0) {
				continue;
			}
			if (clipRight && targetX + x >= vClip || !clipRight && targetX + x < vClip) {
				continue;
			}

			Uint8 xOffset = hFlip ? x : 7 - x;
			Uint16 mask = 1 << xOffset;

			Uint16 paletteIndex =
				((plane0 & (mask << 0)) >> (xOffset)) |
				((plane1 & (mask << 1)) >> (xOffset)) |
				((plane2 & (mask << 2)) >> (xOffset)) |
				((plane3 & (mask << 3)) >> (xOffset));

			if (paletteIndex != 0 || drawIndexZero) {
				setPixelPacked_screen(screen, x + targetX, y + targetY, palette[paletteIndex]);
			}
		}
	}
}

SDL_bool drawIndexZero(Uint16 tile) {
	return (tile & 0x8000) == 0x8000;
}

SDL_bool hFlip(Uint16 tile) {
	return (tile & 0x4000) == 0x8000;
}

SDL_bool vFlip(Uint16 tile) {
	return (tile & 0x2000) == 0x8000;
}

Uint8 paletteIndex(Uint16 tile) {
	return (tile & 0x1C00) >> 10;
}

Uint8 pageIndex(Uint16 tile) {
	return (tile & 0x0300) >> 8;
}

Uint8 glyphIndex(Uint16 tile) {
	return (tile & 0xFF);
}

Uint16 packTile(SDL_bool drawIndexZero, SDL_bool hFlip, SDL_bool vFlip, Uint8 paletteIndex, Uint8 pageIndex, Uint8 glyphIndex) {
	SDL_assert(paletteIndex < 8);
	SDL_assert(pageIndex < 4);

	return (drawIndexZero ? 0x8000 : 0x0000)
		| (hFlip ? 0x4000 : 0x0000)
		| (vFlip ? 0x2000 : 0x0000)
		| (paletteIndex << 10)
		| (pageIndex << 8)
		| glyphIndex;
}

void renderTile(h_ppu ppu, Sint16 targetX, Sint16 targetY, Uint16 tile, Uint8 hClip, Uint8 vClip, h_screen screen) {
	renderGlyph(screen, targetX, targetY, &ppu->bgPages[pageIndex(tile)][32 * glyphIndex(tile)], ppu->bgPalettes[paletteIndex(tile)], hFlip(tile), vFlip(tile), drawIndexZero(tile), hClip, vClip);
}

void renderSprite(h_screen screen, Sint16 targetX, Sint16 targetY, Uint8 sizeX, Uint8 sizeY, Uint8* glyphPage, Uint8 firstGlyphIndex, Uint16* palette, SDL_bool hFlip, SDL_bool vFlip, SDL_bool drawIndexZero, Uint8 hClip, Uint8 vClip) {
	if (targetX >= CONTENT_SIZE || targetX + 8 * sizeX <= 0) {
		return;
	}
	if (targetY >= CONTENT_SIZE || targetY + 8 * sizeY <= 0) {
		return;
	}

	Sint8 shiftX = 8;
	if (hFlip) {
		shiftX = -8;
		targetX += 8 * (sizeX - 1);
	}
	Sint8 shiftY = 8;
	if (vFlip) {
		shiftY = -8;
		targetY += 8 * (sizeY - 1);
	}

	for (Uint8 y = 0; y < sizeY; y++) {
		Uint8 glyphRow = (firstGlyphIndex + (y << 3)) & 0xF8;
		for (Uint8 x = 0; x < sizeX; x++) {
			Uint8 glyphCol = (firstGlyphIndex + x) & 0x07;
			Uint8 glyphIndex = glyphRow | glyphCol;

			renderGlyph(screen, targetX + shiftX * x, targetY + shiftY * y, &glyphPage[32 * glyphIndex], palette, hFlip, vFlip, drawIndexZero, hClip, vClip);
		}
	}
}

void renderLayerTiles(h_ppu ppu, Uint8 layerIndex, Uint8 offsetX, Uint8 offsetY, Uint8 hClip, Uint8 vClip, h_screen screen) {
	Uint16 tileIndex = 0;
	for (Sint16 y = 0 - offsetY; y <= 248 - offsetY; y += 8) {
		for (Sint16 x = 0 - offsetX; x <= 248 - offsetX; x += 8) {
			renderTile(ppu, x, y, ppu->tileMaps[layerIndex][tileIndex++], hClip, vClip, screen);
		}
	}
}

SDL_bool layerEnabled(Uint8 layerControl) {
	return (layerControl & 0x80) == 0x80;
}

SDL_bool drawSpritesFirst(Uint8 layerControl) {
	return (layerControl & 0x40) == 0x40;
}

Uint8 layerXOffset(Uint8 layerControl) {
	return (layerControl & 0x28) >> 3;
}

Uint8 layerYOffset(Uint8 layerControl) {
	return (layerControl & 0x07);
}

Uint8 packLayerControl(SDL_bool enabled, SDL_bool drawSpritesFirst, Uint8 xOffset, Uint8 yOffset) {
	SDL_assert(xOffset < 8);
	SDL_assert(yOffset < 8);
	return (enabled ? 0x80 : 0x00)
		| (drawSpritesFirst ? 0x40 : 0x00)
		| (xOffset << 3)
		| yOffset;
}

void renderLayer(h_ppu ppu, Uint8 layerIndex, h_screen screen) {
	Uint8 layerControl = ppu->layerControls[layerIndex];
	Uint8 layerHClip = ppu->layerHClip[layerIndex];
	Uint8 layerVClip = ppu->layerVClip[layerIndex];

	if (!layerEnabled(layerControl)) {
		return;
	}

	if (drawSpritesFirst(layerControl)) {
		renderLayerTiles(ppu, layerIndex, layerXOffset(layerControl), layerYOffset(layerControl), layerHClip, layerVClip, screen);
	}
	// TODO: draw sprites
	if (!drawSpritesFirst(layerControl)) {
		renderLayerTiles(ppu, layerIndex, layerXOffset(layerControl), layerYOffset(layerControl), layerHClip, layerVClip, screen);
	}
}


void render_ppu(h_ppu ppu, h_screen screen) {
	clear_screen(screen, 0, 0xAA, 0xAA);

	for (int i = 0; i < 4; i++) {
		renderLayer(ppu, i, screen);
	}

	/*for (int x = 0; x < 31; x++) {
		for (int y = 0; y < 31; y++) {
			renderGlyph(screen, 8 * x, 8 * y,
				&glyphs_rando[32 * (1 + (x % 2) + 2 * (y % 2))],
				palette_vga,
				SDL_FALSE, SDL_FALSE, SDL_FALSE, 0xF0, 0x0F);
		}
	}

	renderSprite(screen, 0, 0, 2, 2, glyphs_rando, 11, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);
	renderSprite(screen, 17, 0, 2, 2, glyphs_rando, 11, palette_vga, SDL_TRUE, SDL_FALSE, SDL_TRUE, 0, 0);
	renderSprite(screen, 0, 17, 2, 2, glyphs_rando, 255, palette_vga, SDL_FALSE, SDL_TRUE, SDL_TRUE, 0, 0);
	renderSprite(screen, 17, 17, 2, 2, glyphs_rando, 255, palette_vga, SDL_TRUE, SDL_TRUE, SDL_FALSE, 0, 0);

	renderSprite(screen, 0, 0, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);
	renderSprite(screen, 0 - 15, 0, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);
	renderSprite(screen, 0, 0 - 15, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);

	renderSprite(screen, 232, 0, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);
	renderSprite(screen, 232 + 15, 0, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);
	renderSprite(screen, 232, 0 - 15, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);

	renderSprite(screen, 0, 232, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);
	renderSprite(screen, 0 - 15, 232, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);
	renderSprite(screen, 0, 232 + 15, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);

	renderSprite(screen, 232, 232, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);
	renderSprite(screen, 232 + 15, 232, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);
	renderSprite(screen, 232, 232 + 15, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);*/
}

void setLayerControl_ppu(h_ppu ppu, Uint8 layerIndex,
		SDL_bool enabled, SDL_bool drawSpritesFirst,
		Uint8 xOffset, Uint8 yOffset) {
	ppu->layerControls[layerIndex] = packLayerControl(enabled, drawSpritesFirst, xOffset, yOffset);
}

void setLayerClip_ppu(h_ppu ppu, Uint8 layerIndex,
		Uint8 hClipY, SDL_bool clipBelow,
		Uint8 vClipX, SDL_bool clipRight) {
	SDL_assert((hClipY & 0x01) == 0x00);
	SDL_assert((vClipX & 0x01) == 0x00);

	ppu->layerHClip[layerIndex] = (clipBelow ? 0x80 : 0x00) | (hClipY >> 1);
	ppu->layerVClip[layerIndex] = (clipRight ? 0x80 : 0x00) | (vClipX >> 1);
}

void setBackgroundPage_ppu(h_ppu ppu, Uint8 pageIndex, Uint8 *data) {
	SDL_memcpy(ppu->bgPages[pageIndex], data, 8192);
}


void setBackgroundPalette_ppu(h_ppu ppu, Uint8 paletteIndex, Uint16 *data) {
	SDL_memcpy(ppu->bgPalettes[paletteIndex], data, 32);
}

void setBackgroundTile_ppu(h_ppu ppu, Uint8 layerIndex, Uint8 x, Uint8 y,
		Uint8 pageIndex, Uint8 glyphIndex,
		SDL_bool hFlip, SDL_bool vFlip,
		Uint8 paletteIndex, SDL_bool drawColorZero) {
	ppu->tileMaps[layerIndex][x + 32 * y] = packTile(drawColorZero, hFlip, vFlip, paletteIndex, pageIndex, glyphIndex);
}