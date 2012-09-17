#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include "SimpleSorts.h"

typedef void (*SortingFunction)(void*, int, int, CompareFunction);

#define ArraySize 10000

#define bubbleIterationsCount 7

#define insertIterationsCount 30

#define smallSize 100

int ArrayForMineHeapSort[ArraySize];

int ArrayForSTLHeapSort[ArraySize];


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


void ProvideDemo(SortingFunction func)
{
	int i = 0;
	int Array[smallSize];

	for (i = 0; i < smallSize; ++i)
		Array[i] = rand();

	printf("----------- Sample of sorting -----------\n\n");

	printf("Input array:\n\n");
	PrintArray(Array, smallSize);

	func(Array, smallSize, sizeof(int), int_cmp);

	printf("\n\nSorted Array:\n\n");
	PrintArray(Array, smallSize);
	printf("\n");
}



double ProvideIteration(int Array1[], int Array2[], int Size, SortingFunction func)
{
	int i = 0;
	int tempRand = 0;

	double stlSortTime = 0;
	double mineSortTime = 0;

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

	stlSortTime = ((double)(finish - start) / CLOCKS_PER_SEC);




	start = clock();

	func(Array1, Size, sizeof(int), int_cmp);

	finish = clock();

	mineSortTime = ((double)(finish - start) / CLOCKS_PER_SEC);


	return (mineSortTime - stlSortTime);
}



int main()
{
	double deltasSum = 0;
	int i = 0;
	int* tempArray;
	int size = 0;
	int temp = 0;
	char c;

	srand(time(0));

	printf("Bubble sort\n");

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

		BubbleSort(tempArray, size, sizeof(int), int_cmp);

		PrintArray(tempArray, size);

		free(tempArray);

		printf("\n");
	}
	else
		if (c == 'r')
		ProvideDemo(BubbleSort);

	printf("--------- Compare my Bubble sort with standart sort algorithm ---------------\n");
	printf("Try to sort arrays for %d times:", bubbleIterationsCount);	

	for (i = 0; i < bubbleIterationsCount; ++i)
	{

		deltasSum += ProvideIteration(ArrayForMineHeapSort, ArrayForSTLHeapSort, ArraySize, BubbleSort);

		printf(".");
	}

	printf("\n");
	

	printf("On %d iterations difference in time \n between my bubble sort and standart is: %f second(s)", 
		bubbleIterationsCount, deltasSum/bubbleIterationsCount);



	printf("\n\n\n-------------------------------------------------------------------------\n\n\n\n");

	
	printf("Insert sort\n");
	getchar();
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

		InsertSort(tempArray, size, sizeof(int), int_cmp);

		PrintArray(tempArray, size);

		free(tempArray);

		printf("\n");
	}
	else
		if (c == 'r')
		ProvideDemo(BubbleSort);

	printf("-------- Compare my Insert sort with standart sort algorithm ---------------\n");
	printf("Try to sort arrays for %d times:", insertIterationsCount);

	deltasSum = 0;

	for (i = 0; i < insertIterationsCount; ++i)
	{

		deltasSum += ProvideIteration(ArrayForMineHeapSort, ArrayForSTLHeapSort, ArraySize, InsertSort);

		if (i % 4 == 0)
			printf(".");
	}

	printf("\n");
	

	printf("On %d iterations difference in time \n between my insert sort and standart is: %f second(s)\n", 
		insertIterationsCount, deltasSum / insertIterationsCount);


	printf("Press \"Enter\" to exit...");
	
	getch();

	return 0;
}
