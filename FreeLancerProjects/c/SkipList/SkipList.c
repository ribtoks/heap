#include "SkipList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void InitFactoryStruct(FuncFactory* factory, ConstructorFunc constructor,
		       DestructorFunc destructor,  CompareFunc comparator,
		       PrintFunc printer)
{
   factory->constructor = constructor;
   factory->compareFunc = comparator;
   factory->destructorFunc = destructor;
   factory->printFunc = printer;
}

void* StringConstructor(const void* data)
{
   const char* from = (const char*) data;
   char* str = 0;
  
   int n = strlen(from) + 1;
   str = (char*)malloc(n*sizeof(char));
   return strcpy(str, from);
}

int StringComparer(const void* line1, const void* line2)
{
   const char* str1 = (const char*) line1;
   const char* str2 = (const char*) line2;

   return strcmp(str1, str2);
}

void StringDestructor(void* data)
{
   free(data);
   data = 0;
}

void StringPrinter(const void* line)
{
   const char* str = (const char*)line;
   printf(str);
}

void InitStringFactoryStruct(FuncFactory* factory)
{
   InitFactoryStruct(factory,
		     StringConstructor,
		     StringDestructor,
		     StringComparer,
		     StringPrinter);
}



// -----------------------------------------
// ------------ Node realization -----------
// -----------------------------------------


Node* CreateNode()
{
   return (Node*)malloc(sizeof(Node));
}

void DeleteNode(Node* node)
{
   if (node->value)
   {
      free(node->value);
      node->value = 0;
   }

   free(node);
   node = 0;
}


// ---------------------------------------------
// ------------ SkipList realization -----------
// ---------------------------------------------

SkipList* CreateSkipList(FuncFactory* functions)
{
   SkipList* list = (SkipList*)malloc(sizeof(SkipList));
   int i = 0;

   list->header = CreateNode();
   list->header->forward = malloc((MAX_LEVEL + 1)*sizeof(Node*));

   for (i = 0; i <= MAX_LEVEL; ++i)
      list->header->forward[i] = list->header;

   list->level = 0;
   list->zero_node = 0;
   list->header->value = 0;
   list->functions = functions;
   
   return list;
}

Node* FindNode(const SkipList* list, const void* value)
{
   int i = 0;
   Node* curr = list->header;
   CompareFunc cmp = list->functions->compareFunc;

   for (i = list->level; i >=0; --i)
   {
      while ( (curr->forward[i] != list->header) &&
	      (cmp(curr->forward[i]->value, value) < 0) )
	 curr = curr->forward[i];
   }

   curr = curr->forward[0];

   if ((curr != list->header) && (cmp(curr->value, value) == 0))
      return curr;

   return list->zero_node;
}

void RemoveNode(SkipList* list, const void* value)
{
   int i;
   Node* update[MAX_LEVEL + 1];
   Node* curr;
   CompareFunc cmp = list->functions->compareFunc;

   curr = list->header;

   for (i = list->level; i >= 0; --i)
   {
      while ( (curr->forward[i] != list->header) &&
	      (cmp(curr->value, value) < 0) )
	 curr = curr->forward[i];
      update[i] = curr;
   }

   curr = curr->forward[0];

   if ( (curr == list->header) ||
	(cmp(curr->value, value) != 0))
      return;

   // adjust forward pointers
   for (i = 0; i <= list->level; ++i)
   {
      if (update[i]->forward[i] != curr)
	 break;

      update[i]->forward[i] = curr->forward[i];
   }

   DeleteNode(curr);

   // adjust header level
   while ( (list->level > 0) &&
	   (list->header->forward[list->level] == list->header))
      --(list->level);
}

Node* InsertNode(SkipList* list, const void* value)
{
   int i, newLevel;
   Node* update[MAX_LEVEL + 1];
   Node* curr;

   CompareFunc cmp = list->functions->compareFunc;


   curr = list->header;

   for (i = list->level; i >= 0; --i)
   {
      while ( curr->forward[i] != list->header &&
	      (cmp(curr->forward[i]->value, value) < 0))
	 curr = curr->forward[i];
      update[i] = curr;
   }

   curr = curr->forward[0];

   if ( (curr != list->header) &&
	(cmp(curr->value, value) == 0))
      return curr;

   for (newLevel = 0; (rand() < RAND_MAX/2) &&
	   (newLevel < MAX_LEVEL); ++newLevel);

   if (newLevel > list->level)
   {
      for (i = list->level + 1; i <= newLevel; ++i)
	 update[i] = list->header;
      list->level = newLevel;
   }


   // create node
   curr = CreateNode();
   curr->forward = malloc((newLevel + 1)*sizeof(Node*));
   curr->value = list->functions->constructor(value);

   for (i = 0; i <= newLevel; ++i)
   {
      curr->forward[i] = update[i]->forward[i];
      update[i]->forward[i] = curr;
   }

   return curr;
}

void PrintList(const SkipList* list)
{
   Node* curr = 0;

   if (!(list->header) || !(list->header->forward))
      return;

   curr = list->header->forward[0];

   while (curr != list->header)
   {
      list->functions->printFunc(curr->value);
      curr = curr->forward[0];

      if (curr != list->header)
	 printf("\n");
   }
   printf("\n");
}

void CleanupList(SkipList* list)
{
   Node* curr = list->header->forward[0];
   Node* node_to_delete;

   while (curr != list->header)
   {
      node_to_delete = curr;
      curr = curr->forward[0];

      DeleteNode(node_to_delete);
   }

   free(list->header->forward);
   DeleteNode(list->header);
   list->header = 0;
}

void PrintListToFile(FILE* out, PrintToFileFunc fileprinter, const SkipList* list)
{
   Node* curr = 0;

   if (!(list->header) || !(list->header->forward))
      return;

   curr = list->header->forward[0];

   while (curr != list->header)
   {
      fileprinter(out, curr->value);
      curr = curr->forward[0];

      if (curr != list->header)
	 fprintf(out, "\n");
   }
   fprintf(out, "\n");
}
