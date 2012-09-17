#include "graphlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


typedef struct
{
	pixel *points;
	int count;    
} Figure;


pixel Points1[MAXARR] = 
{
	{ 20.0, 20.0 },
	{ 30.0, 20.0 },
	{ 40.0, 40.0 },
	{ 30.0, 40.0 },
	{ 20.0, 20.0 }
};


pixel Points2[MAXARR] = 
{
	{ 20.0, 20.0 },
	{ 30.0, 32.0 },
	{ 20.0, 32.0 },
	{ 20.0, 20.0 }
};

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

Figure figures[] = 
{
	{ Points1, 4 },
	{ Points2, 3 },
	{ Points3, 7}
};

Figure custom;
pixel CustPoints[MAXARR];

int fquantity = 3;

Figure* chooseFigure()
{
	int number;
	int iter = 0;
	int i,j;
	int vert;
	float x,y;
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
		
	if (number == fquantity+2)
		return 0;
	else
		return &figures[number-1];
}

char EmptyPoint = '.';

int main (void)
{
	int i;
	char screen[SHEIGHT][SWIDTH] = {0};
	Figure* instance;

	while(instance = chooseFigure())
	{
		for (i = 0; i < SHEIGHT; i++)
			memset(screen[i], EmptyPoint, SWIDTH-1);

		

		Fill(instance->points, instance->count, screen);

		for (i = 0; i < SHEIGHT; i++)
			printf("%s\n", screen[i]);
	}

	system("PAUSE");
}
