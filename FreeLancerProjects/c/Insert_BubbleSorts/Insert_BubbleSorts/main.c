#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include "SimpleSorts.h"

// указатель на функцию, которая может сортировать 
// масив целых чисел
// первый параметр - масив
// второй параметр - размер масива
typedef void (*SortingFunction)(void*, int, int, CompareFunction);

#define ArraySize 10000

// число итераций проверки пузырьковой сортировки
#define bubbleIterationsCount 7

// число итераций проверки сортировки вставкой
#define insertIterationsCount 30

#define smallSize 100

// масив для сортировки своим heap-sort'ом
int ArrayForMineHeapSort[ArraySize];

// масив для сортировки готовым heap-sort'ом
int ArrayForSTLHeapSort[ArraySize];


// выводит масив Array длиной Size на консоль
void PrintArray(int Array[], int Size)
{
	int i = 0;
	for (i = 0; i < Size; ++i)
		printf("%d ", Array[i]);
	printf("\n");
}

/*
	Функция сравнения 2 чисел типа int
	Используется для стандартной функции сравнения
*/
int int_cmp(const void *a, const void *b)
{
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;

	/*
		Сравнение 2 чисел типа int: возвращаем отрицательное
		число если b > a, 0 если b == a и положительное число
		если a > b
	*/
    return *ia  - *ib; 	
}

/*
	Функция, которая проводит демонстративное сортирование 
	произвольного масива методом insert sort
*/
void ProvideDemo(SortingFunction func)
{
	int i = 0;
	int Array[smallSize];

	// сгенерировать масив
	for (i = 0; i < smallSize; ++i)
		Array[i] = rand();

	printf("----------- Sample of sorting -----------\n\n");

	printf("Input array:\n\n");
	// вивести на консоль
	PrintArray(Array, smallSize);

	// сортировать его
	func(Array, smallSize, sizeof(int), int_cmp);

	printf("\n\nSorted Array:\n\n");
	// вивести на консоль
	PrintArray(Array, smallSize);
	printf("\n");
}



/*
	Функция, которая принимает 2 масива одинакового размера,
	заполняет их случайными числами, а потом сортирует
	стандартной функцией и собственной.

	Возвращает разницу во времени работы 
	моего и стандартного алгоритмов
*/
double ProvideIteration(int Array1[], int Array2[], int Size, SortingFunction func)
{
	int i = 0;
	int tempRand = 0;

	double stlSortTime = 0;
	double mineSortTime = 0;

	// переменные для засекания времени
	clock_t start = 0, finish = 0;

	// заполнение масивов случайними числами
	for (i = 0; i < Size; ++i)
	{
		tempRand = rand();

		Array2[i] = tempRand;
		Array1[i] = tempRand;
	}	

	//------------------------------------------
	// здесь вичысляеться время работы стандартного алгоритма

	// засечь время начала
	start = clock();

	// сортировать масив стандартной функцией, опеределеной в stdlib.h
	qsort(Array2, Size, sizeof(int), int_cmp);

	// засечь время конца
	finish = clock();

	// вичислить время работы стандартного алгоритма
	stlSortTime = ((double)(finish - start) / CLOCKS_PER_SEC);

	//------------------------------------------

	//.......................................................................

	//------------------------------------------
	// здесь вичысляеться время работы моего алгоритма

	// засечь время начала
	start = clock();

	// сортировать масив собственным алгоритмом
	func(Array1, Size, sizeof(int), int_cmp);

	// засечь время конца
	finish = clock();

	// вичислить время работы своего алгоритма
	mineSortTime = ((double)(finish - start) / CLOCKS_PER_SEC);

	//------------------------------------------

	// возвратить разницу во времени работы двох алгоритмов
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

	// инициализировать генератор 
	// случайных чисел
	srand(time(0));

	printf("Bubble sort\n");

	printf("Enter your (y) array, or try to sort random (r) array? (y/r)");
	scanf("%c", &c);

	if (c == 'y')
	{
		printf("Enter number of elements in array >: ");		
		scanf("%d", &size);

		// виделить память для масива
		tempArray = (int*)malloc(size*sizeof(int));
		
		printf("Now enter %d elements of array:\n", size);

		// ввести масив из size елементов
		for (i = 0; i < size; ++i)
		{
			scanf("%d", &temp);
			tempArray[i] = temp;
		}

		// сортировать введенный масив
		BubbleSort(tempArray, size, sizeof(int), int_cmp);

		PrintArray(tempArray, size);

		// освободить использованную память
		free(tempArray);

		printf("\n");
	}
	else
		if (c == 'r')
		// вивести на консоль демонстративную сортировку масива
		ProvideDemo(BubbleSort);

	printf("--------- Compare my Bubble sort with standart sort algorithm ---------------\n");
	printf("Try to sort arrays for %d times:", bubbleIterationsCount);	

	// 300 раз провести сравнение времени работы двух функций 
	// сортировки на различных масивах
	for (i = 0; i < bubbleIterationsCount; ++i)
	{
		// на каждой итерации:

		// получить разницу во времени сортирования двумя алгоритмами сортировки
		// сумироть все разницы во времени
		deltasSum += ProvideIteration(ArrayForMineHeapSort, ArrayForSTLHeapSort, ArraySize, BubbleSort);

		printf(".");
	}

	printf("\n");
	
	// теперь в переменной deltaSum - разницы во времени сортировки
	// если значение больше 0 - значит наша собственная функция работае медленнее,
	// а именно на deltaSum секунд в среднем.
	// если значение меньше 0 - значит готовая функция работает быстрее на deltaSum секунд

	// вывод на консоль результата сравнения
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

		// виделить память для масива
		tempArray = (int*)malloc(size*sizeof(int));
		
		printf("Now enter %d elements of array:\n", size);

		// ввести масив из size елементов
		for (i = 0; i < size; ++i)
		{
			scanf("%d", &temp);
			tempArray[i] = temp;
		}

		// сортировать введенный масив
		InsertSort(tempArray, size, sizeof(int), int_cmp);

		PrintArray(tempArray, size);

		// освободить использованную память
		free(tempArray);

		printf("\n");
	}
	else
		if (c == 'r')
		// вивести на консоль демонстративную сортировку масива
		ProvideDemo(BubbleSort);

	printf("-------- Compare my Insert sort with standart sort algorithm ---------------\n");
	printf("Try to sort arrays for %d times:", insertIterationsCount);

	deltasSum = 0;

	// 300 раз провести сравнение времени работы двух функций 
	// сортировки на различных масивах
	for (i = 0; i < insertIterationsCount; ++i)
	{
		// на каждой итерации:

		// получить разницу во времени сортирования двумя алгоритмами сортировки
		// сумироть все разницы во времени
		deltasSum += ProvideIteration(ArrayForMineHeapSort, ArrayForSTLHeapSort, ArraySize, InsertSort);

		if (i % 4 == 0)
			printf(".");
	}

	printf("\n");
	
	// теперь в переменной deltaSum - разницы во времени сортировки
	// если значение больше 0 - значит наша собственная функция работае медленнее,
	// а именно на deltaSum секунд в среднем.
	// если значение меньше 0 - значит готовая функция работает быстрее на deltaSum секунд

	// вывод на консоль результата сравнения
	printf("On %d iterations difference in time \n between my insert sort and standart is: %f second(s)\n", 
		insertIterationsCount, deltasSum / insertIterationsCount);


	// задержать закривание консоли до того, как пользователь нажмет Enter
	printf("Press \"Enter\" to exit...");
	
	getch();

	return 0;
}