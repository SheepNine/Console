#include "SDL.h"
#define SDL_WINDOW_NONE 0

#define CONTENT_SIZE 248

Uint8 R[CONTENT_SIZE * CONTENT_SIZE];
Uint8 G[CONTENT_SIZE * CONTENT_SIZE];
Uint8 B[CONTENT_SIZE * CONTENT_SIZE];

void InitBuffer() {
	for (int y = 0; y < CONTENT_SIZE; y++)
	{
		for (int x = 0; x < CONTENT_SIZE; x++)
		{
			R[x + CONTENT_SIZE * y] = x | y;
			G[x + CONTENT_SIZE * y] = x & y;
			B[x + CONTENT_SIZE * y] = x ^ y;
		}
	}
}

Uint8 Sample(Uint8* buffer, int x, int y, int scale) {
	int srcOffset = (x / scale) + (y / scale) * CONTENT_SIZE;
	return buffer[srcOffset];
}

void Draw(SDL_Window* window) {
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

						((Uint8*)surface->pixels)[destOffset + 0] = Sample(B, x, y, scale);
						((Uint8*)surface->pixels)[destOffset + 1] = Sample(G, x, y, scale);
						((Uint8*)surface->pixels)[destOffset + 2] = Sample(R, x, y, scale);
					}
				}
			}
			SDL_UnlockSurface(surface);
		}
		SDL_UpdateWindowSurface(window);
	}
}

int main(int argc, char** argv) {
	int initResult = SDL_Init(SDL_INIT_VIDEO);

	if (initResult == 0)
	{
		InitBuffer();

		SDL_Window* window = SDL_CreateWindow(
			"Console",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			744, 744,
			SDL_WINDOW_RESIZABLE);

		if (window != NULL)
		{
			SDL_SetWindowMinimumSize(window, CONTENT_SIZE, CONTENT_SIZE);
			Draw(window);

			SDL_bool done = SDL_FALSE;
			while (!done)
			{
				SDL_Event evt;
				if (SDL_WaitEvent(&evt)) {
					switch (evt.type)
					{
					case SDL_WINDOWEVENT:
						if (evt.window.event == SDL_WINDOWEVENT_CLOSE) {
							done = SDL_TRUE;
						}
						if (evt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
							Draw(window);
						}
						if (evt.window.event == SDL_WINDOWEVENT_EXPOSED) {
							Draw(window);
						}
						break;
					}
				} else {
					done = true;
				}
			}

			SDL_DestroyWindow(window);
		}
	}

	SDL_Quit();
	return 0;
}
