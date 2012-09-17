#include <stdio.h>
#include <ctype.h>
#include <conio.h>
#include "AllHeaderCode.h"

#define MAX_EDGE_COUNT 100

struct Point Points[MAX_EDGE_COUNT];
int PointsSize = 0;

struct Point sample1[3];
int sample1Size = 3;

struct Point sample2[4];
int sample2Size = 4;

struct Point sample3[5];
int sample3Size = 5;

struct Point zeroPoint;

struct Point ReadOnePoint()
{
	struct Point p;
	printf("Enter coordinates of point like \"x y\" >: ");
	scanf("%d %d", &p.x, &p.y);

	TransformPoint(&p);

	getchar();

	return p;
}

void InitSamples()
{
	int i = 0;

	TransformPoint(&zeroPoint);

	sample1[0].x = -5;
	sample1[0].y = -3;
	sample1[1].x = 5;
	sample1[1].y = -3;
	sample1[2].x = 0;
	sample1[2].y = 5;

	for (i = 0; i < sample1Size; ++i)
		TransformPoint(&sample1[i]);

	sample2[0].x = -2;
	sample2[0].y = -2;
	sample2[1].x = -2;
	sample2[1].y = 2;
	sample2[2].x = 2;
	sample2[2].y = 2;
	sample2[3].x = 2;
	sample2[3].y = -2;

	for (i = 0; i < sample2Size; ++i)
		TransformPoint(&sample2[i]);

	sample3[0].x = -2;
	sample3[0].y = -3;
	sample3[1].x = -5;
	sample3[1].y = 2;
	sample3[2].x = 0;
	sample3[2].y = 7;
	sample3[3].x = 5;
	sample3[3].y = 2;
	sample3[4].x = 2;
	sample3[4].y = -3;

	for (i = 0; i < sample3Size; ++i)
		TransformPoint(&sample3[i]);
}

void ReadPolygon()
{
	int edgeNumber;
	int i;

	printf("Please, enter number of points of polygon >: ");
	scanf("%d", &edgeNumber);
	getchar();

	printf("Now, enter %d points:\n", edgeNumber);

	for (i = 0; i < edgeNumber; ++i)
		Points[i] = ReadOnePoint();

	PointsSize = i;
}

int main()
{
	struct Point p;
	char c;
	int i = 0;
	int j = 0;

	InitializeScreen();
	InitSamples();

	printf("Menu:\n");
	printf("Enter '1' to print first sample.\n");
	printf("Enter '2' to print second sample.\n");
	printf("Enter '3' to print third sample.\n");
	printf("Enter 'c' to enter custom sample.\n");
	printf("Enter 'x' to exit.\n\n");
	printf(">: ");

	scanf("%c", &c);
	getchar();

	if ((char)tolower(c) == 'x')
		return 0;

	if ((char)tolower(c) == 'c')
	{
		printf("Reading a polygon...\n\n");
		ReadPolygon();

		printf("\nPlease, enter a point, from which polygon filling process should start:\n");
		p = ReadOnePoint();

		if (!IsPointInsidePolygon(Points, PointsSize, p.x, p.y))
		{
			printf("Your point does not belong to polygon!\n");
			printf("Error! Exiting...\n");
	
			printf("Press \"Enter\" to exit...");
			getch();
	
			return 0;
		}
		
		FillPolygon(Points, PointsSize, p.x, p.y);
	
		PrintScreen();
	}

	if (c == '1')
	{
		for (i = 0, j = sample1Size - 1; i < sample1Size; ++i)
		{
			DrawLine(sample1[j], sample1[i]);
			j = i;
		}

		PrintScreen();
		ClearScreen();
		FillPolygon(sample1, sample1Size, zeroPoint.x, zeroPoint.y);
		PrintScreen();
	}

	if (c == '2')
	{
		for (i = 0, j = sample2Size - 1; i < sample2Size; ++i)
		{
			DrawLine(sample2[j], sample2[i]);
			j = i;
		}

		PrintScreen();
		ClearScreen();
		FillPolygon(sample2, sample2Size, zeroPoint.x, zeroPoint.y);
		PrintScreen();
	}

	if (c == '3')
	{
		for (i = 0, j = sample3Size - 1; i < sample3Size; ++i)
		{
			DrawLine(sample3[j], sample3[i]);
			j = i;
		}

		PrintScreen();
		ClearScreen();

		FillPolygon(sample3, sample3Size, zeroPoint.x, zeroPoint.y);

		PrintScreen();
	}
	
	printf("Press \"Enter\" to exit...");
	getch();

	return 0;
}
