#pragma once

#include "PointStruct.h"
/*
	Проводит инициализацию екрана
*/
void InitScreen();


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
	Функция устанавливает пиксель 
		(если у него коректные координаты)
	как зарисованный

	Входные параметры:

	x, y - координаты точки, которую надо зарисовать
	c - символ, которым надо зарисовать точку
*/
void SetPoint(int x, int y, unsigned char c);


/*
	Функция проверяет, лежит ли точка
	в границах псевдоекрана

	1 - если в границах екрана
	0 - в противном случае
*/
int IsPointOk(int x, int y);

void TransformPoint(struct Point* p);