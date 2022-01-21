#include "tetris.h"
#include "pieces.h"
#include <stdlib.h>

#define X_SIZE 10
#define Y_SIZE 18
#define PIECE_DELAY 500

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
int score;

void removeLine(int y)
{
	int i,j;
	for(j = y; j > 0; j--)
		for(i = 0; i < X_SIZE; i++)
			tm[i][j] = tm[i][j - 1];
}

void removeLines(int lines[PIECE_H]){
	int i;
	for(i = 0; i < PIECE_H; i++)
		if(lines[i])
			removeLine(lines[i]);
}

void scoring()
{
	int i,j;
	int lines[PIECE_H] = {0, 0, 0, 0};
	int cntLines = 0;
	bool lineComplete = true;
	for(j = 0; j < Y_SIZE; j++)
	{
		for(i = 0; i < X_SIZE; i++)
		{
			if(tm[i][j] == 0)
			{
				lineComplete=false;
			}
		}

		if(lineComplete)
		{
			lines[cntLines++] = j;
		}

		lineComplete = true;
	}

	removeLines(lines);
	score = score + cntLines * 100;
	printf("score: %d\n", score);
}

void printMatrix(int arr[PIECE_H][PIECE_H])
{
	int i,j;
	for(i=0; i < 4; i++){
		for(j=0; j < 4; j++){
			printf("%d ", arr[i][j]);
		}
		printf("\n");
	}
	printf("--end--\n");
}

void rotatePiece(int arr[PIECE_H][PIECE_H])
{
	int i, j, tmp;
	for(i = 0; i < PIECE_H / 2; i++)
		for(j = i; j < PIECE_H - 1 - i; j++)
		{
			//printf("%d %d\n", i, j);
			tmp = arr[i][j];
			arr[i][j] = arr[PIECE_H - 1 - j][i];
			arr[PIECE_H - 1 - j][i] = arr[PIECE_H - 1 - i][PIECE_H - 1 - j];
			arr[PIECE_H - 1 - i][PIECE_H - 1 - j] = arr[j][PIECE_H - 1 - i];
			arr[j][PIECE_H - 1 - i] = tmp;
		}

}

void assignNewPiece()
{
	int selectedPiece = random() % 7;

	printf("selectedPiece: %d\n", selectedPiece);

	int i,j;
	for(i = 0; i < PIECE_H; i++)
		for(j = 0; j < PIECE_H; j++)
			currentPiece[j][i] = i < PIECE_H - 1 ? pieces[selectedPiece][j][i] : 0;
}

void initTetris()
{
	int i, j;
	for(i = 0; i < X_SIZE; i++)
		for(j = 0; j < Y_SIZE; j++)
			tm[i][j] = 0;

	//tm[1][1] = 1;
	//tm[0][17] = 1;
	//tm[9][17] = 1;
	//tm[9][0] = 1;
	for(i = 0; i < X_SIZE - 1; i++){
	tm[i][17] = 1;
	tm[i][16] = 1;
	tm[i][15] = 1;
	tm[i][14] = 1;
	}
	assignNewPiece();
	score = 0;
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

bool xCollision(int x)
{
	printf("x: %d\n", x);
	int i,j;
	for(i = 0; i < PIECE_W; i++)
		for(j = 0; j < PIECE_H; j++)
			if(currentPiece[j][i] == 1 &&
					(j + x >= X_SIZE ||
					 j + x < 0 ||
					 currentPiece[j][i] == tm[j + x][i + cnt_y]))
				return true;

	return false;
	
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
			if(!xCollision(cnt_x - 1))
			{
				cnt_x--;
			}
		    	break;

                    case SDLK_RIGHT:
			if(!xCollision(cnt_x + 1))
			{
				cnt_x++;
			}
                    	break;
		    case SDLK_UP:
			printMatrix(currentPiece);
			rotatePiece(currentPiece);
			printMatrix(currentPiece);
			break;
		    case SDLK_DOWN:
			initTetris();
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
	for(i = 0; i < PIECE_H && collided == false; i++)
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
		for(i = 0; i < PIECE_H; i++)
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
		scoring();
		posMovingPieceX = cnt_x * 16;
		posMovingPieceY = cnt_y * 16;
		pieceCnt = currentPieceCnt;
		//printf("%d\n", destR.y);
	}
}

void renderMovingPiece()
{
	int i,j;
	for(i = 0; i < PIECE_H; i++)
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
