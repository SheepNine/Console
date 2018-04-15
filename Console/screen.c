#include "screen.h"
#define CONTENT_SIZE 248

typedef struct screen_def screen, *h_screen;

struct screen_def {
	Uint8 R[CONTENT_SIZE * CONTENT_SIZE];
	Uint8 G[CONTENT_SIZE * CONTENT_SIZE];
	Uint8 B[CONTENT_SIZE * CONTENT_SIZE];
};

h_screen create_screen() {
	h_screen result = (h_screen)SDL_malloc(sizeof(screen));
	clear_screen(result, 0, 255, 255);
	return result;
}

void destroy_screen(h_screen screen) {
	SDL_free(screen);
}

void clear_screen(h_screen screen, Uint8 r, Uint8 g, Uint8 b) {
	SDL_memset(screen->R, r, CONTENT_SIZE * CONTENT_SIZE);
	SDL_memset(screen->G, g, CONTENT_SIZE * CONTENT_SIZE);
	SDL_memset(screen->B, b, CONTENT_SIZE * CONTENT_SIZE);
}

void setPixel_screen(h_screen screen, Uint8 x, Uint8 y, Uint8 r, Uint8 g, Uint8 b, SDL_bool translucent) {
	SDL_assert(x < CONTENT_SIZE);
	SDL_assert(x < CONTENT_SIZE);

	int offset = x + y * CONTENT_SIZE;
	if (translucent) {
		screen->R[offset] = (screen->R[offset] + r) >> 1;
		screen->G[offset] = (screen->G[offset] + g) >> 1;
		screen->B[offset] = (screen->B[offset] + b) >> 1;
	}
	else {
		screen->R[offset] = r;
		screen->G[offset] = g;
		screen->B[offset] = b;
	}
}

void setPixelPacked_screen(h_screen screen, Uint8 x, Uint8 y, Uint16 color) {
	setPixel_screen(screen, x, y,
		((color & 0x7C00) >> 7),
		((color & 0x03E0) >> 2),
		((color & 0x001F) << 3),
		(color & 0x8000 ? SDL_TRUE : SDL_FALSE));
}

void setPixelRoundTrip_screen(h_screen screen, Uint8 x, Uint8 y, Uint8 r, Uint8 g, Uint8 b, SDL_bool translucent) {
	setPixelPacked_screen(screen, x, y,
		(translucent ? 0x8000 : 0x0000) |
		((r & 0xF8) << 7) |
		((g & 0xF8) << 2) |
		((b & 0xF8) >> 3));
}

void setDemo_screen(h_screen screen) {
	for (int y = 0; y < CONTENT_SIZE; y++)
	{
		for (int x = 0; x < CONTENT_SIZE; x++)
		{
			setPixelRoundTrip_screen(screen, x, y, x | y, x & y, x ^ y, SDL_FALSE);
		}
		setPixelRoundTrip_screen(screen, y / 2, y, 255, 255, 255, SDL_TRUE);
	}

}

Uint32 samplePixel_screen(h_screen screen, int x, int y, int scale) {
	int srcOffset = (x / scale) + (y / scale) * CONTENT_SIZE;
	return (screen->R[srcOffset] << 16) | (screen->G[srcOffset] << 8) | (screen->B[srcOffset]);
}

void blt_screen(h_screen screen, Uint8 scale, Uint32 *dest, Uint32 stride) {
	for (int y = 0; y < CONTENT_SIZE * scale; y++) {
		for (int x = 0; x < CONTENT_SIZE * scale; x++) {
			dest[x + stride * y] = samplePixel_screen(screen, x, y, scale);
		}
	}
}

void drawGlyph_screen(h_screen screen, Uint8 targetX, Uint8 targetY, Uint8* bitPlanes, Uint16* palette, SDL_bool hFlip, SDL_bool vFlip, SDL_bool drawIndexZero) {
	for (int y = 0; y < 8; y++) {
		if (targetY >= CONTENT_SIZE - y) {
			continue;
		}
		Uint8 yOffset = vFlip ? 7 - y : y;
		Uint16 plane0 = ((Uint16)bitPlanes[yOffset +  0]) << 0;
		Uint16 plane1 = ((Uint16)bitPlanes[yOffset +  8]) << 1;
		Uint16 plane2 = ((Uint16)bitPlanes[yOffset + 16]) << 2;
		Uint16 plane3 = ((Uint16)bitPlanes[yOffset + 24]) << 3;
		for (int x = 0; x < 8; x++) {
			if (targetX >= CONTENT_SIZE - x) {
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

void drawSprite_screen(h_screen screen, Sint16 targetX, Sint16 targetY, Uint8 sizeX, Uint8 sizeY, Uint8* glyphPage, Uint8 firstGlyphIndex, Uint16* palette, SDL_bool hFlip, SDL_bool vFlip, SDL_bool drawIndexZero) {
	int shiftX = 8;
	if (hFlip) {
		shiftX = -8;
		targetX += 8 * (sizeX - 1);
	}
	int shiftY = 8;
	if (vFlip) {
		shiftY = -8;
		targetY += 8 * (sizeY - 1);
	}

	for (int y = 0; y < sizeY; y++) {
		int glyphRow = ((firstGlyphIndex >> 3) + y) % 0x20;
		for (int x = 0; x < sizeX; x++) {
			int glyphCol = ((firstGlyphIndex & 0x7) + x) % 0x8;
			int glyphIndex = (glyphRow << 3) | glyphCol;

			drawGlyph_screen(screen, targetX + shiftX * x, targetY + shiftY * y, &glyphPage[32 * glyphIndex], palette, hFlip, vFlip, drawIndexZero);
		}
	}
}