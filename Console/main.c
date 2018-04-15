#include "SDL.h"
#include "screen.h"
#include "resources.h"
#define SDL_WINDOW_NONE 0

Uint8 brickGlyphPlanes[128] = {
	0xFF, 0xFF, 0xFC, 0xF8, 0xF0, 0xFB, 0xFF, 0x00,
	0xFF, 0x80, 0x83, 0x84, 0x88, 0x87, 0x80, 0x7F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0xFE, 0xFE, 0x1E, 0x1E, 0xFE, 0xFE, 0xFE, 0x00,
	0xFE, 0x01, 0xE1, 0x11, 0xE1, 0x01, 0x01, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0xFE, 0xFE, 0xFE, 0xE6, 0xDE, 0xFE, 0xFE, 0x00,
	0xFE, 0x01, 0x01, 0x19, 0x39, 0x01, 0x01, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0xFF, 0xFF, 0xEF, 0xDF, 0xFF, 0xF7, 0xFF, 0x00,
	0xFF, 0x80, 0x90, 0xB0, 0x80, 0x8C, 0x80, 0x7F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

Uint16 brickPalette[32] = {
	0x0000, 0x4104, 0x30C2, 0x5184, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	
	0x0000, 0x31C2, 0x1082, 0x5304, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

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

				clear_screen(screen, 0, 0xAA, 0xAA);

				for (int x = 0; x < 31; x++) {
					for (int y = 0; y < 31; y++) {
						drawGlyph_screen(screen, 8 * x, 8 * y,
							&brickGlyphPlanes[(x % 2) * 32 + (y % 2) * 64],
							&brickPalette[((y/2) % 2) * 16],
							SDL_FALSE, SDL_FALSE, SDL_FALSE);
					}
				}

				Uint8 glyphBitPlanes[32] = {
					0x00, 0xFF, 0x11, 0x22, 0x44, 0x88, 0x81, 0x18,
					0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
					0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
					0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
				};
				Uint16 palette[16] = {
					0x0000, 0x7FFF, 0x7C00, 0x001F,
					0x8000, 0xFFFF, 0xFC00, 0x801F,
					0x8000, 0xFFFF, 0xFC00, 0x801F,
					0x0000, 0x7FFF, 0x7C00, 0x001F,
				};

				drawSprite_screen(screen, 0, 0, 2, 2, glyphs_rando, 11, palette_vga, SDL_FALSE, SDL_FALSE, SDL_FALSE);
				drawSprite_screen(screen, 17, 0, 2, 2, glyphs_rando, 11, palette_vga, SDL_TRUE, SDL_FALSE, SDL_TRUE);
				drawSprite_screen(screen, 0, 17, 2, 2, glyphs_rando, 255, palette_vga, SDL_FALSE, SDL_TRUE, SDL_TRUE);
				drawSprite_screen(screen, 17, 17, 2, 2, glyphs_rando, 255, palette_vga, SDL_TRUE, SDL_TRUE, SDL_FALSE);

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
		setDemo_screen(screen);

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
