#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>
#include "heapSort.h"

#define ArraySize 20000

#define smallSize 100

#define iterationsCount 100

int ArrayForMineHeapSort[ArraySize];

int ArrayForSTLHeapSort[ArraySize];

double deltas[iterationsCount];

void PrintArray(int Array[], int Size)
{
	int i = 0;
	for (i = 0; i < Size; ++i)
		printf("%d ", Array[i]);
	printf("\n");
}


int int_cmp(const void *a, const void *b)
{
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;

    return *ia  - *ib; 	
}


void ProvideDemo()
{
	int Array[smallSize];

	int i = 0;

	for (i = 0; i < smallSize; ++i)
		Array[i] = rand();

	printf("----------- Sample of sorting -----------\n\n");

	printf("Input array:\n\n");
	PrintArray(Array, smallSize);

	HeapSort(Array, smallSize, sizeof(int), int_cmp);

	printf("\n\nSorted Array:\n\n");
	PrintArray(Array, smallSize);
	printf("\n");
}

double ProvideIteration(int Array1[], int Array2[], int Size)
{
	int i = 0;
	int tempRand = 0;

	double stlHeapSortTime = 0;
	double mineHeapSortTime = 0;

	clock_t start = 0, finish = 0;

	for (i = 0; i < Size; ++i)
	{
		tempRand = rand();

		Array2[i] = tempRand;
		Array1[i] = tempRand;
	}	


	start = clock();

	qsort(Array2, Size, sizeof(int), int_cmp);

	finish = clock();

	stlHeapSortTime = ((double)(finish - start) / CLOCKS_PER_SEC);




	start = clock();

	HeapSort(Array1, Size, sizeof(int), int_cmp);

	finish = clock();

	mineHeapSortTime = ((double)(finish - start) / CLOCKS_PER_SEC);


	return (mineHeapSortTime - stlHeapSortTime);
}

int main()
{
	double deltasSum = 0;
	int i = 0;
	char c;
	int size = 0;
	int temp = 0;
	int* tempArray = 0;

	srand(time(0));
	
	printf("Enter your (y) array, or try to sort random (r) array? (y/r)");
	scanf("%c", &c);

	if (c == 'y')
	{
		printf("Enter number of elements in array >: ");		
		scanf("%d", &size);

		tempArray = (int*)malloc(size*sizeof(int));
		
		printf("Now enter %d elements of array:\n", size);

		for (i = 0; i < size; ++i)
		{
			scanf("%d", &temp);
			tempArray[i] = temp;
		}

		HeapSort(tempArray, size, sizeof(int), int_cmp);

		PrintArray(tempArray, size);

		free(tempArray);

		printf("\n");
	}
	else
		if (c == 'r')
			ProvideDemo();

	printf("--------------- Compare my heap sort with standart ---------------\n");
	printf("Try to sort arrays for %d times:", iterationsCount);

	for (i = 0; i < iterationsCount; ++i)
	{

		deltas[i] = ProvideIteration(ArrayForMineHeapSort, ArrayForSTLHeapSort, ArraySize);

		deltasSum += (double)deltas[i];

		if (i % 4 == 0)
			printf(".");
	}

	printf("\n");
	

	printf("On %d iterations average difference in time \n between my heap sort and standart is: %f second(s)\n", iterationsCount, (deltasSum / iterationsCount));

	printf("Press \"Enter\" to exit...");
	getch();

	return 0;
}
