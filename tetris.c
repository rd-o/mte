#include "tetris.h"
#include "pieces.h"
#include <stdlib.h>
#include <math.h>

#define X_SIZE 10
#define Y_SIZE 18
#define WINDOW_W 1
#define WINDOW_H 1
#define PIECE_DELAY 400
#define SCREEN_FACTOR 4
#define PIXEL_ORIGINAL_SIZE 8
#define SCREEN_POS_X 11
#define SCREEN_POS_Y 0
#define SCORE_NUMBER_POS_X 109
#define SCORE_NUMBER_POS_Y 27
#define LEVEL_NUMBER_POS_X 127
#define LEVEL_NUMBER_POS_Y 50
#define LINES_NUMBER_POS_X 109
#define LINES_NUMBER_POS_Y 72
#define SCORE_DIGITS 5
#define LEVEL_DIGITS 2
#define LINES_DIGITS 5

int levelDelay[] = {887, 820, 753, 686, 619, 552, 469, 368, 285, 184, 167, 
	151, 134, 117, 100, 100, 84, 84, 67, 67, 50};
int pointsForLine[] = {40, 100,	300, 1200};
int level;
int lineCounter;
int lineCounterForLevel;

SDL_Texture* numbers;
SDL_Texture* pixels;
SDL_Texture* background;
SDL_Rect srcR, destR, staticPixel, backgroundRect;
SDL_Rect pixelRect, pixelTextureRect;
//sound effects
Mix_Chunk *ePiecesDown = NULL;
Mix_Chunk *eRotate = NULL;
Mix_Chunk *eScore = NULL;

Uint32 pieceCnt = 0;
int tm[X_SIZE][Y_SIZE];
//large size to be able to rotate the piece
int currentPiece[PIECE_H][PIECE_H];
int nextPiece[PIECE_H][PIECE_H];
int testRotatedPiece[PIECE_H][PIECE_H];

static int pixelSize = SCREEN_FACTOR * PIXEL_ORIGINAL_SIZE;
static int screenPosX = SCREEN_POS_X * SCREEN_FACTOR;
static int screenPosY = SCREEN_POS_Y * SCREEN_FACTOR;

bool isRunning = false;
int cnt_y  = 0;
int cnt_x  = 5;
SDL_Window *window;
SDL_Renderer *renderer;
int posMovingPieceX;
int posMovingPieceY;
int score;
bool afterReset;

