#include "tetris.h"


void setup() {                
	pinMode(13, OUTPUT);     
	initTetris();
	pinMode(6, INPUT);
	pinMode(7, INPUT);
	pinMode(8, INPUT);
	pinMode(9, INPUT);

}

void loop() {
	delay(100);
	mainGameLoop();
}
