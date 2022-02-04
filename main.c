#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include "tetris.h"
const int FPS = 60;
const int frameDelay = 1000 / FPS;

void mainGameLoop(){
	Uint32 frameStart;
	int frameTime;

	frameStart = SDL_GetTicks();

	handleEvents();
	update();
	render();

	frameTime = SDL_GetTicks() - frameStart;

	if(frameDelay > frameTime)
	{
		delay(frameDelay - frameTime);
	}
}

int main(int argc, char *argv[])
{

	if(init("tetris", 160 * 4, 144 * 4, false) == 1)
	{
		return 0;
	}
	initTetris();


	#ifdef __EMSCRIPTEN__
                emscripten_set_main_loop(mainGameLoop, 0, 1);
	#endif

	#ifndef __EMSCRIPTEN__
		while (running())
		{
			mainGameLoop();
		}
	#endif

	clean();
	return 0;
}
