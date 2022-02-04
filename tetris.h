#ifndef TETRIS_H
#define TETRIS_H

#include "sdl_display.h"

void handleEvents();
void update();
bool running(); 
void render();
void clean();
bool checkColision(int cnt_y_local);
void initTetris();

#endif
