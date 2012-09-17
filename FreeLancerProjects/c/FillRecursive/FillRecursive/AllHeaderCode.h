#pragma once

/*
	Простая структура, которая представляет
	точку на псевдомониторе
*/
struct Point
{
	int x;
	int y;
};

/*
	Проводит инициализацию екрана
*/
void InitializeScreen();


/*
	Очищает псевдоекран
*/
void ClearScreen();


/*
	Выводит псевдоекран на стандартный
	поток stdout
*/
void PrintScreen();

/*
	Функция проверяет, лежит ли точка
	в границах псевдоекрана

	1 - если в границах екрана
	0 - в противном случае
*/
int IsPointOk(int x, int y);

void TransformPoint(struct Point* p);

int IsFilled(int x, int y);

void Fill(int x, int y);

void DrawLine(struct Point p1, struct Point p2);

int IsPointInsidePolygon (struct Point p[], int Number, int x, int y);

void FillPolygon(struct Point Points[], int Count, int x, int y);