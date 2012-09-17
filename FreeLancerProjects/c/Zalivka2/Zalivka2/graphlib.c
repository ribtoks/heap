/*=====================================================
* Подпрограмма ефективной
* однотонной заливки многоугольника методом построчного
* сканирования.
*
* Дублирование занесения пикселов практически отсутствует
*
*/

#include <stdio.h>
#include <stdlib.h>
#include "graphlib.h"


/*
структура, которая сохраняет вертикаль начиная с у
длинной в srcPointNum - так званое ребро
*/
typedef struct
{
	int y;
	int srcPointNum;
}
YVertex;

// Заливает строку y от xBeg до xEnd
void FillStr (int y, int xBeg, int xEnd, char screen[SHEIGHT][SWIDTH])
{
	int x;
	for (x = xBeg; x <= xEnd; ++x)
	{
		//проверяет не закрашивалась область ранее. Если да - изменяем цвет
		if (screen[y][x] >= '0' && screen[y][x] < '9')
			screen[y][x] = screen[y][x] + 1;
		else
			screen[y][x] = '1';
	}
}  /* FillStr */



/*--------------- Глобальные переменные процедуры заливки ---------------*/
/*
Все переменные здесь статические с целью накопления результатов
*/

static int PxNext;        /* Номер след вершины в списке */



/* Список активных ребер */
static int   aListLength;  // Длина списка активных ребер
static int   IYREB[MAXLST]; /* Макс Y-коорд активных ребер */
static float RXREB[MAXLST]; /* Тек  X-коорд активных ребер */
static float RPRIR[MAXLST]; /* Х-приращение на 1 шаг по Y  */
static float RYSL[MAXLST];  /* Dy между тек и соседн верш  */
/* Dy <= 0.0 - обычная вершина */
/*     > 0.0 - локал экстремум */


/*---------------------------------------------------- CreateEdgeList
*  1) Формирует элементы списка для ребер,
*     начинающихся в yStart;
*  2) Вычиcляeт PxNext - индeкc нaчaлa следующей
*     вepшины в cпиcкe вepшин;
*  3) Возвращает IYSLED - Y кoopдинaтy ближaйшeй
*     вepшины, дo кoтopoй мoжнo зaливaть бeз
*     пepecтpoйки cпиcкa.
*
*  Глoбaльныe вeличины :
*
*  KOD    - код заливки
*  NWER   - кoл-вo вepшин в иcxoднoм мнoгoyгoльникe,
*  *pt_X  - X-кoopдинaты иcxoднoгo мнoгoyгoльника,
*  *pt_Y  - Y-кoopдинaты иcxoднoгo мнoгoyгoльника,
*  IEDG   - yпopядoчeнный пo вoзpacтaнию мaccив
*           Y кoopдинaт вepшин иcxoднoгo мнoгoyгoльн
*  INOM   - INOM[i] зaдaeт нoмep вepшины в иcxoднoм
*           мнoгoyгoльникe для IEDG[i],
*  PxNext - индeкc мaccивoв IEDG, INOM
*           oпpeдeляeт гдe мoжeт нaчaтьcя ребpo,
*  aListLength - длинa пocтpoeннoгo cпиcкa aктивныx ребep,
*           cocтoящeгo из :
*           IYREB  - мaкc кoopдинaты ребep,
*           RXREB  - внaчaлe мин, зaтeм тeкyщaя X-кoopдинaтa,
*           RPRIR  - пpиpaщeниe к X-кoopдинaтe нa 1 шaг пo Y,
*           RYSL   - пpизнaк тoгo чтo зa вepшинa :
*                    <= 0 - oбычнaя,
*                     > 0 - лoкaльный экcтpeмyм
*                     пepeceчeниe cтpoки зaкpacки
*                     c экcтpeмyмoм cчитaeтcя зa 2 тoчки,
*                     c oбычнoй - зa 1;
*/

