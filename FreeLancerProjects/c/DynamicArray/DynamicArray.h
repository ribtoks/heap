#pragma once
#include <stdio.h>

typedef void* (*ConstructorFunc)(const void*);
typedef void (*DestructorFunc)(void*);
typedef int (*CompareFunc)(const void*, const void*);
typedef void (*PrintFunc)(const void*);

typedef void (*FilePrintFunc)(FILE*, const void*);

struct Functions_struct
{
   ConstructorFunc constructor;
   CompareFunc compareFunc;
   DestructorFunc destructorFunc;
   PrintFunc printFunc;
};

#define Functions struct Functions_struct

void InitFunctionsStruct(Functions* factory, ConstructorFunc constructor,
		       DestructorFunc destructor, CompareFunc comparator,
		       PrintFunc printer);

// now write apropriate functions for work with c-strings
void* StrConstruct(const void*);
/* 
   < 0 === [line1 < line2]
   = 0 === [line1 == line2]
   > 0 === [line1 > line2]
*/
int StrCompare(const void*, const void*);
void StrDestruct(void*);
void StrPrint(const void*);

void InitStringFactoryStruct(Functions*);

struct ArrayStruct
{
   Functions* functions;
   
   void** innerArray;
   
   int size;
   int bufferSize;
};

#define Array struct ArrayStruct

Array* CreateArray(Functions*);
void Add(Array*, const void*);
void Remove(Array*, const void*);
int GetSize(const Array*);
int FindIndex(const Array*, const void*);
void* GetElementAt(const Array*, int);
void DeleteArray(Array*);
void Sort(Array*);
void PrintArray(const Array* arr);
void PrintArrayToFile(FILE*, const Array* arr, FilePrintFunc);
