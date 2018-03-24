#include "SDL.h"
#define SDL_WINDOW_NONE 0

int main(int argc, char** argv) {
	int initResult = SDL_Init(SDL_INIT_VIDEO);

	if (initResult == 0)
	{
		SDL_Window* window = SDL_CreateWindow(
			"Console",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			320, 248,
			SDL_WINDOW_NONE);

		if (window != NULL)
		{
			SDL_Surface* surface = SDL_GetWindowSurface(window);

			if (surface != NULL)
			{
				if (SDL_LockSurface(surface) == 0)
				{
					if (surface->format->format == SDL_PIXELFORMAT_RGB888)
					{
						int offset;
						for (int y = 0; y < surface->h; y++)
						{
							for (int x = 0; x < surface->w; x++)
							{
								offset = (x + y * surface->w) * surface->format->BytesPerPixel;
								((Uint8*)surface->pixels)[offset + 0] = 128; // Blue
								((Uint8*)surface->pixels)[offset + 1] = 128; // Green
								((Uint8*)surface->pixels)[offset + 2] = 0; // Red
							}
						}

						offset = (0 + 0 * surface->w) * surface->format->BytesPerPixel;
						((Uint8*)surface->pixels)[offset + 0] = 255; // Blue
						((Uint8*)surface->pixels)[offset + 1] = 255; // Green
						((Uint8*)surface->pixels)[offset + 2] = 255; // Red

						offset = (surface->w - 1 + 0 * surface->w) * surface->format->BytesPerPixel;
						((Uint8*)surface->pixels)[offset + 0] = 0; // Blue
						((Uint8*)surface->pixels)[offset + 1] = 0; // Green
						((Uint8*)surface->pixels)[offset + 2] = 255; // Red

						offset = (0 + (surface->h - 1) * surface->w) * surface->format->BytesPerPixel;
						((Uint8*)surface->pixels)[offset + 0] = 255; // Blue
						((Uint8*)surface->pixels)[offset + 1] = 0; // Green
						((Uint8*)surface->pixels)[offset + 2] = 0; // Red
					}
					SDL_UnlockSurface(surface);
				}
				SDL_UpdateWindowSurface(window);
			}

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
