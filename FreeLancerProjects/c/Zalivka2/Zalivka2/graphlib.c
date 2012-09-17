#include <stdio.h>
#include <stdlib.h>
#include "graphlib.h"


typedef struct
{
	int y;
	int srcPointNum;
}
YVertex;

void FillStr (int y, int xBeg, int xEnd, char screen[SHEIGHT][SWIDTH])
{
	int x;
	for (x = xBeg; x <= xEnd; ++x)
	{

		if (screen[y][x] >= '0' && screen[y][x] < '9')
			screen[y][x] = screen[y][x] + 1;
		else
			screen[y][x] = '1';
	}
}




static int PxNext;



static int   aListLength;
static int   IYREB[MAXLST];
static float RXREB[MAXLST];
static float RPRIR[MAXLST];
static float RYSL[MAXLST];

static int CreateEdgeList (pixel points[], int count,
	int yStart, YVertex yVertexes[MAXARR], char screen[SHEIGHT][SWIDTH])
{
	int   i,inumedg,intek,intabs,isd;
	int   iyt,ixt,numofedge,inc,inpred,inposl;
	float dy;
	pixel t, c;


	inumedg = 0;
	for (i = PxNext; i <= count; ++i)
	{
		if (yVertexes[i].y != yStart)
			break;
		else ++inumedg;
	}

	for (i = 1; i <= inumedg; ++i)
	{
		intek = yVertexes[PxNext + i-1].srcPointNum;
		intabs = abs(intek);
		
		t = points[intabs];
		
		if ((inpred= intabs - 1) < 1) inpred = count;
		if ((inposl= intabs + 1) > count) inposl = 1;

		for (isd = 0; isd <= 1; ++isd)
		{
			if (!isd)
			{
				numofedge = inc = inpred;
			}
			else
			{
				inc = inposl;
				numofedge = intabs;
			}

			c.y = points[inc].y;
			dy = c.y - t.y;

			if (dy < 0.0) continue;

			c.x = points[inc].x;

			if (dy == 0.0)
			{
				if ((inc= yVertexes[numofedge].srcPointNum) < 0) continue;

				yVertexes[numofedge].srcPointNum = -inc;
				iyt = (int)t.y;
				inc = (int)c.x;
				ixt = (int)t.x;

				FillStr (iyt, inc, ixt, screen);

				continue;
			}

			++aListLength;
			IYREB[aListLength]= c.y;
			RXREB[aListLength]= t.x;
			RPRIR[aListLength]= (c.x - t.x) / dy;
			inc= (!isd) ? inposl : inpred;
			RYSL[aListLength]=  points[inc].y - t.y;
		}
	}

	PxNext += inumedg;
	i = PxNext;
	if (i > count) i = count;

	return (yVertexes[i].y);
}

void initYVertexes(pixel points[], int count, YVertex yVertexes[MAXARR])
{
	int i;
	for (i = 1; i <= count; ++i)
	{
		yVertexes[i].y = points[i].y;
		yVertexes[i].srcPointNum = i;
	}
}


void sortYVertexes(pixel points[], int count, YVertex yVertexes[MAXARR])
{
	int i, j, k, l;
	int yMin;

	for (i = 1; i <= count; ++i)
	{
		yMin = yVertexes[i].y;
		k = 0;
		
		for (j = i + 1; j <= count; ++j)
		{
			if ((l= yVertexes[j].y) < yMin)
			{
				yMin = l;
				k = j;
			}
		}

		if (k)
		{
			yVertexes[k].y = yVertexes[i].y;
			yVertexes[i].y = yMin;

			yMin = yVertexes[k].srcPointNum;
			yVertexes[k].srcPointNum = yVertexes[i].srcPointNum;
			yVertexes[i].srcPointNum = yMin;
		}
	}
}

void Fill(pixel points[], int count, char screen[SHEIGHT][SWIDTH])
{
	int  i,j,k,l;
	int  yCurrent;
	
	int  yStart;
	int  yMax;
	int  yNext;
	int  newysl;
	int  xMin;
	int  xCurrent;
	int  xCrossings[MAXLST];

	YVertex yVertexes[MAXARR];

	initYVertexes(points, count, yVertexes);

	sortYVertexes(points, count, yVertexes);

	aListLength= 0;
	PxNext= 1;
	yStart= yVertexes[1].y;
	yMax= yVertexes[count].y;

	yNext = CreateEdgeList (points, count, yStart, yVertexes, screen);
	if (!aListLength) return;

	for(;;)
	{
		for (yCurrent = yStart; yCurrent <= yNext; ++yCurrent)
		{
			if (yCurrent == yNext)
			{    
				newysl = CreateEdgeList (points, count, yCurrent, yVertexes, screen);
				if (!aListLength) return;
			}

			l = 0;
			for (i = 1; i <= aListLength; ++i)
			{
				if (RYSL[i] > 0.0)
					xCrossings[++l]= RXREB[i];
				else
					RYSL[i]= 1.0;
			}

			for (i = 1; i <= l; ++i)
			{
				xMin = xCrossings[i];
				k = 0;

				for (j = i + 1; j <= l; ++j)
				{
					xCurrent = xCrossings[j];
					if (xCurrent < xMin)
					{
						k = j;
						xMin = xCurrent;
					}
				}

				if (k)
				{
					xCrossings[k] = xCrossings[i];
					xCrossings[i] = xMin;
				}
			}

			for (j=1;  j<=l-1;  j+= 2)
				FillStr (yCurrent,xCrossings[j],xCrossings[j+1], screen);

			for (j=1;  j<=aListLength; ++j)
				RXREB[j]= RXREB[j] + RPRIR[j];
		}
		if (yNext == yMax) return;


		i = 0;
		while(i <= aListLength)
		{
			if (IYREB[i] != yNext)
			{
				++i;
				continue;
			}

			--aListLength;

			for (j = i; j <= aListLength; ++j)
			{
				k = j + 1;
				IYREB[j] = IYREB[k];
				RXREB[j] = RXREB[k];
				RPRIR[j] = RPRIR[k];
			}
		}

		yStart = yNext + 1;
		yNext = newysl;
	}

}  /* Fill*/
