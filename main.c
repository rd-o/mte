#include "tetris.h"

int main(int argc, char *argv[])
{
	const int FPS = 60;
	const int frameDelay = 1000 / FPS;

	Uint32 frameStart;
	int frameTime;

	init("GameWindow", 800, 600, false);

	while (running())
	{
		frameStart = SDL_GetTicks();

		handleEvents();
		update();
		render();

		frameTime = SDL_GetTicks() - frameStart;

		if(frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}
	}

	clean();
	return 0;
}
