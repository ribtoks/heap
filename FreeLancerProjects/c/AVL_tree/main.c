#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL_tree.h"

#define MAX_FILENAME_LENGTH 1000
#define MAX_WORD_LENGTH 101

char filename[MAX_FILENAME_LENGTH];
char word[MAX_WORD_LENGTH];

void ToLower(char* line)
{
   int i = 0;

   while (line[i])
   {
      if ( (line[i] >= 'A') && (line[i] <= 'Z') )
	 line[i] = 'a' + (line[i] - 'A');
      ++i;
   }
}

void ProcessFile(const char* filename, AVL_tree* stringTree)
{
   FILE* in;
   int length = MAX_WORD_LENGTH;
   int index = 0;
   int minus = 0;

   if ((in = fopen(filename, "r")) == NULL)
   {
      printf("Cannot open file.\n");
      return;
   }

   while (!feof(in))
   {
      index = 0;
      minus = 0;
      memset(word, '\0', MAX_WORD_LENGTH*sizeof(char));
      fscanf(in, "%s", word);

      length = strlen(word);

      if (length <= 1)
	 continue;

      if (word[length - 1] == ','
	 || word[length - 1] == '.'
	 || word[length - 1] == ')'
	 || word[length - 1] == '\"'
	 || word[length - 1] == ';'
	 || word[length - 1] == ':'
	 || word[length - 1] == ']')
      {
	 word[length - 1] = '\0';
	 ++minus;
      }

      if (word[length - 2] == '\"'
	 || word[length - 2] == '.'
	 || word[length - 2] == ')')
      {
	 word[length - 2] = '\0';
	 ++minus;
      }

      if (word[0] == '(' ||
	 word[0] == '\"' ||
	 word[0] == '[')
	 index = 1;

      if (length - minus - index <= 1)
	 continue;
      
      ToLower(word);      
      Add(stringTree, &word[index]);
   }

   fclose(in);
}

int main()
{
   int i = 0;
/*
   FuncFactory stringFunctions;
  
   AVL_tree* stringTree = 0;
   AVL_tree* tree = 0;
   int count = 0;


   InitStringFactoryStruct(&stringFunctions);
   tree = CreateAVLTree(&stringFunctions);

   memset(filename, '\0', MAX_FILENAME_LENGTH*sizeof(char));

   printf("Enter filename :> ");
   gets(filename);


   ProcessFile(filename, tree);

   printf("\nAll words:\n");


   Print_InOrder(tree);

   count = ElementCount(tree);
   printf("All words count: %d", count);
  
   CleanupTree(tree);

   printf("\nPress any key to exit...");
*/
   FuncFactory integerFunctions;

   AVL_tree* integerTree = 0;
   InitIntegerFactoryStruct(&integerFunctions);
   integerTree = CreateAVLTree(&integerFunctions);

   int arr[] = { 10, 2, 9, 1, 8, 6, 3, 7, 5, 4 };

   for (i = 0; i < 10; ++i)
   {
      Add(integerTree, &arr[i]);

//      PrintTree(integerTree);
//      printf("\r\n");
   }

   PrintTree(integerTree);
   printf("\r\n\r\n-------------------------------------------\r\n\r\n");

   Remove(integerTree, &arr[6]);
   PrintTree(integerTree);

   printf("\r\n\r\n-------------------------------------------\r\n\r\n");

   Remove(integerTree, &arr[3]);
   PrintTree(integerTree);

   CleanupTree(integerTree);
   
   
//   getchar();  
   return 0;
}
