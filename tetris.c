#include "tetris.h"
#include "pieces.h"
#include <stdlib.h>
#include <math.h>

int levelDelay[] = {887, 820, 753, 686, 619, 552, 469, 368, 285, 184, 167, 
	151, 134, 117, 100, 100, 84, 84, 67, 67, 50};
int pointsForLine[] = {40, 100,	300, 1200};
int level;
int lineCounter;
int lineCounterForLevel;

Uint32 pieceCnt = 0;
int tm[X_SIZE][Y_SIZE];
//large size to be able to rotate the piece
int currentPiece[PIECE_H][PIECE_H];
int nextPiece[PIECE_H][PIECE_H];
int testRotatedPiece[PIECE_H][PIECE_H];

static int screenPosX = SCREEN_POS_X * SCREEN_FACTOR;
static int screenPosY = SCREEN_POS_Y * SCREEN_FACTOR;

int cnt_y  = 0;
int cnt_x  = 5;
int posMovingPieceX;
int posMovingPieceY;
int score;
bool afterReset;

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
		playEScore();
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
		delay(30);
		render();
	}
	delay(200);
}

void initTetris()
{
	int i, j;
	pieceCnt = getTicks();

	for(i = 0; i < X_SIZE; i++)
		for(j = 0; j < Y_SIZE; j++)
			tm[i][j] = 0;

	score = 0;
	lineCounter = 0;
	level = 0;
	afterReset = true;
	assignNewPiece();
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

void onLeftKey()
{
	if(!xCollision(cnt_x - 1, currentPiece))
	{
		cnt_x--;
	}
}

void onRightKey()
{
	if(!xCollision(cnt_x + 1, currentPiece))
	{
		cnt_x++;
	}

}

void onUpKey()
{
	memcpy(testRotatedPiece, currentPiece, PIECE_H * PIECE_H * sizeof(int));
	rotatePiece(testRotatedPiece);
	if(!xCollision(cnt_x, testRotatedPiece))
	{
		memcpy(currentPiece, testRotatedPiece, PIECE_H * PIECE_H * sizeof(int));
		playERotate();
	}
}

void onDownKey()
{
	int yPos = cnt_y + 1;
	while(!checkColision(yPos)){
		yPos++;
	}
}

void onSpaceKey()
{
	initTetris();
}

void handleEvents()
{
	handlePlatformEvents(&onLeftKey, &onRightKey, &onUpKey, &onDownKey, &onSpaceKey);
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
		playEPiecesDown();
	}

	return collided;
}

void update()
{
	scoring();
	Uint32 currentPieceCnt = getTicks();
	if((currentPieceCnt - pieceCnt) > levelDelay[level])
	{
		cnt_y++;
		checkColision(cnt_y);
		pieceCnt = currentPieceCnt;
	}
}

void renderMovingPiece()
{
	struct PixelLoc pixelLoc;

	int i,j;
	for(i = 0; i < PIECE_H; i++)
		for(j = 0; j < PIECE_H; j++)
			if(currentPiece[j][i])
			{
				pixelLoc.x = j * pixelSize + cnt_x * pixelSize + screenPosX;
				pixelLoc.y = i * pixelSize + cnt_y * pixelSize + screenPosY;
				renderPixels(pixelLoc);
			}
}

void renderNumbers(int value, int numberPosX, int numberPosY, int numberOfDigits)
{
	renderGraphicNumbers(value, numberPosX, numberPosY, numberOfDigits);
}

void renderNextPiece()
{
	struct PixelLoc pixelLoc;
	int i,j;
	for(i = 0; i < PIECE_H; i++)
		for(j = 0; j < PIECE_H; j++)
			if(nextPiece[j][i])
			{
				pixelLoc.x = pixelSize * j + 109 * SCREEN_FACTOR;
				pixelLoc.y = pixelSize * i + 93 * SCREEN_FACTOR;
				renderPixels(pixelLoc);
			}
}

void render()
{
	struct PixelLoc pixelLoc;
	renderClear();
	
	renderBackground();
	
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
				pixelLoc.x = i * pixelSize + screenPosX;
				pixelLoc.y = j * pixelSize + screenPosY;
				renderPixels(pixelLoc);
			}

	renderPresent();
}

void clean()
{
	cleanDisplay();
}

bool running()
{
	return isRunning;
}
