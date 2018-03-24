#include "SDL.h"
#define SDL_WINDOW_NONE 0

void Draw(SDL_Window* window) {
	SDL_Surface* surface = SDL_GetWindowSurface(window);

	if (surface != NULL)
	{
		if (SDL_LockSurface(surface) == 0)
		{
			if (surface->format->format == SDL_PIXELFORMAT_RGB888)
			{
				SDL_memset4(surface->pixels, 0x00AAAAAA, surface->w * surface->h);

				int scale;
				if (surface->w > surface->h) {
					scale = surface->h / 248;
				} else {
					scale = surface->w / 248;
				}
				int pictureOffsetX = (surface->w - 248 * scale) / 2;
				int pictureOffsetY = (surface->h - 248 * scale) / 2;
				int pictureOffset = (pictureOffsetX + surface->w * pictureOffsetY) * surface->format->BytesPerPixel;

				int offset;
				for (int y = 0; y < 248 * scale; y++)
				{
					for (int x = 0; x < 248 * scale; x++)
					{
						offset = pictureOffset + (x + y * surface->w) * surface->format->BytesPerPixel;
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
}

int main(int argc, char** argv) {
	int initResult = SDL_Init(SDL_INIT_VIDEO);

	if (initResult == 0)
	{
		SDL_Window* window = SDL_CreateWindow(
			"Console",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			744, 744,
			SDL_WINDOW_RESIZABLE);

		if (window != NULL)
		{
			SDL_SetWindowMinimumSize(window, 248, 248);
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
