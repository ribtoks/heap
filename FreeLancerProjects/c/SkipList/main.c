#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SkipList.h"

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

void ProcessFile(const char* filename, SkipList* stringList)
{
   FILE* in;
   int length = MAX_WORD_LENGTH;
   int index = 0;
   int minus = 0;

   if ((in = fopen(filename, "r")) == NULL)
   {
      printf("Cannot open file. Press any key to exit.\n");
      getchar();
      exit(-1);
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
      InsertNode(stringList, &word[index]);
   }

   fclose(in);
}

int AskUser(const char* askWhat)
{
   char c;

   printf(askWhat);
   printf(" (y/n) >: ");

   scanf("%c", &c);

   getchar();
   if (c == 'y' || c == 'Y')
      return 1;
   return 0;
}

void FilePrinter(FILE* out, const void* what)
{
   const char* line = (const char*)what;
   fprintf(out, line);
}

int main()
{
   FuncFactory stringFunctions;
  
   SkipList* list = 0;

   FILE* out = 0;

   InitStringFactoryStruct(&stringFunctions);
   list = CreateSkipList(&stringFunctions);

   memset(filename, '\0', MAX_FILENAME_LENGTH*sizeof(char));

   printf("Enter filename :> ");
   gets(filename);


   ProcessFile(filename, list);

   printf("\nAll words:\n");


   PrintList(list);

   if (AskUser("Print results to file?"))
   {
      if ((out = fopen("results.txt", "w+")) == NULL)
      {
	 printf("Cannot open file. Press any key to exit.\n");
	 getchar();
	 exit(-1);
      }

      PrintListToFile(out, FilePrinter, list);      
   }
  
   CleanupList(list);

   printf("Done.\n\nPress any key to exit...");
   getchar();  
   return 0;
}
