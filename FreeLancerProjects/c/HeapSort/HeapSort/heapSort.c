#include "heapSort.h"
#include <memory.h>


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

		if (compFunc(&bytes[first * elementSize], &bytes[k * elementSize]) < 0)
		{
			memcpy(temp, &bytes[k * elementSize], elementSize);
			memcpy(&bytes[k * elementSize], &bytes[first * elementSize], elementSize);
			memcpy(&bytes[first * elementSize], temp, elementSize);

			first = k;
		}
		else
			break;
	}

	if (2*first + 1 == last)
		if (compFunc(&bytes[first * elementSize], &bytes[last * elementSize]) < 0)
		{
			memcpy(temp, &bytes[last * elementSize], elementSize);
			memcpy(&bytes[last * elementSize], &bytes[first * elementSize], elementSize);
			memcpy(&bytes[first * elementSize], temp, elementSize);
		}

	free(temp);
}


void Build(void* arr, int arrSize, int elementSize, CompareFunction func)
{
	int i;
	for (i = (arrSize/2) - 1; i >= 0; --i)
		ReBuild(arr, i, arrSize - 1, elementSize, func);
}

void HeapSort(void* arr, int arrSize, int elementSize, CompareFunction compFunc)
{
	int j;

	unsigned char* temp = (unsigned char*)malloc(elementSize);
	unsigned char* bytes = (unsigned char*)arr;

	Build(arr, arrSize, elementSize, compFunc);
	
	for (j = arrSize - 1; j >= 1; --j)
	{
		memcpy(temp, &bytes[j * elementSize], elementSize);
		memcpy(&bytes[j * elementSize], &bytes[0], elementSize);
		memcpy(&bytes[0], temp, elementSize);
		
		ReBuild(arr, 0, j - 1, elementSize, compFunc);
	}

	free(temp);
}
