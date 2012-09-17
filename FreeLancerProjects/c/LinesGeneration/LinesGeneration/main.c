#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "ScreenInterface.h"
#include "LinesGeneration.h"
#include "PointStruct.h"

#define BresenhamCoordChar 42
#define DDCCoordChar 4

struct Point possiblePoints[10000];

void PrintMenu()
{
	printf("Menu:\n");
	printf(" Enter 'p' to Print Screen to console\n");
	printf(" Enter 'c' to Clear Screen\n");

	printf(" Enter 'b' to draw a line using Bresenham algorithm (%c)\n", BresenhamCoordChar);
	printf(" Enter 'd' to draw a line using DDC algorithm (%c)\n", DDCCoordChar);

	printf(" Enter 'x' to exit\n");

	printf("\nYour choise >:");
}

struct Point ReadPoint()
{
	struct Point p;
	printf("Enter coordinates of point like \"x y\" >: ");
	scanf("%d %d", &p.x, &p.y);

	TransformPoint(&p);
	

	getchar();

	return p;
}


int main ()
{
	struct Point p1;
	struct Point p2;

	char c;
	int exit = 0;
	int size = 0, i = 0;

	InitScreen();
	
	while (1)
	{
		PrintMenu();
		
		scanf("%c", &c);
		getchar();
		
		switch (c)
		{
		case 'p':
			PrintScreen();
			break;

		case 'c':
			ClearScreen();
			break;

		case 'b':

			printf("Enter first point of segment:\n");
			p1 = ReadPoint();

			if (IsPointOk(p1.x, p1.y) == 0)
			{
				printf("\nThis point doesn't belong to screen! Try again.\n\n");
				break;
			}

			printf("Enter last point of segment:\n");
			p2 = ReadPoint();

			if (IsPointOk(p2.x, p2.y) == 0)
			{
				printf("\nThis point doesn't belong to screen! Try again.\n\n");
				break;
			}

			size = GenerateLineBresenham(p1, p2, possiblePoints);
			for (i = 0; i < size; ++i)
				SetPoint(possiblePoints[i].x, possiblePoints[i].y, BresenhamCoordChar);
			break;

		case 'd':

			printf("Enter first point of segment:\n");
			p1 = ReadPoint();

			if (IsPointOk(p1.x, p1.y) == 0)
			{
				printf("\nThis point doesn't belong to screen! Please, try again.\n\n");
				break;
			}

			printf("Enter last point of segment:\n");
			p2 = ReadPoint();

			if (IsPointOk(p2.x, p2.y) == 0)
			{
				printf("\nThis point doesn't belong to screen! Please, try again.\n\n");
				break;
			}

			size = GenerateLineBresenham(p1, p2, possiblePoints);
			for (i = 0; i < size; ++i)
				SetPoint(possiblePoints[i].x, possiblePoints[i].y, DDCCoordChar);
			break;

		case 'x':
			exit = 1;
			break;

		default:
			printf("Can not execute this action. Will close now.\n");
			exit = 1;
			break;
		}

		if (exit)
			break;
		else
			printf("Done...\n\n");
	}

	printf("Press \"Enter\" to exit...");
	getch();

	return 0;
}
