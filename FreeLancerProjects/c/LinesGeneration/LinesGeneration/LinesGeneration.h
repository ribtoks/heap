#pragma once
#include "PointStruct.h"
/*
	Генерация отрезка на екране (псевдографика) с помощью
	алгоритма ЦДА - DDC (Digital Differential Analyzer)

	Входные параметры:

	p1 - начальная точка линии
	p2 - конечная точка линии

	Результат - количество точек в масиве
*/
int GenerateLineDDC(struct Point p1, struct Point p2, struct Point* resultArray);

/*
	Генерация отрезка на екране (псевдографика) с помощью
	алгоритма Брезенхэма

	Входные параметры:

	p1 - начальная точка линии
	p2 - конечная точка линии

	Результат - количество точек в масиве
*/
int GenerateLineBresenham(struct Point p1, struct Point p2, struct Point* resultArray);