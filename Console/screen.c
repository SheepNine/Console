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