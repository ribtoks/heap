#include "graphlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


// Структура для хранения одной фигуры
typedef struct
{
	pixel *points; // Указатель на массив точек фигуры
	int count;    // Колличество точек в массиве точек фигуры
} Figure;

// Массив точек для первой фигуры
pixel Points1[MAXARR] = 
{
	{ 20.0, 20.0 },
	{ 30.0, 20.0 },
	{ 40.0, 40.0 },
	{ 30.0, 40.0 },
	{ 20.0, 20.0 }
};

// Массив точек для второй фигуры
pixel Points2[MAXARR] = 
{
	{ 20.0, 20.0 },
	{ 30.0, 32.0 },
	{ 20.0, 32.0 },
	{ 20.0, 20.0 }
};

//Массив точек для третей фигуры
pixel Points3[MAXARR] = 
{
	{ 20.0, 20.0 },
	{ 32.0, 24.0 },
	{ 28.0, 30.0 },
	{ 32.0, 36.0 },
	{ 34.0, 42.0 },
	{ 26.0, 46.0 },
	{ 26.0, 40.0 },
	{ 20.0, 20.0 }
};

// Массив фигур
Figure figures[] = 
{
	{ Points1, 4 },
	{ Points2, 3 },
	{ Points3, 7}
};

//пользовательская фигура
Figure custom;
//и точки для нее
pixel CustPoints[MAXARR];

// Колличество фигур
int fquantity = 3;

// Функция для выбора одной фигуры из массива фигур
// запрашивает у пользователя номер фигуры в промежутке от 1 до fquantity
// и возвращает указатель на соответствующую фигуру
Figure* chooseFigure()
{
	int number;
	int iter = 0;
	int i,j;
	int vert;
	float x,y;
	/*
	Запрашиваем у пользователя номер фигуры до тех пор
	пока он не введёт число от 1 до fquantity.
	Или не запросит выход.
	В случае неправильного ввода виводим подсказку.
	*/
	do
	{
		if (iter > 0)
			printf("Please make sure you entered valid figure number![1..%d]\r\nOr exit code %d",fquantity,fquantity+2);
		printf("Enter:\r\n");
		for (i = 0;i<fquantity;++i)
			printf("%d)For sample figure with %d verteces\r\n",i+1,figures[i].count);
		printf("%d)Custom poligon\r\n",fquantity+1);
		printf("%d) Exit.\r\n",fquantity+2);
		scanf("%d", &number);
		++iter;
	}
	while (number > fquantity+2 || number <= 0);

	/*
	при необходимости вводим пользовательскую фигуру
	она будет сохранена на протяжении роботы программы
	*/
	if (number == fquantity+1)
	{
		printf("input the quantity of verteces: ");
		scanf("%d",&vert);
		custom.count = vert;
		printf("now input %d vertesis like this: \"1.0 1.0\" \r\n",vert);
		for (j = 0; j < vert; ++j )
		{
			scanf("%f %f",&x,&y);
			CustPoints[j].x = x;
			CustPoints[j].y = y;
		}
		CustPoints[vert]=CustPoints[0];
		custom.points = CustPoints;
		return &custom;
	}
		
	//Проверяем не был ли нажат код выхода - если был возвращаем ноль
	if (number == fquantity+2)
		return 0;
	else
		// Возвращаем указатель на фигуру с номером number
		return &figures[number-1];
}

//Символ пустого пикселя
char EmptyPoint = '.';

int main (void)
{
	int i;
	char screen[SHEIGHT][SWIDTH] = {0};
	Figure* instance;

	// Выбор фигуры	и проверка на выход
	while(instance = chooseFigure())
	{
		// Заполняем массив для рисования пустыми пикселями
		for (i = 0; i < SHEIGHT; i++)
			memset(screen[i], EmptyPoint, SWIDTH-1);

		

		// Вызов функции заливки из библиотеки
		Fill(instance->points, instance->count, screen);

		// Копирование символов в консоль
		for (i = 0; i < SHEIGHT; i++)
			printf("%s\n", screen[i]);
	}

	//Ожидаем нажатия любой кнопки перед выходом
	system("PAUSE");
}
