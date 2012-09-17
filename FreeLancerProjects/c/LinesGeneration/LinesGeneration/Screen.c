#include <stdio.h>
#include "ScreenInterface.h"
#include "PointStruct.h"

// ширина и висота псевдоекрана
#define ScreenWidth 75
#define ScreenHeight 41

#define CenterX 37
#define CenterY 20

// сам псевдоекран
unsigned char Screen[ScreenHeight][ScreenWidth];

// Функция очистки псевдоекрана
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

// инициализация екрана
void InitScreen()
{
	ClearScreen();
}

/*
	Функция устанавливает пиксель 
		(если у него коректные координаты)
	как зарисованный

	Входные параметры:

	x, y - координаты точки, которую надо зарисовать
	c - символ, которым надо зарисовать точку
*/
void SetPoint(int x, int y, unsigned char c)
{
	Screen[y][x] = c;
}

/*
	Выводит псевдоекран на стандартный
	поток stdout
*/
void PrintScreen()
{
	int i = 0;
	int j = 0;
	
	// обычный цикл вывода 2-х мерного масива 
	// в поток вывода
	for (; i < ScreenHeight; ++i)
	{
		for (j = 0; j < ScreenWidth; ++j)
			printf("%c", Screen[i][j]);
		printf("\n");
	}
	printf("\n");
}


/*
	Функция проверяет, лежит ли точка
	в границах псевдоекрана

	1 - если в границах екрана
	0 - в противном случае
*/
int IsPointOk(int x, int y)
{
	// проверка на приналежность точки екрану
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