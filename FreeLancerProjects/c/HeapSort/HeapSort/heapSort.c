#include "heapSort.h"
#include <memory.h>


/*
	Фукнция ReBuild восстанавливает 
	свойство пирамиды - каждый "отец"
	не меньше своих сыновей.

	Входние параметры:
	arr - входной масив
	first и last задают начало и конец части масива,
	для которого надо восстановить свойство.
*/
void ReBuild(void* arr, int first, int last, int elementSize, CompareFunction compFunc)
{
	unsigned char* temp = (unsigned char*)malloc(elementSize);
	unsigned char* bytes = (unsigned char*)arr;

	int k = 0;
	while (2*first + 2 <= last)
	{
		if (compFunc(&bytes[(2*first + 1)*elementSize], &bytes[(2*first + 2)*elementSize]) > 0)
			k = 2*first + 1;
		else
			k = 2*first + 2;

		// cын больше отца
		if (compFunc(&bytes[first * elementSize], &bytes[k * elementSize]) < 0)
		{
			// поменять их местами
			memcpy(temp, &bytes[k * elementSize], elementSize);
			memcpy(&bytes[k * elementSize], &bytes[first * elementSize], elementSize);
			memcpy(&bytes[first * elementSize], temp, elementSize);

			//и дальше перестроим пирамиду сына
			first = k;
		}
		else
			// отец не меньше сыновей - перестройка закончена
			break;
	}

	// после выхода из while возможны 2 условия
	// 2*first + 2 > last или arr[first] >= arr[k]
	if (2*first + 1 == last)
		// у arr[first] есть 1 сын
		if (compFunc(&bytes[first * elementSize], &bytes[last * elementSize]) < 0)
		{
			// поменять их местами
			memcpy(temp, &bytes[last * elementSize], elementSize);
			memcpy(&bytes[last * elementSize], &bytes[first * elementSize], elementSize);
			memcpy(&bytes[first * elementSize], temp, elementSize);
		}

	free(temp);
}


/*
	Фукция Build генерирует начальную 
	перестановку елементов масива arr так,
	чтобы она соответствовала пирамиде.

	arr - входной масив
	arrSize - его размер
*/
void Build(void* arr, int arrSize, int elementSize, CompareFunction func)
{
	int i;
	for (i = (arrSize/2) - 1; i >= 0; --i)
		// перестройка части масива
		ReBuild(arr, i, arrSize - 1, elementSize, func);
}


/*
	Функция пирамидальной сортировки

	Входные параметры:
	arr - входной массив
	arrSize - размер входного массива
	elemntSize - размер елемента масива
	compFunc - указатель на функцию сравнения елементов
*/
void HeapSort(void* arr, int arrSize, int elementSize, CompareFunction compFunc)
{
	int j;

	unsigned char* temp = (unsigned char*)malloc(elementSize);
	unsigned char* bytes = (unsigned char*)arr;

	// начальная перестановка
	Build(arr, arrSize, elementSize, compFunc);
	
	for (j = arrSize - 1; j >= 1; --j)
	{
		// "забыть" о максимальном 
		//  (убрать его с вершыны пирамиды)
		memcpy(temp, &bytes[j * elementSize], elementSize);
		memcpy(&bytes[j * elementSize], &bytes[0], elementSize);
		memcpy(&bytes[0], temp, elementSize);
		
		// восстановить условие 
		// "отец не меньше своих сыновей"
		ReBuild(arr, 0, j - 1, elementSize, compFunc);
	}

	free(temp);
}