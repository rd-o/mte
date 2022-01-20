#ifndef TETRIS_H
#define TETRIS_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdbool.h>

void init(const char* title, int width, int height, bool fullscreen);

extern bool isRunning;// = false;
//extern int cnt;// = 0;
extern SDL_Window *window;
extern SDL_Renderer *renderer;

void handleEvents();
void update();
bool running(); 
void render();
void clean();

#endif
