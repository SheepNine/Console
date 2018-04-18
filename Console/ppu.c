#include "ppu.h"
#define RAM_SIZE 74752

typedef struct ppu_def ppu, *h_ppu;

struct ppu_def {
	Uint8 ram[74752];
};

h_ppu create_ppu() {
	h_ppu result = (h_ppu)SDL_malloc(sizeof(ppu));
	SDL_memset(result, 0, 74752);
	return result;
}

void destroy_ppu(h_ppu ppu) {
	SDL_free(ppu);
}

void render_ppu(h_ppu ppu, h_screen screen) {

}