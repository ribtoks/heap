#pragma once

#include <stdio.h>

typedef void* (*ConstructorFunc)(const void*);
typedef void (*DestructorFunc)(void*);
typedef int (*CompareFunc)(const void*, const void*);
typedef void (*PrintFunc)(const void*);

typedef void (*PrintToFileFunc)(FILE*, const void*);

#define bool int
#define true 1
#define false 0

struct FuncFactory_struct
{
   ConstructorFunc constructor;
   CompareFunc compareFunc;
   DestructorFunc destructorFunc;
   PrintFunc printFunc;
};

#define FuncFactory struct FuncFactory_struct

void InitFactoryStruct(FuncFactory* factory, ConstructorFunc constructor,
		       DestructorFunc destructor, CompareFunc comparator,
		       PrintFunc printer);


// now write apropriate functions for work with c-strings
void* StringConstructor(const void*);
/* 
   < 0 === [line1 < line2]
   = 0 === [line1 == line2]
   > 0 === [line1 > line2]
*/
int StringComparer(const void*, const void*);
void StringDestructor(void*);
void StringPrinter(const void*);

void InitStringFactoryStruct(FuncFactory* factory);


struct NodeStruct
{
   void* value;
   struct NodeStruct** forward;
};

#define Node struct NodeStruct

Node* CreateNode();
void DeleteNode(Node*);

#define MAX_LEVEL 15


struct SkipListStruct
{
   Node* header;
   Node* zero_node;
   
   int level;

   FuncFactory* functions;
};

#define SkipList struct SkipListStruct

SkipList* CreateSkipList(FuncFactory*);
Node* InsertNode(SkipList*,const void*);
void RemoveNode(SkipList*, const void*);
Node* FindNode(const SkipList*, const void*);
void PrintList(const SkipList*);
void CleanupList(SkipList*);
void PrintListToFile(FILE*, PrintToFileFunc, const SkipList*);
