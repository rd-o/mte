#include "tetris.h"
#include "pieces.h"
#include <stdlib.h>

#define X_SIZE 10
#define Y_SIZE 18
#define PIECE_DELAY 100

SDL_Texture* playerTex;
SDL_Rect srcR, destR, staticPixel;
Uint32 pieceCnt = 0;
int tm[X_SIZE][Y_SIZE];
//large size to be able to rotate the piece
int currentPiece[PIECE_H][PIECE_H];
static int pixelSize = 16;

bool isRunning = false;
int cnt_y  = 0;
int cnt_x  = 5;
SDL_Window *window;
SDL_Renderer *renderer;
int posMovingPieceX;
int posMovingPieceY;

void assignNewPiece()
{
	int selectedPiece = random() % 7;

	printf("selectedPiece: %d\n", selectedPiece);

	int i,j;
	for(i = 0; i < PIECE_W; i++)
		for(j = 0; j < PIECE_H; j++)
			currentPiece[j][i] = pieces[selectedPiece][j][i];
}

void initTetris()
{
	int i, j;
	for(i = 0; i < X_SIZE; i++)
		for(j = 0; j < Y_SIZE; j++)
			tm[i][j] = 0;

	//tm[1][1] = 1;
	tm[0][17] = 1;
	tm[9][17] = 1;
	assignNewPiece();
}

void init(const char* title, int width, int height, bool fullscreen)
{
	int flags = 0;
	
	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		}

		isRunning = true;
	}

	SDL_Surface* tmpSurface = IMG_Load("pixel.png");
	playerTex = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	pieceCnt = SDL_GetTicks();

	initTetris();
}

void xCollision()
{
	
}

void handleEvents()
{
	SDL_Event event;

	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT :
		isRunning = false;
		break;
	default:
		break;
	}

	if( event.type == SDL_KEYDOWN ){
		switch( event.key.keysym.sym ){
		    case SDLK_LEFT:
			if(cnt_x > 0)
			{
				cnt_x--;
			}
		    	break;

                    case SDLK_RIGHT:
			if(cnt_x < X_SIZE - 1)
			{
				cnt_x++;
			}
                    	break;
		}
	}
}

void checkColision()
{
	/*
	if(cnt_y == Y_SIZE)
	{
		tm[cnt_x][cnt_y - 1] = 1; 
		cnt_x = 5;
		cnt_y = 0;
	}
	if(tm[cnt_x][cnt_y] == 1){
		tm[cnt_x][cnt_y - 1] = 1; 
		cnt_x = 5;
		cnt_y = 0;
	}*/
	bool collided = false;
	int i,j;
	for(i = 0; i < PIECE_W && collided == false; i++)
		for(j = 0; j < PIECE_H && collided == false; j++)
		{
			if(currentPiece[j][i] == tm[j + cnt_x][i + cnt_y] && currentPiece[j][i] == 1) 
			{
				collided = true;
			}

			if(i + cnt_y == Y_SIZE && currentPiece[j][i] == 1)
			{
				collided = true;
			}
			
			if(j + cnt_x == X_SIZE && currentPiece[j][i] == 1)
			{
				collided = true;
			}
		}

	if(collided){
		for(i = 0; i < PIECE_W; i++)
			for(j = 0; j < PIECE_H; j++)
				if(currentPiece[j][i] == 1){
					printf("x: %d, y: %d\n", i + cnt_x, j + cnt_y - 1);
					tm[j + cnt_x][i + cnt_y - 1] = 1; 
				}
		printf("--- end collision ---\n");
		cnt_x = 5;
		cnt_y = 0;
		assignNewPiece();
	}

}

void update()
{
	destR.h = 16;
	destR.w = 16;
	staticPixel.h = 16;
	staticPixel.w = 16;

	//staticPixel.x = 20;
	//staticPixel.y = 20;

	Uint32 currentPieceCnt = SDL_GetTicks();
	if((currentPieceCnt - pieceCnt) > PIECE_DELAY)
	{
		cnt_y++;
		checkColision();
		posMovingPieceX = cnt_x * 16;
		posMovingPieceY = cnt_y * 16;
		pieceCnt = currentPieceCnt;
		//printf("%d\n", destR.y);
	}
}

void renderMovingPiece()
{
	int i,j;
	for(i = 0; i < PIECE_W; i++)
		for(j = 0; j < PIECE_H; j++)
		{
			if(currentPiece[j][i]){
				destR.x = j * pixelSize + posMovingPieceX;
				destR.y = i * pixelSize + posMovingPieceY;
				SDL_RenderCopy(renderer, playerTex, NULL, &destR);
			}
		}
	
}

void render()
{
	SDL_RenderClear(renderer);
	//SDL_RenderCopy(renderer, playerTex, NULL, &destR);
	renderMovingPiece();

	int i,j;
	for(i = 0; i < X_SIZE; i++)
		for(j = 0; j < Y_SIZE; j++)
			if(tm[i][j])
			{
				staticPixel.x = i * pixelSize;
				staticPixel.y = j * pixelSize;
				SDL_RenderCopy(renderer, playerTex, NULL, &staticPixel);
			}
	SDL_RenderPresent(renderer);
}

void clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

bool running()
{
	return isRunning;
}
