#include "SimpleSorts.h"
#include <memory.h>

/*
	‘ункции сортировки вставкой

	¬ходные параметры:
	arr - входной массив
	arrSize - размер входного массива
*/
void InsertSort(void* arr, int arrSize, int elementSize, CompareFunction compFunc)
{
	unsigned char* temp = (unsigned char*)malloc(elementSize);
	unsigned char* bytes = (unsigned char*)arr;

	unsigned char min = 0;
	int minIndex = 0;
	int i = 0, j = 0;

	// главный цикл
	for (i = 0; i < arrSize; ++i)
	{
		min = bytes[i*elementSize];
		minIndex = i;

		// на каждой итерации главного цыкла
		// найти минимальный елемент на 
		// оставшейс€ части масива
		for (j = i + 1; j < arrSize; ++j)
		{
			if (compFunc(&bytes[j*elementSize], &min) < 0)
			{
				min = bytes[j*elementSize];
				minIndex = j;
			}
		}

		// вставить очередной минимальный елемент
		// в начало масива, обмен€в его с данным елементом
		memcpy(temp, &bytes[i * elementSize], elementSize);
		memcpy(&bytes[i * elementSize], &bytes[minIndex * elementSize], elementSize);
		memcpy(&bytes[minIndex * elementSize], temp, elementSize);
	}

	free(temp);
}

/*
	функци€ пузырьковой сортировки

	¬ходные параметры:
	arr - входной массив
	arrSize - размер входного массива
*/
void BubbleSort(void* arr, int arrSize, int elementSize, CompareFunction compFunc)
{
	unsigned char* temp = (unsigned char*)malloc(elementSize);
	unsigned char* bytes = (unsigned char*)arr;

	int i, j;
	for (i = 0; i < arrSize; ++i)
		for (j = 0; j < arrSize - 1; ++j)
		{
			// каждый в свою очередь найбольшый елемент
			// передвигаетс€ к концу масива как пузырек
			if (compFunc(&bytes[j*elementSize], &bytes[(j + 1)*elementSize]) > 0)
			{
				memcpy(temp, &bytes[(j + 1) * elementSize], elementSize);
				memcpy(&bytes[(j + 1) * elementSize], &bytes[j * elementSize], elementSize);
				memcpy(&bytes[j * elementSize], temp, elementSize);
			}
		}

	free(temp);
}