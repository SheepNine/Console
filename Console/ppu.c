#include "ppu.h"
#define RAM_SIZE 74752
#include "resources.h"

typedef struct ppu_def ppu, *h_ppu;

struct ppu_def {
	Uint8 ram[74752];
};

h_ppu create_ppu() {
	h_ppu result = (h_ppu)SDL_malloc(sizeof(ppu));
	SDL_memset(result, 0, 74752);
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

Uint8 brickGlyphPlanes[128] = {
	0xFF, 0xFF, 0xFC, 0xF8, 0xF0, 0xFB, 0xFF, 0x00,
	0xFF, 0x80, 0x83, 0x84, 0x88, 0x87, 0x80, 0x7F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0xFE, 0xFE, 0x1E, 0x1E, 0xFE, 0xFE, 0xFE, 0x00,
	0xFE, 0x01, 0xE1, 0x11, 0xE1, 0x01, 0x01, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0xFE, 0xFE, 0xFE, 0xE6, 0xDE, 0xFE, 0xFE, 0x00,
	0xFE, 0x01, 0x01, 0x19, 0x39, 0x01, 0x01, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0xFF, 0xFF, 0xEF, 0xDF, 0xFF, 0xF7, 0xFF, 0x00,
	0xFF, 0x80, 0x90, 0xB0, 0x80, 0x8C, 0x80, 0x7F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

Uint16 brickPalette[32] = {
	0x0000, 0x4104, 0x30C2, 0x5184, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x31C2, 0x1082, 0x5304, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

void render_ppu(h_ppu ppu, h_screen screen) {
	clear_screen(screen, 0, 0xAA, 0xAA);

	for (int x = 0; x < 31; x++) {
		for (int y = 0; y < 31; y++) {
			renderGlyph(screen, 8 * x, 8 * y,
				&brickGlyphPlanes[(x % 2) * 32 + (y % 2) * 64],
				&brickPalette[((y / 2) % 2) * 16],
				SDL_FALSE, SDL_FALSE, SDL_FALSE, 0xF0, 0x0F);
		}
	}

	Uint8 glyphBitPlanes[32] = {
		0x00, 0xFF, 0x11, 0x22, 0x44, 0x88, 0x81, 0x18,
		0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
		0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	};
	Uint16 palette[16] = {
		0x0000, 0x7FFF, 0x7C00, 0x001F,
		0x8000, 0xFFFF, 0xFC00, 0x801F,
		0x8000, 0xFFFF, 0xFC00, 0x801F,
		0x0000, 0x7FFF, 0x7C00, 0x001F,
	};

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
	renderSprite(screen, 232, 232 + 15, 2, 2, glyphs_rando, 33, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE, 0, 0);
}