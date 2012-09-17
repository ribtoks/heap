#include "DynamicArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void InitFunctionsStruct(Functions* factory, ConstructorFunc constructor,
		       DestructorFunc destructor,  CompareFunc comparator,
		       PrintFunc printer)
{
   factory->constructor = constructor;
   factory->compareFunc = comparator;
   factory->destructorFunc = destructor;
   factory->printFunc = printer;
}

void* StrConstruct(const void* data)
{
   const char* from = (const char*) data;
   char* str = 0;
  
   int n = strlen(from) + 1;
   str = (char*)malloc(n*sizeof(char));
   return strcpy(str, from);
}

int StrCompare(const void* line1, const void* line2)
{
   const char* str1 = (const char*) line1;
   const char* str2 = (const char*) line2;

   return strcmp(str1, str2);
}

void StrDestruct(void* data)
{
   free(data);
   data = 0;
}

void StrPrint(const void* line)
{
   const char* str = (const char*)line;
   printf(str);
}

void InitStringFactoryStruct(Functions* factory)
{
   InitFunctionsStruct(factory,
		     StrConstruct,
		     StrDestruct,
		     StrCompare,
		     StrPrint);
}

Array* CreateArray(Functions* functions)
{
   Array* arr = (Array*)malloc(sizeof(Array));

   arr->functions = functions;
   arr->size = 0;
   arr->bufferSize = 1;

   arr->innerArray = malloc(arr->bufferSize * sizeof(void*));

   return arr;
}

void ResizeArray(Array* arr)
{
   int newSize = (arr->bufferSize)*2 + 1;
   int i = 0;
   void** newArr = malloc(newSize*sizeof(void*));
   void** array_to_delete = 0;
   
   for (i = 0; i < arr->size; ++i)
      newArr[i] = arr->innerArray[i];

   array_to_delete = arr->innerArray;
   free(array_to_delete);

   arr->innerArray = newArr;
   arr->bufferSize = newSize;
}

void ShiftArrayLeft(Array* arr, int index)
{
   int i = 0;
   
   if (index < 0)
      return;

   for (i = index; i < arr->size - 1; ++i)
      arr->innerArray[i] = arr->innerArray[i + 1];

   arr->functions->destructorFunc(arr->innerArray[arr->size - 1]);
   --(arr->size);
}


void Add(Array* arr, const void* value)
{
   if (arr->size >= arr->bufferSize)
      ResizeArray(arr);

   arr->innerArray[arr->size] = arr->functions->constructor(value);
   ++(arr->size);
}

int FindIndex(const Array* arr, const void* value)
{
   int i = 0;
   CompareFunc cmp = arr->functions->compareFunc;

   for (i = 0; i < arr->size; ++i)
      if (cmp(arr->innerArray[i], value) == 0)
	 return i;

   return -1;
}

void* GetElementAt(const Array* arr, int index)
{
   return arr->innerArray[index];
}

void Remove(Array* arr, const void* value)
{
   int index = FindIndex(arr, value);

   if (index == -1)
      return;

   arr->functions->destructorFunc(arr->innerArray[index]);
   ShiftArrayLeft(arr, index);
}

void DeleteArray(Array* arr)
{
   int i = 0;

   for (i = 0; i < arr->size; ++i)
      arr->functions->destructorFunc(arr->innerArray[i]);

   free(arr->innerArray);
   arr->innerArray = 0;

   free(arr);
   arr = 0;
}

void QuickSort(void** arr, int first, int last, CompareFunc cmp)
{
   int left = first;
   int right = last;
   void* temp = 0;
   void* value = arr[(left + right) / 2];

   while (left <= right)
   {
      while (cmp(arr[left], value) < 0)
	 ++left;

      while (cmp(arr[right], value) > 0)
	 --right;

      if (left <= right)
      {
	 temp = arr[left];
	 arr[left] = arr[right];
	 arr[right] = temp;
	 
	 ++left;
	 --right;
      }
   }

   if (left < last)
      QuickSort(arr, left, last, cmp);

   if (right > first)
      QuickSort(arr, first, right, cmp);
}

void Sort(Array* arr)
{
   if (arr->size > 1)
      QuickSort(arr->innerArray, 0, arr->size - 1, arr->functions->compareFunc);
}


void PrintArray(const Array* arr)
{
   int i = 0;
   
   printf("[");

   for (i = 0; i < arr->size; ++i)
   {
      arr->functions->printFunc(arr->innerArray[i]);

      if (i < arr->size - 1)
	 printf(", ");
   }

   printf("]");
}


void PrintArrayToFile(FILE* out, const Array* arr, FilePrintFunc filePrint)
{
   int i = 0;
   
   fprintf(out, "[");

   for (i = 0; i < arr->size; ++i)
   {
      filePrint(out, arr->innerArray[i]);

      if (i < arr->size - 1)
	 fprintf(out, ", ");
   }

   fprintf(out, "]");
}
