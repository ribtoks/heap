#pragma once
#include <stdio.h>
// function pointers

typedef void* (*ConstructorFunc)(const void*);
typedef void (*DestructorFunc)(void*);
typedef int (*CompareFunc)(const void*, const void*);
typedef void (*PrintFunc)(const void*);

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

void InitStringFactoryStruct(FuncFactory* factory);
void InitIntegerFactoryStruct(FuncFactory* factory);

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

// ------------------------------
// ------ Integers work ---------
// ------------------------------

void* IntegerConstructor(const void*);
int IntegerComparer(const void*, const void*);
void IntegerDestructor(void*);
void IntegerPrinter(const void*);

// ----------------------------------------------
// ------------ TreeNode declarations -----------
// ----------------------------------------------

// struct of tree node
struct TreeNodeStruct
{
   struct TreeNodeStruct* left_child;
   struct TreeNodeStruct* right_child;
   void* value;
   int valuecount;

   struct TreeNodeStruct* parent;
   int bal;

   int nodecount;
};

#define TreeNode struct TreeNodeStruct

TreeNode* CreateTreeNode();
bool IsARightChild(TreeNode*);
bool IsALeftChild(TreeNode*);
bool IsALeaf(TreeNode*);
void DeleteTreeNode(TreeNode*);
void PrintTreeNode(TreeNode*, PrintFunc, const int);
void PrintTreeNode_InOrder(TreeNode*, PrintFunc);


// ----------------------------------------------
// ------------ AVL tree declarations -----------
// ----------------------------------------------

struct AVL_tree_struct
{
   TreeNode* root;
   TreeNode* zero_node;
   FuncFactory* functions;
};
#define AVL_tree struct AVL_tree_struct

AVL_tree* CreateAVLTree(FuncFactory*);
bool Add(AVL_tree*, const void*);
void Remove(AVL_tree*, const void*);
int Size(const AVL_tree*);
bool Find(const AVL_tree*, const void*);
void PrintTree(const AVL_tree*);
void Print_InOrder(const AVL_tree*);
void CleanupTree(AVL_tree*);
int ElementCount(const AVL_tree*);

int NodeElementCount(const TreeNode*);
