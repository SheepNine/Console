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

Uint8 Sample(Uint8* buffer, int x, int y, int scale) {
	int srcOffset = (x / scale) + (y / scale) * CONTENT_SIZE;
	return buffer[srcOffset];
}

void Draw(h_screen screen, SDL_Window* window) {
	SDL_Surface* surface = SDL_GetWindowSurface(window);

	if (surface != NULL)
	{
		if (SDL_LockSurface(surface) == 0)
		{
			if (surface->format->format == SDL_PIXELFORMAT_RGB888)
			{
				SDL_memset4(surface->pixels, 0x00AAAAAA, surface->w * surface->h);

				int scale = SDL_min(surface->w, surface->h) / CONTENT_SIZE;
				int contentOffsetX = (surface->w - CONTENT_SIZE * scale) / 2;
				int contentOffsetY = (surface->h - CONTENT_SIZE * scale) / 2;
				int contentOffset = (contentOffsetX + surface->w * contentOffsetY) * surface->format->BytesPerPixel;

				for (int y = 0; y < CONTENT_SIZE * scale; y++) {
					for (int x = 0; x < CONTENT_SIZE * scale; x++) {
						int destOffset = contentOffset + (x + surface->w * y) * surface->format->BytesPerPixel;

						((Uint8*)surface->pixels)[destOffset + 0] = Sample(screen->B, x, y, scale);
						((Uint8*)surface->pixels)[destOffset + 1] = Sample(screen->G, x, y, scale);
						((Uint8*)surface->pixels)[destOffset + 2] = Sample(screen->R, x, y, scale);
					}
				}
			}
			SDL_UnlockSurface(surface);
		}
		SDL_UpdateWindowSurface(window);
	}
}