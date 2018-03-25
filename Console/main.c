#include "SDL.h"
#include "screen.h"
#define SDL_WINDOW_NONE 0

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
