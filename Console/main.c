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
