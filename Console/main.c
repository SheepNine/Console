#include "SDL.h"
#include "screen.h"
#define SDL_WINDOW_NONE 0

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
				int contentOffset = (contentOffsetX + surface->w * contentOffsetY);

				blt_screen(screen, scale, &((Uint32*)surface->pixels)[contentOffset], surface->w);
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
		h_screen screen = create_screen();
		setDemo_screen(screen);

		SDL_Window* window = SDL_CreateWindow(
			"Console",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			744, 744,
			SDL_WINDOW_RESIZABLE);

		if (window != NULL)
		{
			SDL_SetWindowMinimumSize(window, CONTENT_SIZE, CONTENT_SIZE);
			Draw(screen, window);

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
							Draw(screen, window);
						}
						if (evt.window.event == SDL_WINDOWEVENT_EXPOSED) {
							Draw(screen, window);
						}
						break;
					}
				} else {
					done = true;
				}
			}

			SDL_DestroyWindow(window);
		}

		destroy_screen(screen);
	}

	SDL_Quit();
	return 0;
}
