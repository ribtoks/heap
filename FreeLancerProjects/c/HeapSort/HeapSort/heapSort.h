typedef int (CompareFunction) (const void*, const void*);

void HeapSort(void* arr, int arrSize, int elementSize, CompareFunction compFunc);