bool loadMedia()
{
	bool success = true;

	if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
	{
		printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
	}

	ePiecesDown = Mix_LoadWAV("assets/effects/ePiecesDown.wav");
	if(ePiecesDown == NULL )
	{	
        	printf( "Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	eRotate = Mix_LoadWAV("assets/effects/eRotate.wav");
	if(eRotate == NULL )
	{	
        	printf( "Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	eScore = Mix_LoadWAV("assets/effects/eScore.wav");
	if(eScore == NULL )
	{	
        	printf( "Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	return success;
}

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
	bool scoreAdded = false;
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
			scoreAdded = true;
		}

		lineComplete = true;
	}

	if(scoreAdded)
	{
		removeLines(lines);
		lineCounterForLevel = lineCounterForLevel + cntLines;
		lineCounter = lineCounter + lineCounterForLevel;
		int points = pointsForLine[cntLines - 1] * (level + 1);

		score = score + cntLines * points;

		if(level <= 8)
		{
			if(lineCounterForLevel >= 10)
			{
				level++;
				lineCounterForLevel = lineCounterForLevel - 10;
			}
		}else if(level > 8 && level <= 20)
		{
			if(lineCounterForLevel >= 20)
			{
				level++;
				lineCounterForLevel = lineCounterForLevel - 20;
			}
		}
		Mix_PlayChannel(-1, eScore, 0);
		printf("score: %d\n", score);
	}
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

void createNewPiece(int piece[PIECE_H][PIECE_H]) 
{
	int selectedPiece = random() % 7;

	int i,j;
	for(i = 0; i < PIECE_H; i++)
		for(j = 0; j < PIECE_H; j++)
			piece[j][i] = i < PIECE_H - 1 ? pieces[selectedPiece][j][i] : 0;

}

void assignNewPiece()
{
	if(afterReset)
	{
		createNewPiece(currentPiece);
		createNewPiece(nextPiece);
		afterReset = false;
	}else
	{
		memcpy(currentPiece, nextPiece, PIECE_H * PIECE_H * sizeof(int));
		createNewPiece(nextPiece);
	}
}

void terminateGame(){
	int i, j;

	for(j = Y_SIZE - 1; j >= 0; j--)
	{
		for(i = 0; i < X_SIZE; i++){
			tm[i][j] = 1;
		}
		SDL_Delay(30);
		render();
	}
	SDL_Delay(200);
}

void initTetris()
{
	int i, j;

	for(i = 0; i < X_SIZE; i++)
		for(j = 0; j < Y_SIZE; j++)
			tm[i][j] = 0;

	score = 0;
	lineCounter = 0;
	level = 0;
	afterReset = true;
	assignNewPiece();
}

int init(const char* title, int width, int height, bool fullscreen)
{
	int flags = 0;
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, &window, &renderer);
	isRunning = true;

	SDL_Surface* tmpSurface = IMG_Load("assets/textures/numbers.png");
	numbers = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load("assets/textures/pixels.png");
	if(!tmpSurface) {
		printf("IMG_Load: %s\n", IMG_GetError());
	}
	pixels = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load("assets/textures/background.png");
	background = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	pieceCnt = SDL_GetTicks();

	backgroundRect.x = 0;
	backgroundRect.y = 0;
	backgroundRect.h = 144 * 4;
	backgroundRect.w = 160 * 4;
	loadMedia();
	initTetris();

	return 0;
}

bool xCollision(int x, int piece[PIECE_H][PIECE_H])
{
	printf("x: %d\n", x);
	int i,j;
	for(i = 0; i < PIECE_W; i++)
		for(j = 0; j < PIECE_H; j++)
			if(piece[j][i] == 1 &&
					(j + x >= X_SIZE ||
					 j + x < 0 ||
					 piece[j][i] == tm[j + x][i + cnt_y]))
				return true;

	return false;
	
}

void handleEvents()
{
	SDL_Event event;
	int yPos;

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
			if(!xCollision(cnt_x - 1, currentPiece))
			{
				cnt_x--;
			}
		    	break;

                    case SDLK_RIGHT:
			if(!xCollision(cnt_x + 1, currentPiece))
			{
				cnt_x++;
			}
                    	break;
		    case SDLK_UP:
			memcpy(testRotatedPiece, currentPiece, PIECE_H * PIECE_H * sizeof(int));
			rotatePiece(testRotatedPiece);
			if(!xCollision(cnt_x, testRotatedPiece))
			{
				memcpy(currentPiece, testRotatedPiece, PIECE_H * PIECE_H * sizeof(int));
				Mix_PlayChannel(-1, eRotate, 0);
			}
			break;
		    case SDLK_DOWN:
			yPos = cnt_y + 1;
			printf("=================\n");
			while(!checkColision(yPos)){
				yPos++;
			}
			break;
		    case SDLK_SPACE:
			initTetris();
			break;
		}
	}
}

bool checkColision(int cnt_y_local)
{
	bool collided = false;
	int i,j;
	for(i = 0; i < PIECE_H && collided == false; i++)
		for(j = 0; j < PIECE_H && collided == false; j++)
		{
			if(currentPiece[j][i] == tm[j + cnt_x][i + cnt_y_local] && currentPiece[j][i] == 1) 
			{
				collided = true;
			}

			if(i + cnt_y_local == Y_SIZE && currentPiece[j][i] == 1)
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
					printf("x: %d, y: %d\n", i + cnt_x, j + cnt_y_local - 1);
					tm[j + cnt_x][i + cnt_y_local - 1] = 1; 
					if(i + cnt_y_local - 1 < 2)
					{
						terminateGame();
						initTetris();
						return collided;
					}
				}
		printf("--- end collision ---\n");
		cnt_x = 5;
		cnt_y = 0;
		assignNewPiece();
		Mix_PlayChannel(-1, ePiecesDown, 0);
	}

	return collided;
}

