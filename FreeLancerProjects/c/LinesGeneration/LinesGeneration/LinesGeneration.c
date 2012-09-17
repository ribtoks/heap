#include <math.h>
#include <stdlib.h>

#include "LinesGeneration.h"
#include "ScreenInterface.h"
#include "PointStruct.h"

#define BresenhamCoordChar 42
#define DDCCoordChar 4

/*
	Функция для нахождения максимума 
	2-х целых чисел
*/
int Max(int a, int b)
{
	if (a > b)
		return a;
	return b;
}

/*
	Функция нахождения знака целого числа
*/
int sign(int a)
{
	if (a > 0)
		return 1;

	if (a < 0)
		return -1;

	return 0;
}

/*
	Генерация отрезка на екране (псевдографика) с помощью
	алгоритма ЦДА - DDC (Digital Differential Analyzer)

	Входные параметры:

	p1 - начальная точка линии
	p2 - конечная точка линии

	Результат - количество точек в масиве
*/
int GenerateLineDDC(struct Point p1, struct Point p2, struct Point* resultArray)
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
		//SetPoint((int)x, (int)y, DDCCoordChar);
		resultArray[currIndex].x = x;
		resultArray[currIndex].y = y;
		++currIndex;

		// инкрементация всех параметров
		x += dx;
		y += dy;
		++i;
    }

	return currIndex;
}


/*
	Генерация отрезка на екране (псевдографика) с помощью
	алгоритма Брезенхэма

	Входные параметры:

	p1 - начальная точка линии
	p2 - конечная точка линии

	Результат - количество точек в масиве
*/
int GenerateLineBresenham(struct Point p1, struct Point p2, struct Point* resultArray)
{
	int dx, dy;
	int sx, sy;

	int x, y;
	int i;
	int d1, d2;

	int currIndex = 0;

	/*
		Знак D используется как критерий для выбора ближайшей растровой точки. 
		Если D < 0, то точное Y-значение округляется до меньшего последнего 
		целочисленного значения Y, т.е. Y-координата не меняется по сравнению с 
		предыдущей точкой. В противном случае Y увеличивается на 1.
	*/
	int d;

	// длины смещений по X и по Y
	dx = abs(p2.x - p1.x);
	dy = abs(p2.y - p1.y);

	// определение знака напрямления по оси X
	if (p2.x >= p1.x)
		sx = 1;
	else
		sx = -1;

	// определение знака напрямления по оси Y
	if (p2.y >= p1.y)
		sy = 1;
	else
		sy = -1;

	//проверка угла наклона линии
    if (dy <= dx)
    {
		//деление на два для определения углового коэффициента
		d = (dy << 1) - dx;

		d1 = dy << 1;
		d2 = (dy - dx) << 1;

		// рисование начальной точки линии
		//SetPoint(p1.x, p1.y, BresenhamCoordChar);
		resultArray[currIndex].x = p1.x;
		resultArray[currIndex].y = p1.y;
		++currIndex;

		// цикл вывода линии на экран
		for (x = p1.x + sx, y = p1.y, i = 1; i <= dx; i++, x += sx)
		{
			// Если d < 0 значение y не меняется по сравнению с предыдущей точкой, иначе y увеличивается
			if (d > 0)
			{
				d += d2;
				y += sy;
			}
			else 
				d += d1;

			// вывод на экран очередной точки линии
			//SetPoint(x, y, BresenhamCoordChar);		     
			resultArray[currIndex].x = x;
			resultArray[currIndex].y = y;
			++currIndex;
		}
	}
    else
    {
        d = (dx << 1) - dy;

        d1 = dx << 1;
        d2 = (dx - dy) << 1;

		// рисование начальной точки линии
        //SetPoint(p1.x, p1.y, BresenhamCoordChar);
		resultArray[currIndex].x = p1.x;
		resultArray[currIndex].y = p1.y;
		++currIndex;

		// цикл вывода линии на экран
        for (x = p1.x, y = p1.y + sy, i = 1;  i <= dy;  i++, y += sy)
        {
			// Если d < 0 значение y не меняется по сравнению с предыдущей точкой, иначе y увеличивается
            if (d > 0)
            {
                d += d2;
                x += sx;
            }
            else 
				d += d1;

			// вывод на экран очередной точки линии
            //SetPoint(x, y, BresenhamCoordChar);
			resultArray[currIndex].x = x;
			resultArray[currIndex].y = y;
			++currIndex;
		}
	}
	return currIndex;
}