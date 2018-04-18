#include "SDL.h"
#include "screen.h"
#include "ppu.h"
#include "resources.h"
#define SDL_WINDOW_NONE 0

Uint32 frameEventCode;
SDL_atomic_t timerQueue;

void Draw(h_screen screen, SDL_Window* window) {
	Uint64 startTimer = SDL_GetPerformanceCounter();
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

				h_ppu ppu = create_ppu();

				setLayerControl_ppu(ppu, 0, SDL_TRUE, SDL_FALSE, 0, 3);
				setBackgroundPage_ppu(ppu, 0, glyphs_rando);
				setBackgroundPalette_ppu(ppu, 0, palette_vga);
				setLayerClip_ppu(ppu, 0, 6, SDL_FALSE, 4, SDL_FALSE);

				for (int y = 0; y < 32; y++) {
					for (int x = 0; x < 32; x++) {
						setBackgroundTile_ppu(ppu, 0, x, y, 0, (1 + (x % 2) + 2 * (y % 2)), SDL_FALSE, SDL_FALSE, 0, SDL_FALSE);
					}
				}

				render_ppu(ppu, screen);
				destroy_ppu(ppu);

				blt_screen(screen, scale, &((Uint32*)surface->pixels)[contentOffset], surface->w);
			}
			SDL_UnlockSurface(surface);
		}
		SDL_UpdateWindowSurface(window);
	}
	Uint64 endTimer = SDL_GetPerformanceCounter();
	Uint64 runtimeMs = (endTimer - startTimer) * 1000 / SDL_GetPerformanceFrequency();
	SDL_Log("Draw: %lld ms", runtimeMs);
}

Uint32 timerCallback(Uint32 interval, void *param)
{
	SDL_Event event;
	SDL_UserEvent userevent;

	userevent.type = SDL_USEREVENT;
	userevent.code = frameEventCode;
	userevent.data1 = NULL;
	userevent.data2 = NULL;

	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_AtomicIncRef(&timerQueue);
	SDL_PushEvent(&event);

	return interval;
}

int main(int argc, char** argv) {
	int initResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	if (initResult == 0)
	{
		h_screen screen = create_screen();

		SDL_Window* window = SDL_CreateWindow(
			"Console",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			744, 744,
			SDL_WINDOW_RESIZABLE);

		if (window != NULL)
		{
			SDL_SetWindowMinimumSize(window, CONTENT_SIZE, CONTENT_SIZE);
			//Draw(screen, window);

			frameEventCode = SDL_RegisterEvents(1);
			if (frameEventCode != (Uint32)-1)
			{
				SDL_TimerID timerId = SDL_AddTimer(42, timerCallback, NULL);

				if (timerId != 0) {
					SDL_bool done = SDL_FALSE;
					Uint8 frameCounter = 0;
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
								/*if (evt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
									Draw(screen, window);
								}
								if (evt.window.event == SDL_WINDOWEVENT_EXPOSED) {
									Draw(screen, window);
								}*/
								break;
							case SDL_USEREVENT:
								if (evt.user.code == frameEventCode) {
									SDL_bool doDraw = SDL_AtomicDecRef(&timerQueue);
									if (doDraw) {
										Draw(screen, window);
									} else {
										SDL_Log("Frame skipped!");
									}
								}
								break;
							case SDL_KEYDOWN:
								if (evt.key.keysym.scancode == SDL_SCANCODE_F1) {
									toggleCrtMode_screen(screen);
								}
								break;
							}
						}
						else {
							done = true;
						}
					}
				}
				SDL_RemoveTimer(timerId);
			}
			SDL_DestroyWindow(window);
		}

		destroy_screen(screen);
	}

	SDL_Quit();
	return 0;
}
