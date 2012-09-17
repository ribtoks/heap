#include "AllHeaderCode.h"
#include <math.h>
#include <stdio.h>


// ширина и висота псевдоекрана
#define ScreenWidth 75
#define ScreenHeight 41

#define CenterX 37
#define CenterY 20

// сам псевдоекран
unsigned char ScreenMatrix[ScreenHeight][ScreenWidth];

// Функция очистки псевдоекрана
void ClearScreen()
{
	int i = 0;
	int j = 0;
	
	for (; i < ScreenHeight; ++i)
		for (j = 0; j < ScreenWidth; ++j)
			ScreenMatrix[i][j] = 250;

	for (i = 0; i < ScreenHeight; ++i)
		ScreenMatrix[i][CenterX] = '|';

	for (j = 0; j < ScreenWidth; ++j)
		ScreenMatrix[CenterY][j] = '-';

	ScreenMatrix[CenterY][CenterX] = '+';
}

// инициализация екрана
void InitializeScreen()
{
	ClearScreen();
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
			printf("%c", ScreenMatrix[i][j]);
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

// проверяет, закрашена ли точка
int IsFilled(int x, int y)
{
	if (ScreenMatrix[y][x] == '*')
		return 1;
	return 0;
}

// закрашивает точку
void Fill(int x, int y)
{
	ScreenMatrix[y][x] = '*';
}

void TransformPoint(struct Point* p)
{
	p->x += CenterX;
	p->y = CenterY - p->y;
}

// удоенная площадь треугольника
int TriangleSquare(struct Point p1, struct Point p2, struct Point p3)
{
	return abs(p1.x*(p2.y - p3.y) - p1.y*(p2.x - p3.x) + (p2.x*p3.y - p3.x * p2.y));
}

int Max(int a, int b)
{
	if (a > b)
		return a;
	return b;
}

int sign(int a)
{
	if (a > 0)
		return 1;

	if (a < 0)
		return -1;

	return 0;
}

void DrawLine(struct Point p1, struct Point p2)
{
	int length, i;
	double dx, dy;
	int currIndex = 0;

	double x, y;

	// длина как максимум смещений по X и по Y
	length = Max(abs(p1.x - p2.x), abs(p1.y - p2.y));

	// полагаем большее из приращений dx или dy равными единице растра
	dx = (double)(p2.x - p1.x) / length;
	dy = (double)(p2.y - p1.y) / length;

	// округляем величины, а не отбрасываем дробную часть
	// использование знаковой функции делает алгоритм пригодным для всех квадрантов
	x = p1.x + 0.5*sign(dx);
	y = p1.y + 0.5*sign(dy);

	// начало основного цикла
	i = 1;
	while (i <= length)
	{
		// рисуем текущую точку
		Fill((int)x, (int)y);		
		++currIndex;

		// инкрементация всех параметров
		x += dx;
		y += dy;
		++i;
    }

	return currIndex;
}


/*
	Функция, реализующая проверку принадлежности данной точки 
	данному многоугольнику произвольного вида

	Входные параметры:

	p - указатель на массив пар целочисленных координат вершин многоугольника, а именно, на массив структур вида
	Number - число вершин многоугольника
	x - целочисленное значение координаты X заданной точки
	y - целочисленное значение координаты Y заданной точки

	Возвращает 1, если точка принадлежит многоугольнику
	или 0 в противном случае
*/
int IsPointInsidePolygon (struct Point p[], int Number, int x, int y)
{
	int i1, i2, n, N, S, S1, S2, S3, flag;
	struct Point point;
	N = Number;

	point.x = x;
	point.y = y;

	for (n = 0; n < N; ++n)
	{
		flag = 0;
		i1 = n < N-1 ? n + 1 : 0;

		/*
			Для принадлежности точки выпуклому многоугольнику вершины последовательно нумеруются от 1 до n. 
			Вершина с условным номером 1 соединяется отрезками со всеми другими вершинами. Образуются (n-2) 
			треугольника с вершинами {1, (i-1), i}, где 3 ≤ i ≤ n. Осуществляется проверка принадлежности 
			заданной точки хотя бы одному из этих треугольников.
		*/
		while (flag == 0)
		{
			i2 = i1 + 1;
			if (i2 >= N)
				i2 = 0;

			if (i2 == (n < N - 1 ? n + 1 : 0))
				break;

			// обшая площадь
			S = TriangleSquare(p[i1], p[i2], p[n]);

			// площади каждого "подтреугольника"
			S1 = TriangleSquare(p[i1], p[i2], point);
			S2 = TriangleSquare(p[n], p[i1], point);
			S3 = TriangleSquare(p[n], p[i2], point);
			
			if (S == S1 + S2 + S3)
			{
				// нашли треугольник, в котором лежит наша точка
				flag = 1;
				break;
			}
			i1 = i1 + 1;
			if (i1 >= N)
				i1 = 0;
		}
		if (flag == 0)
			break;
	}
 
	return flag;
}


void FillPolygon(struct Point Points[], int Count, int x, int y)
{
	// если точка лежит в области "екрана"
	if (IsPointOk(x, y))
		// если она еще не закрашеная
		if (!IsFilled(x, y))
			// если она принадлежит полигону
			if (IsPointInsidePolygon(Points, Count, x, y))
			{
				// закрасить ее
				Fill(x, y);
				
				// закрасить соседние 4 точки
				FillPolygon(Points, Count, x - 1, y);
				FillPolygon(Points, Count, x, y + 1);
				FillPolygon(Points, Count, x + 1, y);
				FillPolygon(Points, Count, x, y - 1);
			}
}