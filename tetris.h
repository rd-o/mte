#ifndef TETRIS_H
#define TETRIS_H
#include <stdbool.h>

#ifdef __ARDUINO_TETRIS__
#include "arduino_display.h"
#include "Arduino.h"
#endif

#ifndef __ARDUINO_TETRIS__
#include "sdl_display.h"
#endif

void handleEvents();
void update();
bool running(); 
void render();
void clean();
bool checkColision(int cnt_y_local);
void initTetris();
void mainGameLoop();

#endif