static int CreateEdgeList (pixel points[], int count,
	int yStart, YVertex yVertexes[MAXARR], char screen[SHEIGHT][SWIDTH])
{
	int   i,inumedg,intek,intabs,isd;
	int   iyt,ixt,numofedge,inc,inpred,inposl;
	float dy;
	pixel t, c;

	/* inumedg = кoл-вo вepшин c дaнным yStart */

	inumedg = 0;
	for (i = PxNext; i <= count; ++i)
	{
		if (yVertexes[i].y != yStart)
			break;
		else ++inumedg;
	}

	/*
	Цикл пocтpoeния cпиcкa aктивныx ребep
	и зaкpaшивaниe гopизонтальных ребep
	*/

	for (i = 1; i <= inumedg; ++i)
	{
		/* Bычисл номера текущей вершины */
		intek = yVertexes[PxNext + i-1].srcPointNum;
		intabs = abs(intek);
		
		t = points[intabs];
		
		/*  Bычисл номеров предыд и послед вершин */
		if ((inpred= intabs - 1) < 1) inpred = count;
		if ((inposl= intabs + 1) > count) inposl = 1;

		/*
		* По заданным :
		*    NWER   - кол-во вершин,
		*    intek  - номер текущей вершины,
		*    isd = 0/1 - правилу выбора соседней вершины -
		*                предыдущая/последующая
		*    вычиcляeт dy,
		*    Еcли dy <  0 тo вepшинa yжe oбpaбoтaнa,
		*    Еcли dy == 0 тo вepшины нa oдном Y
		*                 Пpи этoм cтpoитcя гopизoнтaльный oтpeзoк.
		*                 Фaкт зaкpacки гopизoнтaльнoгo ребpa
		*                 oтмeчaeтcя oтpицaтeльным знaчeниeм
		*                 cooтвeтcтвyющeгo знaчeния INOM.
		*    Еcли dy >  0 тo фopмиpyeтcя нoвый элeмент cпиcкa
		*                 aктивныx ребep
		*/

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
		}   /* цикла по isd */
	}  /* построения списка активных ребер */

	/*  Bычисление Y ближайшей вершины */
	PxNext += inumedg;
	i = PxNext;
	if (i > count) i = count;

	return (yVertexes[i].y);
} /* Процедуры CreateEdgeList */

// Построение массивов Y и их номеров
void initYVertexes(pixel points[], int count, YVertex yVertexes[MAXARR])
{
	int i;
	for (i = 1; i <= count; ++i)
	{
		yVertexes[i].y = points[i].y;
		yVertexes[i].srcPointNum = i;
	}
}

// Сортировка массива yVertexes
void sortYVertexes(pixel points[], int count, YVertex yVertexes[MAXARR])
{
	int i, j, k, l;
	int yMin;  // Y-мин при сортировке массива Y-коорд

	for (i = 1; i <= count; ++i)
	{
		yMin = yVertexes[i].y;
		k = 0;
		
		//поиск наименьшего елемента в отрезке масиива [i+1..count]
		for (j = i + 1; j <= count; ++j)
		{
			if ((l= yVertexes[j].y) < yMin)
			{
				yMin = l;
				k = j;
			}
		}

		//замена і-го елемента на минимальный
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

// Однотонно заливает многоугольник, заданный координатами вершин
void Fill(pixel points[], int count, char screen[SHEIGHT][SWIDTH])
{
	int  i,j,k,l;
	int  yCurrent;    // Y текущей строки сканирования
	
	int  yStart;      // Минимальная Y-координата заливки
	int  yMax;        // Maксимальная Y-координата заливки
	int  yNext;       // Y кoopд ближaйшeй вepшины, дo кoтopoй
				      // можно зaливaть бeз пepecтpoйки cпиcкa
	int  newysl;
	int  xMin;        // X-минимальный при сортировке для тек строки
	int  xCurrent;    // X-текущий при сортировке для тек строки
	int  xCrossings[MAXLST]; // X-координаты пересечений в строке сканир

	// Y-коорд вершин по возрастанию и их номера в исходном массиве точек
	YVertex yVertexes[MAXARR];

	// Построение массивов Y и их номеров
	initYVertexes(points, count, yVertexes);

	// Сортировка массива yVertexes
	sortYVertexes(points, count, yVertexes);

	/* Hачальные присвоения */
	aListLength= 0;
	PxNext= 1;
	yStart= yVertexes[1].y;
	yMax= yVertexes[count].y;

	// Формирование начального списка акт ребер
	yNext = CreateEdgeList (points, count, yStart, yVertexes, screen);
	if (!aListLength) return;

	/* Горизонтальная раскраска по списку */
	for(;;)
	{
		for (yCurrent = yStart; yCurrent <= yNext; ++yCurrent)
		{
			/* Y-координата перестройки */
			if (yCurrent == yNext)
			{    
				newysl = CreateEdgeList (points, count, yCurrent, yVertexes, screen);
				if (!aListLength) return;
			}

			/* Bыборка и сортировка X-ов из списка ребер */
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
			}  /* цикла сортировки */

			/*  Cобственно заливка */

			for (j=1;  j<=l-1;  j+= 2)
				FillStr (yCurrent,xCrossings[j],xCrossings[j+1], screen);

			for (j=1;  j<=aListLength; ++j)        /*  Приращения X-ов */
				RXREB[j]= RXREB[j] + RPRIR[j];
		}  /* цикла горизонтальной раскраски */

		if (yNext == yMax) return;

		/*  Bыбрасывание из списка всех ребер с YMAX ребра == YSLED */

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
