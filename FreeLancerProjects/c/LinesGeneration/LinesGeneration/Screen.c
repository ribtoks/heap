#include <stdio.h>
#include "ScreenInterface.h"
#include "PointStruct.h"

#define ScreenWidth 75
#define ScreenHeight 41

#define CenterX 37
#define CenterY 20

unsigned char Screen[ScreenHeight][ScreenWidth];

void ClearScreen()
{
	int i = 0;
	int j = 0;
	
	for (; i < ScreenHeight; ++i)
		for (j = 0; j < ScreenWidth; ++j)
			Screen[i][j] = 250;

	for (i = 0; i < ScreenHeight; ++i)
		Screen[i][CenterX] = '|';

	for (j = 0; j < ScreenWidth; ++j)
		Screen[CenterY][j] = '-';

	Screen[CenterY][CenterX] = '+';
}

void InitScreen()
{
	ClearScreen();
}

void SetPoint(int x, int y, unsigned char c)
{
	Screen[y][x] = c;
}

void PrintScreen()
{
	int i = 0;
	int j = 0;
	
	for (; i < ScreenHeight; ++i)
	{
		for (j = 0; j < ScreenWidth; ++j)
			printf("%c", Screen[i][j]);
		printf("\n");
	}
	printf("\n");
}


int IsPointOk(int x, int y)
{
	if (x >= 0  &&  x < ScreenWidth)
		if (y >= 0  &&  y < ScreenHeight)
			return 1;

	return 0;
}

void TransformPoint(struct Point* p)
{
	p->x += CenterX;
	p->y = CenterY - p->y;
}
