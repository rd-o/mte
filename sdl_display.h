#ifndef SDL_DISPLAY_H
#define SDL_DISPLAY_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "constants.h"
#include <stdbool.h>
//#include <stdint.h>

//typedef uint32_t Uint32;

extern bool isRunning;
extern int pixelSize;

int init(const char* title, int width, int height, bool fullscreen);
void delay(Uint32 ms);
void playEScore();
void playERotate();
void playEPiecesDown();

extern SDL_Window *window;
extern SDL_Renderer *renderer;

struct PixelLoc {
	int x;
	int y;
};

#endif

