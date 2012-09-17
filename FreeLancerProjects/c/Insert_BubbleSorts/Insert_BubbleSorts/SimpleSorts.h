typedef int (CompareFunction) (const void*, const void*);

void InsertSort(void* arr, int arrSize, int elementSize, CompareFunction compFunc);

void BubbleSort(void* arr, int arrSize, int elementSize, CompareFunction compFunc);