void update()
{
	destR.h = pixelSize;
	destR.w = pixelSize;
	staticPixel.h = pixelSize;
	staticPixel.w = pixelSize;

	scoring();
	Uint32 currentPieceCnt = SDL_GetTicks();
	if((currentPieceCnt - pieceCnt) > levelDelay[level])
	{
		cnt_y++;
		checkColision(cnt_y);
		pieceCnt = currentPieceCnt;
	}
}

void renderMovingPiece()
{
	pixelTextureRect.x = 0;
	pixelTextureRect.y = 0;
	pixelTextureRect.w = 8;
	pixelTextureRect.h = 8;

	int i,j;
	for(i = 0; i < PIECE_H; i++)
		for(j = 0; j < PIECE_H; j++)
			if(currentPiece[j][i])
			{
				destR.x = j * pixelSize + cnt_x * pixelSize + screenPosX;
				destR.y = i * pixelSize + cnt_y * pixelSize + screenPosY;
				SDL_RenderCopy(renderer, pixels, &pixelTextureRect, &destR);
			}
}

void renderNumbers(int value, int numberPosX, int numberPosY, int numberOfDigits)
{
	SDL_Rect numberSelectorRect, numberRect;
	numberSelectorRect.w = 5;
	numberSelectorRect.h = 7;

	numberRect.w = 5 * SCREEN_FACTOR;
	numberRect.h = 7 * SCREEN_FACTOR;
	numberRect.y = numberPosY * SCREEN_FACTOR;

	int n = numberOfDigits - 1;
	int res;
	int tmpValue = value;
	while(n >= 0)
	{
		res = tmpValue / (pow(10, n));
		tmpValue = tmpValue - res * pow(10, n);
		numberSelectorRect.x = res * 5;
		numberSelectorRect.y = 0;
		numberRect.x = numberPosX * SCREEN_FACTOR + (numberOfDigits - n - 1) * 6 * SCREEN_FACTOR;
		SDL_RenderCopy(renderer, numbers, &numberSelectorRect, &numberRect);
		n--;
	}
}

void renderNextPiece()
{
	pixelTextureRect.x = 0;
	pixelTextureRect.y = 0;
	pixelTextureRect.w = 8;
	pixelTextureRect.h = 8;

	int i,j;
	for(i = 0; i < PIECE_H; i++)
		for(j = 0; j < PIECE_H; j++)
			if(nextPiece[j][i])
			{
				destR.x = pixelSize * j + 109 * SCREEN_FACTOR;
				destR.y = pixelSize * i + 93 * SCREEN_FACTOR;
				SDL_RenderCopy(renderer, pixels, &pixelTextureRect, &destR);
			}
}

void render()
{
	SDL_RenderClear(renderer);
	
	pixelTextureRect.x = 0;
	pixelTextureRect.y = 0;
	pixelTextureRect.w = 8;
	pixelTextureRect.h = 8;
	
	SDL_RenderCopy(renderer, background, NULL, &backgroundRect);
	
	renderNumbers(score, SCORE_NUMBER_POS_X, SCORE_NUMBER_POS_Y, SCORE_DIGITS);
	renderNumbers(level, LEVEL_NUMBER_POS_X, LEVEL_NUMBER_POS_Y, LEVEL_DIGITS);
	renderNumbers(lineCounter, LINES_NUMBER_POS_X, LINES_NUMBER_POS_Y, LINES_DIGITS);
	renderNextPiece();

	renderMovingPiece();

	int i,j;
	for(i = 0; i < X_SIZE; i++)
		for(j = 0; j < Y_SIZE; j++)
			if(tm[i][j])
			{
				staticPixel.x = i * pixelSize + screenPosX;
				staticPixel.y = j * pixelSize + screenPosY;
				SDL_RenderCopy(renderer, pixels, &pixelTextureRect, &staticPixel);
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
